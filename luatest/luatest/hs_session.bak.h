#ifndef __HS_SESSION_H__
#define __HS_SESSION_H__

#include <cstdlib>
#include <iostream>
#include <boost/aligned_storage.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "hs_define.h"
#include "hs_packetworker.h"
#include "hs_buffer.h"

using namespace std;
using boost::asio::ip::tcp;

class handler_allocator
	: private boost::noncopyable
{

public:
	handler_allocator() : in_use_(false)
	{
	}

	void* allocate(std::size_t size)
	{
		if (!in_use_ && size<storage_.size) {
			in_use_ = true;
			return storage_.address();
		} else {
			return ::operator new(size);
		}
	}

	void deallocate(void* pointer)
	{
		if (pointer == storage_.address()) {
			in_use_ = false;
		} else {
			::operator delete(pointer);
		}
	}

private:
	boost::aligned_storage<1024> storage_;
	bool in_use_;
};

template <typename Handler>
class custom_alloc_handler
{
public:
	custom_alloc_handler(handler_allocator& a, Handler h)
		: allocator_(a),
		  handler_(h)
	{
	}

	template <typename Arg1>
	void operator()(Arg1 arg1)
	{
		handler_(arg1);
	}

	template <typename Arg1, typename Arg2>
	void operator()(Arg1 arg1, Arg2 arg2)
	{
		handler_(arg1, arg2);
	}

	friend void* asio_handler_allocate(std::size_t size, custom_alloc_handler<Handler>* this_handler)
	{
		return this_handler->allocator_.allocate(size);
	}

	friend void asio_handler_deallocate(void* pointer, std::size_t size, custom_alloc_handler<Handler>* this_handler)
	{
		this_handler->allocator_.deallocate(pointer);
	}

private:
	handler_allocator& allocator_;
	Handler handler_;
};

template <typename Handler>
inline custom_alloc_handler<Handler> make_custom_alloc_handler(handler_allocator& a, Handler h)
{
	return custom_alloc_handler<Handler>(a, h);
}

class HSSession : public boost::enable_shared_from_this<HSSession>
{
public:
	HSSession(boost::asio::io_service& io_service)
		: socket_(io_service), recv_offset_(0), recv_used_offset_(0), send_offset_(0), sent_offset_(0)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		static size_t max_read_length = sizeof(recv_data_)/sizeof(char)/*-HS_NET_PACKET_MAX_SIZE*/;
		// ����ȡmax_read_length-recv_offset_���ֽ�
		socket_.async_read_some(boost::asio::buffer(recv_data_+recv_offset_, max_read_length-recv_offset_),
			make_custom_alloc_handler(allocator_, 
				boost::bind(&HSSession::handle_read,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));
	}

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		cout << "handle_read begin" << endl;
		if (!error) {

			// û���յ�����
			if (!bytes_transferred)
				return;

			size_t i = 0;
			cout << "received length: " << bytes_transferred << ", data: ";
			for (; i<bytes_transferred; ++i) {
				cout << recv_data_[recv_offset_+i] ;
			}
			cout << endl;

			cout << "recv_offset_: " << recv_offset_ << endl;

			// �Ѿ��յ�������
			recv_offset_ += bytes_transferred;

			// �Ƚ���ٽ���������
			size_t next_offset = 0;
			size_t out_size = 0;
			int res = -1;
			while (true) {
				res = HSPacketWorker::DecodePacket(recv_data_+recv_used_offset_, recv_offset_-recv_used_offset_, next_offset, out_size);
				
				// û�������İ���������������
				if (res == 0)
					break;

				// ���ݽ��������⣬�����ǿͻ��˷��������ݱ��������⣬��������������ݽ��մ�������
				if (res < 0) {
					// �Ͽ�����
					socket_.close();
					return;
				}

				recv_used_offset_ += next_offset;

				cout << "handle data: " << recv_data_ + recv_used_offset_ << ", length: " << out_size << endl;

				// �����û�����
				// HandleUserData(recv_data_+recv_used_offset_, out_size);

				// ����Ųout_size���ֽ�
				recv_used_offset_ += out_size;

				cout << "recv_used_offset_: " << recv_used_offset_ << endl;

				// ���ݴ������
				if (recv_offset_ == recv_used_offset_)
					break;
			}

			// ������ݴ�����ϣ���ƫ������Ϊ0
			if (res == 1) {
				recv_offset_ = 0;
				recv_used_offset_ = 0;
				cout << "������ϣ���������ƫ����" << endl;
			} else { // res == 0
				static size_t max_read_length = sizeof(recv_data_)/sizeof(char)-HS_NET_PACKET_MAX_SIZE;

				// ���ƫ��������max_read_length�����ʣ�µ�����Ų��recv_data_�Ŀ�ͷ
				if (recv_offset_ >= max_read_length) {
					cout << "recv_offset_ >= max_read_lengthʱ recv_offset_: " << recv_offset_ << endl;

					int new_offset = recv_offset_ - recv_used_offset_;
					memmove(recv_data_, recv_data_+recv_used_offset_, new_offset);
					recv_offset_ = new_offset;
					recv_used_offset_ = 0;

					cout << "new recv_offset_: " << recv_offset_ << endl;
				}
			}

			// ����������
			start();
		} else {
			// ������
			cout << "hadle_read error: " << error << endl;
		}
		cout << "handle_read end" << endl << endl;
	}

	void handle_write()
	{
		static size_t max_send_length = sizeof(send_data_)/sizeof(char);
		socket_.async_write_some(boost::asio::buffer(send_data_+sent_offset_, send_offset_-sent_offset_),
				make_custom_alloc_handler(allocator_,
					boost::bind(&HSSession::write_result,
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));
	}

	void write_result(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error) {
			if (!bytes_transferred)
				return;

			// �ѷ��͵���ƫ��
			sent_offset_ += bytes_transferred;

			// �������е����ݷ�����
			if (sent_offset_ == send_offset_) {
				sent_offset_ = 0;
				send_offset_ = 0;
			}
			// û���������
			else {
				handle_write();
			}
				
		} else {
			cout << "write_result error: " << error << endl;
		}
	}

	// ��������
	bool send(const char* data, size_t length)
	{
		static size_t s_send_buff_size = sizeof(send_data_)/sizeof(char);
		size_t out_size = 0;

		// ����������ݵ�����
		int res = HSPacketWorker::EncodePacket(data, length, send_data_+send_offset_, s_send_buff_size-send_offset_, out_size);
		// �������޷�����
		if (res == -1) {
			return false;
		}
		// �������������ȴ���һ�η���
		else if (res == 0) {
			
		}

		send_offset_ += out_size;
		handle_write();

		return true;
	}

private:
	tcp::socket socket_;
	char recv_data_[HS_NET_PACKET_MAX_SIZE*8];
	size_t recv_offset_;													// ���ջ���ƫ��
	size_t recv_used_offset_;												// ���ջ��崦��������ݵ�ƫ�ƣ�һ��������recv_offset_
	char send_data_[HS_NET_PACKET_MAX_SIZE*8];
	size_t sent_offset_;													// ʵ�ʷ��͵�����ƫ��
	size_t send_offset_;													// ���ͻ����ƫ�ƣ�һ����С��sent_offset_
	handler_allocator allocator_;
};

typedef boost::shared_ptr<HSSession> session_ptr;

#endif 