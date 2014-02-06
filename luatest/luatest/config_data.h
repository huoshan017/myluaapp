#ifndef __CONFIG_DATA_20130802_H__
#define __CONFIG_DATA_20130802_H__

#include "singleton.hpp"
#include <string>
#include <vector>

using namespace std;

struct ChannelData
{
	int id;
	char listen_ip[32];
	char connect_ip[32];
	short port;
};

class ConfigData : public Singleton<ConfigData>
{
public:
	bool Load(const char* file);

	char* GetIP() { return (char*)ip_; }
	short GetPort() { return port_; }

	const char* GetDBServerName() { return dbserver_name_.c_str(); }
	const char* GetDBUserName() { return user_name_.c_str(); }
	const char* GetDBPassword() { return password_.c_str(); }

	size_t GetGateChannelCount() const { return gate_channels_.size(); }
	ChannelData& GetGateChannelData(size_t index) { return gate_channels_[index]; }

public:
	ConfigData();

private:
	char ip_[32];
	short port_;

	string dbserver_name_;
	string user_name_;
	string password_;

	vector<ChannelData> gate_channels_;
};

#endif