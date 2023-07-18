#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H
enum AdminRequestType{
    AdminDummyRequest,
    AddAccountRequest,
    DeleteAccountRequest,
    ModifyAccountTypeRequest,
    ModifyPasswordRequest,
    AccountDetailsRequest,
    AdminExitRequest,
};

enum RequestType{
    DummyRequest,
    DepositRequest,
    WithdrawalRequest,
    BalanceRequest,
    PasswordChangeRequest,
    ViewDetailsRequest,
    ExitRequest,
};

enum AccountType{
    singleAccount,
    jointAccount
};

enum TransactionType{
    Deposit,
    Withdrawal
};

enum LoginRequest{
    NormalLoginRequest,
    AdminLoginRequest,
};

typedef struct Account{
    int id;
    int balance;
}Account;

typedef struct User{
    
    int id;
    char password[100];
    int account_id;
    enum AccountType account_type;
}User;

typedef struct Admin{
    char username[100];
    char password[100];
}Admin;

typedef struct Credentials{
    int id;
    char password[100];
}Credentials;

#endif 