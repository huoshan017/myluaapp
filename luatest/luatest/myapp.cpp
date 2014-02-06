#include "myapp.h"
#include "HSConfigReader.h"
#include "HSSystem.h"
#include "hs_session.h"
#include "hs_server.h"
#include "config_data.h"
#include "hs_test_eventhandler.h"
#include "hs_odbc.h"
#include "hs_service.h"


#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#endif

using boost::asio::ip::tcp;

MyApp::MyApp() : client_server_(NULL)
{
}

MyApp::~MyApp()
{
}

bool MyApp::Init()
{
	if (!m_lua.Init())
		return false;

	// ����lua�ļ�

	// ��ȡ�����ļ�
	HSConfigFile<int> file("ID");
	file.SetDelimiters("\t");

	string file_path = HSSystem::GetCurrentDir();
	m_strFilePath = file_path;
	file_path += "data\\item.txt";
	if (!file.Load(file_path.c_str()))
		return false;

	HSConfigFile<int>::DataRow* data_row = file.GetDataRow(20001);
	/*if (data_row) {
		cout << "HSValue: " << endl;
		HSValue* v = data_row->GetHSValue("ID");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("����");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("�����ȼ�");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("ͼ��");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("����");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("С��");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("�Ƿ��̳ǵ���");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("����");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("����");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("buff����");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("buff����");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("����۸�");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("�����ȼ�");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("�����۸�");
		if (v)
			cout << v->AsInt() << endl;

		cout << "String Value: " << endl;
		cout << data_row->GetValue("ID") << endl;
		cout << data_row->GetValue("����") << endl;
		cout << data_row->GetValue("�����ȼ�") << endl;
		cout << data_row->GetValue("ͼ��") << endl;
		cout << data_row->GetValue("����") << endl;
		cout << data_row->GetValue("С��") << endl;
		cout << data_row->GetValue("�Ƿ��̳ǵ���") << endl;
		cout << data_row->GetValue("����") << endl;
		cout << data_row->GetValue("����") << endl;
		cout << data_row->GetValue("buff����") << endl;
		cout << data_row->GetValue("buff����") << endl;
		cout << data_row->GetValue("����۸�") << endl;
		cout << data_row->GetValue("�����ȼ�") << endl;
		cout << data_row->GetValue("�����۸�") << endl;
	}*/

	file_path = m_strFilePath + "script\\init.lua";
	if (!m_lua.DoFile(file_path.c_str()))
		return false;

	file_path = m_strFilePath + "\\conf.xml";
	if (!ConfigData::Instance()->Load(file_path.c_str()))
		return false;

	// ���ݿ�����
	if (!HSODBC::Instance()->Connect(ConfigData::Instance()->GetDBServerName(), ConfigData::Instance()->GetDBUserName(), ConfigData::Instance()->GetDBPassword()))
		return false;

	// ��������
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), ConfigData::Instance()->GetPort());
	client_server_ = new HSClientServer(HSServiceMgr::Instance()->get_ios(0));
	client_server_->set_eventhandler(new HSTestEventHandler);
	client_server_->start(ep);
	HSServiceMgr::Instance()->start_service(0);

	size_t c = ConfigData::Instance()->GetGateChannelCount();
	if (c == 0)
		return false;

	gate_channels_.resize(c);
	size_t i = 0;
	for (; i<c; ++i) {
		ChannelData& cd = ConfigData::Instance()->GetGateChannelData(i);
		//boost::asio::ip::tcp::endpoint ep2(boost::asio::ip::address_v4::from_string(cd.connect_ip), cd.port);
		//gate_channels_[i] = new HSChannel(HSServiceMgr::Instance()->get_ios(0));
		//gate_channels_[i]->init(ep2, &gate_eventhandler_, cd.id);
		//gate_channels_[i]->connect(CHANNEL_CONNECT_DEFAULT_TIMEOUT);
		gate_channels_[i].init(cd.listen_ip, cd.port);
		gate_channels_[i].set_timeout(0, 0);
		gate_channels_[i].connect();
	}
	//HSServiceMgr::Instance()->start_service(1);

	return true;
}

void MyApp::Close()
{
	HSODBC::Instance()->Disconnect();

	m_lua.Close();
}

bool MyApp::Run()
{
	// �շ�����
	//m_client_server.poll();

	size_t c = ConfigData::Instance()->GetGateChannelCount();
	size_t i = 0;
	for (; i<c; ++i) {
		gate_channels_[i].poll();
	}

	HSODBC::Instance()->Doing();

	//string file_path = m_strFilePath + "main.lua";
	//m_lua.DoFile(file_path.c_str());
	m_lua.call_va("run", "");

	return true;
}