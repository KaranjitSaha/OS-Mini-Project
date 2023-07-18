#ifndef SERVER_H
#define SERVER_H

int mainController(int client,int userID,long cookie);
int adminController(int client,long cookie);
int viewAccountDetails(int client);
int modify_admin_password(int client);
int modify_account_type(int client);
int delete_account(int client);
int add_account(int client);
int client_exit(int client,int userID);
int password_change(int client, int userID);
int view_details(int client, int userID);
int balance(int client, int userID);
int deposit(int client, int userID);
int withdraw(int client, int userID);
long GenerateCookie(int val);
int normallogin(int client);
int adminlogin(int client);
int loginHandler(int client);
int connectSocket();
int connectClient(int socket_d);

#endif 