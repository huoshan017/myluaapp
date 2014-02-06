#ifndef _MY_APP_20130526_H_
#define _MY_APP_20130526_H_

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "hs_server.h"
#include "hs_server_channel.h"
#include "MyLuaWrapper.h"
#include "hs_test_eventhandler.h"

using namespace std;


class MyApp
{
public:
	MyApp();
	~MyApp();

	bool Init();
	void Close();
	bool Run();

private:
	MyLuaWrapper m_lua;
	string m_strFilePath;
	HSClientServer* client_server_;
	//vector<HSChannel*> gate_channels_;
	vector<hs_server_channel> gate_channels_; 
	HSGateEventHandler gate_eventhandler_;
};

#endif