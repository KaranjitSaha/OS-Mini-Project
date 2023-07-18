#include "./include/datastructures.h"
#include "./include/dbops.h"
#include "./include/syscalls.h"
#include "./include/server.h"
#define PORT 4444
extern int errno;

int mainController(int client,int userID,long cookie){
    enum RequestType requestType;
    long cookie_recv;

    read(client,&cookie_recv,sizeof(cookie_recv));
    read(client,&requestType,sizeof(requestType));
    if(cookie_recv ==cookie){
        printf("cookie matched\n");
        printf("type recv = %d\n",requestType);
        switch(requestType){
            case DepositRequest:
                deposit(client,userID);
                break;
            case WithdrawalRequest:
                withdraw(client,userID);
                break;
            case BalanceRequest:
                balance(client,userID);
                break;
            case PasswordChangeRequest:
                password_change(client,userID);
                break;
            case ViewDetailsRequest:
                view_details(client,userID);
                break;
            case ExitRequest:
                client_exit(client,userID);
                break;
        }
    }
    mainController(client,userID,cookie);
}

int adminController(int client,long cookie){
    enum AdminRequestType type;
    long cookie_recv;

    read(client,&cookie_recv,sizeof(cookie_recv));
    read(client,&type,sizeof(type));

    if(cookie_recv==cookie){
        printf("cookie matched \n");
        printf("type recv = %d\n",type);
        switch(type){
            case AddAccountRequest:
                add_account(client);
                break;
            case DeleteAccountRequest:
                // printf("line 56\n");
                delete_account(client);
                break;
            case ModifyAccountTypeRequest:
                modify_account_type(client);
                break;
            case ModifyPasswordRequest:
                modify_admin_password(client);
                break;
            case AccountDetailsRequest:
                viewAccountDetails(client);
                break;
            case AdminExitRequest:
                client_exit(client,-1);
                break;
            default:
                printf("Nothing Was Caught\n");
                break;
        }
    }
    adminController(client,cookie);
}

int viewAccountDetails(int client){
    int user_id;
    read(client,&user_id,sizeof(user_id));
    view_details(client,user_id);
    return 1;
}

int modify_admin_password(int client){
    char newPassword[100];
    char username[100];
    read(client,newPassword,sizeof(newPassword));
    read(client,username,sizeof(username));
    printf("New Password = %s\n",newPassword);
    printf("Username = %s\n",username);
    int ret =db_modify_admin_password(newPassword,username);
    write(client,&ret,sizeof(ret));
    if(ret==1){
        printf("Successfull\n");
    }
    else
    {
        printf("Unsuccessful\n");
    }
    return ret;
}

int modify_account_type(int client){
    int account_no;
    int account_type;
    read(client,&account_no,sizeof(account_no));
    read(client,&account_type,sizeof(account_type));
    printf("Account No read = %d\n", account_no);
    printf("Account Type read = %d\n", account_type);
    int ret=db_modify_account_type(account_no,account_type);
    write(client,&ret,sizeof(ret));
    if(ret==1){
        printf("Successfull\n");
    }
    else
        printf("Unsuccessfull\n");
    return ret;
}

int delete_account(int client){
    //  printf("line122\n");

    int account_no;
    //  printf("line123\n");

    read(client, &account_no, sizeof(account_no));
    //  printf("line124\n");

    int ret = db_delete_account(account_no);
    //  printf("line125\n");

    write(client, &ret, sizeof(ret));
    //  printf("line128\n");

    if (ret == 1){
        printf("Successfull\n");
    }
    else{
        printf("Unsuccessful\n");
    }
    return ret;
}

int add_account(int client){
    // printf("in the add_account function\n");
    int account_no;

    read(client, &account_no, sizeof(account_no));
    printf("account no read = %d\n", account_no);
    int ret = db_add_account(account_no);
    write(client, &ret, sizeof(ret));
    // printf("%d\n", ret);
    return ret;
}

int client_exit(int client,int userID){
    close(client);
    if (client == -1)
        printf("connection ended with admin\n");
    else
        printf("connection with client%d closed\n", client);
    exit(0);
}

int password_change(int client, int userID){
    char newpassword[100];
    int ret;
    read(client, newpassword, sizeof(newpassword));
    ret = db_password_change(newpassword, userID);
    write(client, &ret, sizeof(ret));
    return ret;
}

