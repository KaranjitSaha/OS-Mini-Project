#ifndef DBOPS_H
#define DBOPS_H
#include"../include/datastructures.h"
#include"../include/syscalls.h"
#include <string.h>

User getUser(int user_id);
int getAccountId(int user_id);
int db_deposit(int amount,int user_id);
int db_withdraw(int amount,int user_id);
int db_balance(int user_id);
int *db_account_details(int user_id);
int db_password_change(char* newpassword,int user_id);
int db_add_account(int account_no);
int db_modify_admin_password(char* newpassword,char* username);
int db_modify_account_type(int account_no,int account_type);
int db_delete_account(int account_no);
int validateLogin(Credentials *logincred);
int ValidateAdminLogin(Admin* logincred);
int validatejointLogin(Credentials *logincred);
#endif







