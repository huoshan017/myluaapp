#ifndef __HS_QUERY_CALLBACKS_H_20130816__
#define __HS_QUERY_CALLBACKS_H_20130816__

void CBRegisterAccount(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

void CBUnregisterAccount(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

void CBGetAllAccounts(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

void CBUserLogin(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

void CBCreateRole(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

void CBDeleteRole(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

void CBGetRoleList(bool error, void* handle, unsigned int* param, size_t param_len, const char* sparam);

#endif