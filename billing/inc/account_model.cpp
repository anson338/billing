#include "account_model.hpp"
#include <cstring>

AccountModel::AccountModel(std::shared_ptr<MYSQL> mysql) :mysqlHandler(mysql)
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("AccountModel constrcut");
#endif
}

AccountModel::~AccountModel()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("AccountModel destrcut");
#endif
}

unsigned char AccountModel::getLoginResult(string & username, string & password)
{
	AccountInfo accountInfo;
	this->getAccountInfo(username, accountInfo);
	if (accountInfo.id == 0) {
		//用户不存在
		return 9;
	}
	//密码验证
	if (strcmp(accountInfo.password, password.c_str()) != 0) {
		//密码错误
		return 3;
	}
	if (accountInfo.is_lock != 0) {
		//停权
		return 7;
	}
	if (accountInfo.is_online != 0) {
		//有角色在线
		return 4;
	}
	//更新状态为在线模式
	this->updateOnlineStatus(username, true);
	return 1;
}

const unsigned int AccountModel::getUserPoint(string & username)
{
	AccountInfo accountInfo;
	this->getAccountInfo(username, accountInfo);
	if (accountInfo.id != 0) {
		return accountInfo.point;
	}
	return 0;
}

void AccountModel::getAccountInfo(string & username, AccountInfo& accountInfo)
{
	auto mysql = this->getMysql();
	auto stmt = mysql_stmt_init(mysql);
	if (!stmt) {
		//out of memory
		return;
	}
	const char* sql = "SELECT id,name,password"
		",question,answer,email,qq,point,is_online,is_lock"
		" FROM account WHERE name=?";
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_prepare() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	unsigned long nameSize = username.length();
	MYSQL_BIND bind[1];
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (void*)username.c_str();
	bind[0].buffer_length = nameSize + 1;
	bind[0].is_null = 0;
	bind[0].length = &nameSize;
	if (mysql_stmt_bind_param(stmt, bind)) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_bind_param() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	auto prepare_meta_result = mysql_stmt_result_metadata(stmt);
	if (!prepare_meta_result)
	{
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_result_metadata() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	if (mysql_stmt_execute(stmt))
	{
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_execute() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	//
	MYSQL_BIND    bindr[10];
	memset(bindr, 0, sizeof(bindr));
	unsigned long length[10];
	my_bool       is_null[10];
	my_bool       error[10];
	int i = 0;
	bindr[i].buffer_type = MYSQL_TYPE_LONG;
	bindr[i].buffer = &accountInfo.id;
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_STRING;
	bindr[i].buffer = accountInfo.name;
	bindr[i].buffer_length = sizeof(accountInfo.name);
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_STRING;
	bindr[i].buffer = accountInfo.password;
	bindr[i].buffer_length = sizeof(accountInfo.password);
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_STRING;
	bindr[i].buffer = accountInfo.question;
	bindr[i].buffer_length = sizeof(accountInfo.question);
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_STRING;
	bindr[i].buffer = accountInfo.answer;
	bindr[i].buffer_length = sizeof(accountInfo.answer);
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_STRING;
	bindr[i].buffer = accountInfo.email;
	bindr[i].buffer_length = sizeof(accountInfo.email);
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_STRING;
	bindr[i].buffer = accountInfo.qq;
	bindr[i].buffer_length = sizeof(accountInfo.qq);
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_LONG;
	bindr[i].buffer = &accountInfo.point;
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_SHORT;
	bindr[i].buffer = &accountInfo.is_online;
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	i++;
	bindr[i].buffer_type = MYSQL_TYPE_SHORT;
	bindr[i].buffer = &accountInfo.is_lock;
	bindr[i].is_null = &is_null[i];
	bindr[i].length = &length[i];
	bindr[i].error = &error[i];
	//
	if (mysql_stmt_bind_result(stmt, bindr))
	{
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_bind_result() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	if (mysql_stmt_store_result(stmt) != 0) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_store_result() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
#ifdef OPEN_SERVER_DEBUG
	int result = mysql_stmt_fetch(stmt);
	if ((result != 0) && (result != MYSQL_NO_DATA)) {
		Logger::write("mysql_stmt_fetch() failed");
		Logger::write(std::to_string(result));
	}
#else
	mysql_stmt_fetch(stmt);
#endif
	if (mysql_stmt_free_result(stmt)) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_free_result() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	mysql_stmt_close(stmt);
}

void AccountModel::updateLockStatus(string & username, bool isLock)
{
	unsigned short status = isLock ? 1 : 0;
	auto mysql = this->getMysql();
	auto stmt = mysql_stmt_init(mysql);
	if (!stmt) {
		//out of memory
		return;
	}
	const char* sql = "UPDATE account SET is_lock=?  WHERE name=?";
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_prepare() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	MYSQL_BIND bind[2];
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_SHORT;
	bind[0].buffer = &status;
	bind[0].is_null = 0;
	bind[0].length = 0;
	unsigned long nameSize = username.length();
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (void*)username.c_str();
	bind[1].buffer_length = nameSize + 1;
	bind[1].is_null = 0;
	bind[1].length = &nameSize;
	if (mysql_stmt_bind_param(stmt, bind)) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_bind_param() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	if (mysql_stmt_execute(stmt))
	{
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_execute() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	mysql_stmt_close(stmt);
}

void AccountModel::updateOnlineStatus(string & username, bool isOnline)
{
	unsigned short status = isOnline ? 1 : 0;
	auto mysql = this->getMysql();
	auto stmt = mysql_stmt_init(mysql);
	if (!stmt) {
		//out of memory
		return;
	}
	const char* sql = "UPDATE account SET is_online=?  WHERE name=?";
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_prepare() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	MYSQL_BIND bind[2];
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_SHORT;
	bind[0].buffer = &status;
	bind[0].is_null = 0;
	bind[0].length = 0;
	unsigned long nameSize = username.length();
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (void*)username.c_str();
	bind[1].buffer_length = nameSize + 1;
	bind[1].is_null = 0;
	bind[1].length = &nameSize;
	if (mysql_stmt_bind_param(stmt, bind)) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_bind_param() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	if (mysql_stmt_execute(stmt))
	{
#ifdef OPEN_SERVER_DEBUG
		Logger::write("mysql_stmt_execute() failed");
		Logger::write(mysql_stmt_error(stmt));
#endif
		return;
	}
	mysql_stmt_close(stmt);
}
