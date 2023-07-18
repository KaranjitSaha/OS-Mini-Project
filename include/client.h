#ifndef CLIENT_H
#define CLIENT_H
void ExitHandler(long cookie,int socket_d);
void AccountHandler(long cookie, int socket_d);
void PasswordHandler(long cookie, int socket_d);
void BalanceHandler(long cookie, int socket_d);
void DepositHandler(long cookie, int socket_d);
void WithdrawHandler(long cookie, int socket_d);
void viewAccountDetailsHandler(long cookie, int socket_d);
void ModifyAdminPasswordHandler(long cookie, int socket_d, char *username);
void ModifyAccountTypeHandler(long cookie, int socket_d);
void DeleteAccountHandler(long cookie, int socket_d);
void AddAccountHandler(long cookie, int socket_d);
int loginToggle(int socket_d);
int normallogin(int socket_d);
int adminlogin(int socket_d);
int optionToggle(long cookie, int socket_d);
int adminoptionToggle(long cookie, int socket_d, char *username);
int jointlogin(int socket_d);
#endif