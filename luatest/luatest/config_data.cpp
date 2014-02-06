#include "config_data.h"

#include <iostream>

#include "tinyxml.h"
#include "HSSystem.h"

using namespace std;

ConfigData::ConfigData() : port_(0)
{
	memset(ip_, 0, sizeof(ip_));
}

bool ConfigData::Load(const char* file)
{
	TiXmlDocument doc;
	bool is_load = doc.LoadFile(file);

	if (!is_load) {
		cout << "ConfigData::Load " << file << " failed" << endl;
		return false;
	}

	TiXmlNode* node = NULL;
	node = doc.RootElement();
	if (!node) {
		cout << "ConfigData::Load " << file << " failed: no root node" << endl;
		return false;
	}

	TiXmlElement* elem = node->ToElement();
	if (!elem) {
		cout << "ConfigData::Load " << file << " failed: root node can not to element" << endl;
		return false;
	}

	if (strcmp("config", elem->Value()) != 0) {
		cout << "ConfigData::Load " << file << " failed: root node value error" << endl;
		return false;
	}

	node = node->FirstChild();
	while (node) {
		if (!node) {
			cout << "ConfigData::Load " << file << " failed: not found first child node" << endl;
			return false;
		}

		elem = node->ToElement();
		if (!elem) {
			cout << "ConfigData::Load " << file << " failed: first child for root node can not to element" << endl;
			return false;
		}

		const char* ev = elem->Value();
		if (strcmp(ev, "listen") == 0) {
			const char* ip = elem->Attribute("ip");
			short port = (short)atoi(elem->Attribute("port"));
	
			size_t s = strlen(ip)<sizeof(ip_) ? strlen(ip) : sizeof(ip_);
			memcpy(ip_, ip, s);
			if (s < sizeof(ip_))
				ip_[s] = '\0';
			port_ = port;
		} else if (strcmp(ev, "db") == 0) {
			dbserver_name_ = elem->Attribute("server_name");
			user_name_ = elem->Attribute("user_name");
			password_ = elem->Attribute("password");
		} else if (strcmp(ev, "gate_channel") == 0) {
			//const char* num = elem->Attribute("num");
			//if (num) {
			//	int n = atoi(num);
			//	if (n > 0) {
			ChannelData cd;
			TiXmlNode* node2 = node->FirstChild();
			while (node2) {
				elem = node2->ToElement();
				if (!elem) {
					cout << "ConfigData::Load" << file << "failed: [gate_channel] have no child node" << endl;
					return false;
				}
				const char* ev2 = elem->Value();
				if (strcmp(ev2, "item") != 0) {
					cout << "ConfigData::Load" << file << " failed: [gate_channel]'s child node is not [item]" << endl;
					return false;
				}
				const char* id = elem->Attribute("id");
				if (id) {
					cd.id = atoi(id);
				}
				const char* ls = elem->Attribute("listen");
				if (ls) {
					size_t s = strlen(ls)<sizeof(cd.listen_ip)-1?strlen(ls):sizeof(cd.listen_ip)-1;
					memcpy(cd.listen_ip, ls, s);
					cd.listen_ip[s] = '\0';
				}
				const char* co = elem->Attribute("connect");
				if (co) {
					size_t s = strlen(co) < sizeof(cd.connect_ip)-1 ? strlen(ls) : sizeof(cd.connect_ip)-1;
					memcpy(cd.connect_ip, co, s);
					cd.connect_ip[s] = '\0';
				}
				const char* po = elem->Attribute("port");
				if (po) {
					cd.port = (short)atoi(po);
				}
				gate_channels_.push_back(cd);
				node2 = node2->NextSibling();
			}
		}
		node = node->NextSibling();
	}

	return true;
} 