int view_details(int client, int userID){
    // printf("in the view_details function\n");
    int *details = malloc(sizeof(int)*4);
    details = db_account_details(userID); 
    // 4 integers should be written to client
    write(client, details, sizeof(int)*4);
    return 1;
} 

int balance(int client, int userID){
    int balance = db_balance(userID);
    write(client, &balance, sizeof(balance));
    if (balance > 0)
        return 1;
    else
        return -1;
}

int deposit(int client, int userID){
    int deposit ;
    read(client, &deposit, sizeof(deposit));
    int ret = db_deposit(deposit, userID);
    write(client, &ret, sizeof(ret));
    return ret;
}

int withdraw(int client, int userID){
    int withdraw;
    // printf("inside withdrawfunction\n");
    read(client, &withdraw, sizeof(withdraw));
    int ret = db_withdraw(withdraw, userID);
    write(client, &ret, sizeof(ret));
    return ret;
}

long GenerateCookie(int val){
    if (val == 1){
        return random();
    }
    else{
        return -1;
    }
}

int normallogin(int client){
    Credentials *logincred = malloc(sizeof(Credentials));
    printf("waiting for login credentials\n");
    int bytes = read(client, logincred, sizeof(Credentials));
    int val = validateLogin(logincred);
    long cookie = GenerateCookie(val);

    write(client, &cookie, sizeof(cookie));  
    if (val == 1){
        // printf("line 217 of serer\n");
        int userID = logincred->id;
        mainController(client, userID,  cookie);
    } 
    else{
        // printf("%ldline 221 of server\n",cookie);
        loginHandler(client);
    }
    return 0;
} 

int adminlogin(int client){
    Admin *logincred = malloc(sizeof(Admin));
    printf("waiting for admin login credentials\n");
    int bytes = read(client, logincred, sizeof(Admin));
    // printf("bytes read %d\n", bytes);
    int val = ValidateAdminLogin(logincred);
    long cookie = GenerateCookie(val);


    write(client, &cookie, sizeof(cookie));  
    if (val == 1){
        adminController(client, cookie);
    } 
    else{
        loginHandler(client);
    }
    return 0;

}

int loginHandler(int client){
    enum LoginRequest type;
    // printf("in Login Handler\n");
    int ret; 
    ret = read(client, &type, sizeof(type));
    // printf("bytes read = %d\n", ret);
    if (ret < 0){
        printf("first query is not doing anything\n");
        return -1;
    }
    switch(type){
        case NormalLoginRequest:
            normallogin(client);
            break;
        case AdminLoginRequest:
            adminlogin(client);
            break;
    } 
    return 0;
}

int connectSocket(){
    int socket_d;
    struct sockaddr_in serveraddr, clientaddr;
    int opt = 1;

    socket_d = socket(AF_INET, SOCK_STREAM, 0);// creating socket in namespace
    if (socket_d == 0){
        int err = errno;	
        printf("server end socket creation failed: %s\n", strerror(err));
        return -1;
    }
    //forcing port reuse
    setsockopt(socket_d, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);

    if(bind(socket_d, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        int err = errno;	
        printf("server end binding failed: %s\n", strerror(err));
        return -1;
    }
    return socket_d;
}

void *connection_handler(void *nsd) {
	int nsfd = *(int*)nsd;
	loginHandler(nsfd);
}

int connectClient(int socket_d)  {
    // handling 10 users at once at max
    struct sockaddr_in clientaddr;
	socklen_t addr_size = sizeof(clientaddr);
    listen(socket_d, 10);
    int childpid;
    pthread_t threads;

    while(1){
        int client = accept(socket_d, (struct sockaddr *)&clientaddr,&addr_size);
        if (client < 0){
            int err = errno;	
            printf("connection failed: %s\n", strerror(err));
            return -1;
        }
        printf("connection accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        
        if(pthread_create(&threads,NULL,connection_handler,(void*)&client)<0){
            printf("couldn't create thread\n");
            return 1;
        }
        
        // if ((childpid=fork()) == 0){
        //     close(socket_d);
        //     loginHandler(client);    
        //     return 1;
        // }
        // else{
        //     close(client);
        // } 
    }
    pthread_exit(NULL);
return 0;
}

int main(){
    srandom(42);
    int socket_d=connectSocket();
    connectClient(socket_d);
    return 0;
}









