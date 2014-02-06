#include "hs_query_callbacks.h"
#include "hs_sessionmgr.h"
#include "hs_odbc.h"

void CBRegisterAccount(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam)
{
	HSODBC::DataHandle* h = (HSODBC::DataHandle*)handle;
	unsigned int session_id = param[0];
	session_ptr sp = HSSessionMgr::Instance()->get_session(session_id);
	if (sp.get() == NULL) {
		return;
	}
	h->GetStatementHandle();
}

void CBUnregisterAccount(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam)
{
	HSODBC::DataHandle* h = (HSODBC::DataHandle*)handle;
}

void CBGetAllAccounts(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam)
{
	HSODBC::DataHandle* h = (HSODBC::DataHandle*)handle;
	unsigned int session_id = param[0];
	session_ptr sp = HSSessionMgr::Instance()->get_session(session_id);
	if (sp.get() == NULL) {
		return;
	}
	if (error) {
		cout << "��������û�ʧ��" << endl;
		return;
	}

	char buf[1024];
	while (h->Fetch()) {
		// �û���
		h->GetDataString(2, buf, sizeof(buf)-1);
		cout << "username: " << buf;
		// ����
		h->GetDataString(3, buf, sizeof(buf)-1);
		cout << ", password: " << buf << endl;
	}
}

void CBUserLogin(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam)
{
	HSODBC::DataHandle* h = (HSODBC::DataHandle*)handle;
	unsigned int session_id = param[0];
	const char* user_name = sparam;
	session_ptr sp = HSSessionMgr::Instance()->get_session(session_id);

	if (error) {
		cout << "�û� " << user_name << " ��¼ʧ��" << endl;
		return;
	}

	size_t c = 0;
	if (h->GetRowCount(c)) {
		if (c == 0) {
			cout << "�û�" << user_name << "������" << endl;
			return;
		}
		if (h->Fetch()) {
			short s = 0;
			h->GetDataShort(1, s);
			if (s == 1) {
				cout << "�û�" << user_name << "�Ѿ���¼" << endl;
				// �ߵ�
			} else {
				cout << "�û�" << user_name << "��½�ɹ�" << endl;
			}
		}
	}
}