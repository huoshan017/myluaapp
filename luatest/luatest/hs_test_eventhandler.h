#ifndef __HS_TEST_EVENT_HANDLER_H_20130814__
#define __HS_TEST_EVENT_HANDLER_H_20130814__

#include "hs_eventhandler.h"
#include "hs_session.h"
#include "hs_sessionmgr.h"
#include "hs_odbc.h"
#include "hs_query_callbacks.h"
#include "HSSystem.h"

#include "json/json.h"

class HSTestEventHandler : public HSEventHandler
{
public:
	virtual void onConnect(unsigned int session_id)
	{
		cout << "HSTestEventHandler::onConnect" << endl;
	}

	virtual void onDisconnect(unsigned int session_id)
	{
		cout << "HSTestEventHandler::onDisconnect" << endl;
	}

	virtual void onError(unsigned int session_id, int error)
	{
		cout << "HSTestEventHandler::onError" << endl;
	}
 
	virtual void onReceive(unsigned int session_id, const char* data, size_t data_len)
	{
		cout << "HSTestEventHandler::onReceive" << endl;

		Json::Reader reader;
		Json::Value json_object;
		Json::Value proto_id = Json::nullValue;
		bool is_proto = false;
		
		string str(data, data_len);
		if (reader.parse(str, json_object)) {
			if (json_object.type() == Json::nullValue || json_object.type() == Json::objectValue) {

				static HSSqlCallback cb;
				static char sql_buf[1024];

				proto_id = json_object["proto_id"];
				// 注册用户
				if (proto_id == "register_account_req") {
					Snprintf(sql_buf, sizeof(sql_buf)-1, "INSERT INTO my_server.account (username, password) VALUES ('%s', '%s')",
						json_object["user_name"].asCString(), json_object["password"].asCString());
					
					cb.param[0] = session_id;
					cb.callback = CBRegisterAccount;
					is_proto = true;
				}
				// 注销用户
				else if (proto_id == "unregister_account_req") {
					Snprintf(sql_buf, sizeof(sql_buf)-1, "DELETE FROM my_server.account WHERE username='%s'",
						json_object["user_name"].asCString());

					cb.param[0] = session_id;
					cb.callback = CBUnregisterAccount;
					is_proto = true;
				}
				// 获得所有用户
				else if (proto_id == "get_all_accounts_req") {
					Snprintf(sql_buf, sizeof(sql_buf)-1, "SELECT * FROM my_server.account");
					cb.param[0] = session_id;
					cb.callback = CBGetAllAccounts;
					is_proto = true;
				}
				// 登录
				else if (proto_id == "login_req") {
					Snprintf(sql_buf, sizeof(sql_buf)-1, "SELECT logined FROM my_server.account WHERE username='%s', password='%s'",
						json_object["user_name"].asCString(), json_object["password"].asCString());
					cb.param[0] = session_id;
					cb.sparam = json_object["user_name"].asCString();
					cb.callback = CBUserLogin;
					is_proto = true;
				}
				else {
					cout << "other proto_id: " << proto_id << endl;
				}

				if (is_proto) {
					HSODBC::Instance()->ExecuteQuery(sql_buf, strlen(sql_buf), cb);
				}
			}
		}
		if (!is_proto) { 
			DefaultHandleData(session_id, data, data_len);
		}
	}

private:
	void DefaultHandleData(unsigned int session_id, const char* data, size_t data_len)
	{
		session_ptr s = HSSessionMgr::Instance()->get_session(session_id);
		if (s.get() == NULL)
			return;

		s->send(data, data_len);
	}
};

class HSGateEventHandler : public HSEventHandler
{
public:
	virtual void onConnect(unsigned int session_id)
	{
		cout << "HSGateEventHandler::onConnect" << endl;
		session_ptr sp = HSSessionMgr::Instance()->get_local_session(session_id);
		sp->send("echo", strlen("echo"));


	}

	virtual void onDisconnect(unsigned int session_id)
	{
		cout << "HSGateEventHandler::onDisconnect" << endl;
	}

	virtual void onError(unsigned int session_id, int error)
	{
		cout << "HSGateEventHandler::onError" << endl;
	}
 
	virtual void onReceive(unsigned int session_id, const char* data, size_t data_len)
	{
		cout << "HSGateEventHandler::onReceive" << endl;
		cout << "receive: ";
		for (size_t i=0; i<data_len; ++i) {
			cout << *(data+i);
		}
		cout << endl;
	}

	virtual void onPoll()
	{
		//cout << "HSGateEventHandler::onPoll" << endl;
	}
};

#endif