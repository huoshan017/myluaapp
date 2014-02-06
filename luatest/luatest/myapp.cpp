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

	// 载入lua文件

	// 读取配置文件
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
		v = data_row->GetHSValue("名称");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("解锁等级");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("图标");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("大类");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("小类");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("是否商城道具");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("参数");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("描述");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("buff类型");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("buff描述");
		if (v)
			cout << v->AsCString() << endl;
		v = data_row->GetHSValue("买入价格");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("解锁等级");
		if (v)
			cout << v->AsInt() << endl;
		v = data_row->GetHSValue("卖出价格");
		if (v)
			cout << v->AsInt() << endl;

		cout << "String Value: " << endl;
		cout << data_row->GetValue("ID") << endl;
		cout << data_row->GetValue("名称") << endl;
		cout << data_row->GetValue("解锁等级") << endl;
		cout << data_row->GetValue("图标") << endl;
		cout << data_row->GetValue("大类") << endl;
		cout << data_row->GetValue("小类") << endl;
		cout << data_row->GetValue("是否商城道具") << endl;
		cout << data_row->GetValue("参数") << endl;
		cout << data_row->GetValue("描述") << endl;
		cout << data_row->GetValue("buff类型") << endl;
		cout << data_row->GetValue("buff描述") << endl;
		cout << data_row->GetValue("买入价格") << endl;
		cout << data_row->GetValue("解锁等级") << endl;
		cout << data_row->GetValue("卖出价格") << endl;
	}*/

	file_path = m_strFilePath + "script\\init.lua";
	if (!m_lua.DoFile(file_path.c_str()))
		return false;

	file_path = m_strFilePath + "\\conf.xml";
	if (!ConfigData::Instance()->Load(file_path.c_str()))
		return false;

	// 数据库连接
	if (!HSODBC::Instance()->Connect(ConfigData::Instance()->GetDBServerName(), ConfigData::Instance()->GetDBUserName(), ConfigData::Instance()->GetDBPassword()))
		return false;

	// 网络层监听
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
	// 收发数据
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