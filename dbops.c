#include"./include/dbops.h"
extern int errno;

/*GET DETAILS OF A USER*/
User getUser(int user_id){
    User validate;
    int userIndex =user_id-1;
    int fd=open("./data/Users.dat",O_RDONLY);
    if(fd==-1){
        printf("Error opening Users file\n");
        exit(1);
    }

    static struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_start=userIndex*sizeof(User);
    lock.l_whence=SEEK_SET;
    lock.l_len=sizeof(User);
    lock.l_pid=getpid();

    int ret=fcntl(fd, F_SETLK, &lock);
    if (ret == -1) {
        int err=errno;
        printf("Attempt to set write lock failed %s\n",strerror(err));
        return validate;
    }
    lseek(fd,userIndex*sizeof(User),SEEK_SET);
    read(fd,&validate,sizeof(User));
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
    return validate;
}

/*GET ACCOUNT ID*/
int getAccountId(int user_id){
    User user=getUser(user_id);
    return user.account_id;
}

/*DEPOSIT AMOUNT TO AN ACCOUNT*/
int db_deposit(int amount,int user_id){
    int account_id=getAccountId(user_id);
    Account account;
    int accountIndex=account_id-1;
    int fd=open("./data/Account.dat",O_RDWR);
    if(fd==-1)
    {
        printf("Unable to open Account file\n");
        return -1;
    }
    static struct flock lock;
    lock.l_len=sizeof(struct Account);
    lock.l_pid=getpid();
    lock.l_whence=SEEK_SET;
    lock.l_start=accountIndex*sizeof(Account);
    lock.l_type=F_WRLCK;

    int ret=fcntl(fd, F_SETLKW,&lock);
    if(ret==-1){
        printf("Attempt to set write lock failed\n");
        return -1;
    }
    //acquiring initial balance
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    read(fd,&account,sizeof(Account));
    printf("Initial Balance= %d\n",account.balance);

    //updating the amount in the account
    account.balance+=amount;
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    write(fd,&account,sizeof(Account));

    //checking if the amount got updated
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    read(fd,&account,sizeof(Account));
    printf("Updated Balance= %d\n",account.balance);

    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 1;//1 represents success
}

/*WITHDRAW AMOUNT FROM AN ACCOUNT*/
int db_withdraw(int amount,int user_id)
{
    int account_id=getAccountId(user_id);
    Account account;
    int accountIndex = account_id-1;
    int fd=open("./data/Account.dat",O_RDWR);
    if(fd == -1) {
      printf("Unable to open the file\n");
      exit(1);
    }
    static struct flock lock;
    lock.l_type=F_WRLCK;
    lock.l_start=accountIndex*sizeof(Account);
    lock.l_pid=getpid();
    lock.l_whence=SEEK_SET;
    lock.l_len=sizeof(Account);

    int ret=fcntl(fd, F_SETLK, &lock);
    if(ret == -1){
        printf("Atempted to set write lock failed\n");
        return -1;
    }

    //acquiring intial balance
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    read(fd,&account,sizeof(Account));
    printf("Initial Balance= %d\n",account.balance);
    //checking for insufficient funds
    if(account.balance<amount)
    {
        printf("Insufficient funds for withdrawal\n");
        return -1;
    }
    account.balance-=amount;

    //updating database
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    write(fd,&account,sizeof(Account));
    
    //checking if the amount got updated
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    read(fd,&account,sizeof(Account));
    printf("Updated Balance= %d\n",account.balance);
    
    lock.l_type=F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 1;
}

/*TO GET THE BALANCE OF AN ACCOUNT*/
int db_balance(int user_id){
    int account_id=getAccountId(user_id);
    Account account;
    int accountIndex=account_id-1;
    int fd=open("./data/Account.dat",O_RDWR,0744);
    if (fd==-1){
        printf("Unable to open the file\n");
        return -1;
    }
    static struct flock lock;
    lock.l_pid=getpid();
    lock.l_start=accountIndex*sizeof(Account);
    lock.l_len=sizeof(Account);
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;

    int ret =fcntl(fd, F_SETLKW, &lock);
    if(ret==-1){
        printf("Attempt to set read lock failed\n");
        return -1;
    }

    //acquiring the balance
    lseek(fd,accountIndex*sizeof(Account),SEEK_SET);
    read(fd,&account,sizeof(Account));
    printf("balance = %d\n",account.balance);

    lock.l_type=F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return account.balance;
}


/*GET ACCOUNT DETAILS*/
int *db_account_details(int user_id){
    static int account_details[4]={0,0,0,0};
    User user=getUser(user_id);
    int account_balance=db_balance(user_id);
    printf("UserID = %d\n",user_id);
    printf("Account Number = %d\n",user.account_id);
    printf("Account Type = %d\n",user.account_type);
    printf("Account Balance = %d\n",account_balance);
    account_details[0]=user_id;
    account_details[1]=user.account_id;
    account_details[2]=user.account_type;
    account_details[3]=account_balance;
    return account_details;
}

/*CHANGE PASSWORD OF AN USER*/
int db_password_change(char* newpassword,int user_id){
    User user;
    int userIndex=user_id-1;
    int fd=open("./data/Users.dat",O_RDWR);
    if(fd == -1) {
      printf("Unable to open the file\n");
      return -1;
    }
    static struct flock lock;
    lock.l_len=sizeof(User);
    lock.l_pid=getpid();
    lock.l_start=userIndex*sizeof(User);
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;

    int ret=fcntl(fd,F_SETLKW, &lock);
    if(ret == -1){
        printf("Attempt to set write lock failed\n");
        return -1;
    }

    //acquiring old password
    lseek(fd,userIndex*sizeof(User),SEEK_SET);
    read(fd,&user,sizeof(User));
    printf("Old Password: %s\n",user.password);

    //updating new password
    strcpy(user.password,newpassword);
    lseek(fd,userIndex*sizeof(User),SEEK_SET);
    write(fd,&user,sizeof(User));

    //checking if the password is updated
    lseek(fd,userIndex*sizeof(User),SEEK_SET);
    read(fd,&user,sizeof(User));
    printf("New Password: %s\n",user.password);
    
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);
    return 1;
}

/*ADD AN ACCOUNT(FOR THIS WE HAVE CREATED A DUMMY USER WHO IS ASSIGNED THE ACCOUNT */
int db_add_account(int account_no){
    /*CREATING AN ACCOUNT */
    Account account={
        .balance=0,
        .id=account_no,
    };
    int fd=open("./data/Account.dat",O_RDWR|O_APPEND);
    if(fd == -1) {
      printf("Unable to open the file\n");
      return -1; 
    }
    //locking the whole file
    static struct flock lock;
    lock.l_len=0;
    lock.l_pid=getpid();
    lock.l_start=0;
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;
    int ret=fcntl(fd,F_SETLK, &lock);
    if(ret == -1){
        printf("Attempt to set write lock failed\n");
        return -1;
    }
    write(fd,&account,sizeof(Account));
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLK, &lock);
    close(fd);
    printf("Account Created Successfully\n");

    /*THIS NEW ACCOUNT NEEDS TO BE GIVEN TO AN USER, HENCE A DUMMY USER IS BEING CREATED HERE*/
    User user;
    fd=open("./data/Users.dat",O_RDWR);
    if(fd == -1) {
      printf("Unable to open the file\n");
      return -1;
    }
    static struct flock lock1;
    lock1.l_len=0;
    lock1.l_pid=getpid();
    lock1.l_start=0;
    lock1.l_type=F_WRLCK;
    lock1.l_whence=SEEK_SET;

    ret=fcntl(fd,F_SETLKW,&lock1);
    if(ret==-1){
        int err=errno;
        printf("Attempt to set write lock1 failed %s\n",strerror(err));
        return -1;
    }

    //getting max no of users
    int max_user_id=-100;
    while(read(fd, &user, sizeof(User))){
        if (user.id > max_user_id)
            max_user_id = user.id;
    }

    printf("max user id = %d\n",max_user_id);
    user.id=max_user_id+1;
    user.account_id=account_no;
    user.account_type=singleAccount;
    char password[100]="dummypass";
    strcpy(user.password,password);

    ret=write(fd,&user,sizeof(User));
    if(ret<0){
        printf("Unable to write new User\n");
        return -1;
    }
    printf("User written but still in critical section\n");
    lock1.l_type=F_UNLCK;
    fcntl(fd,F_SETLKW, &lock1);
    close(fd);
    printf("Successfully created user and assigned the newly created account\n ");
    return 1;
}

/*FUNCTION TO CHANGE PASSWORD OF THE ADMIN*/
int db_modify_admin_password(char* newpassword,char* username){
    Admin admin;
    int fd=open("./data/Admin.dat",O_RDWR);
    if(fd==-1){
        printf("Unable to open the file\n");
        exit(1);
    }
    static struct flock lock;
    lock.l_len=0;
    lock.l_pid=getpid();
    lock.l_start=0;
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;

    int ret=fcntl(fd, F_SETLKW, &lock);
    if(ret == -1){
        int err=errno;
        printf("Attempt to set write lock failed %s\n",strerror(err));
        return -1;
    }
    //checking for the required admin 
    lseek(fd,0,SEEK_SET);
    while(read(fd,&admin,sizeof(Admin))){
        if(strcmp(admin.username,username)==0){
            strcpy(admin.password,newpassword);
            lseek(fd,-sizeof(admin),SEEK_CUR);
            write(fd,&admin,sizeof(Admin));
            lock.l_type=F_UNLCK;
            fcntl(fd, F_SETLK,&lock);
            close(fd);
            return 1;
        }
    }
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLK, &lock);
    close(fd);
    return -1;
}

/*CHANGE THE ACCOUNT TYPE OF AN ACCOUNT */
int db_modify_account_type(int account_no,int account_type){
    User user;
    account_type=account_type-1;
    int fd=open("./data/Users.dat",O_RDWR);
    int result;
    if(fd==-1){
        printf("Unable to open the file\n");
        exit(1);
    }
    static struct flock lock;
    lock.l_len=0;
    lock.l_pid=getpid();
    lock.l_start=0;
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;

    int ret=fcntl(fd, F_SETLKW, &lock);
    if(ret == -1){
        int err=errno;
        printf("Attempt to set write lock failed %s\n",strerror(err));
        return -1;
    }
    
    //searching for the required user
    lseek(fd,0,SEEK_SET);
    while(read(fd,&user,sizeof(user))){
        if(user.account_id==account_no){
            user.account_type=account_type;
            lseek(fd,-sizeof(user),SEEK_CUR);
            write(fd,&user,sizeof(User));
            result=1;
            break;
        }
        // else
        // {   printf("didnt match %d\n",user.account_id);
        // }
    }
    if(result!=1){
        printf("line384\n");
        result=-1;
    }
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLK, &lock);
    close(fd);
    return result;
}

/*TO DELETE AN ACCOUNT(HERE RATHER THAN DELETING I AM CHANGING THE BALANCE AND ID TO -1*/
int db_delete_account(int account_no){
    Account account;
    Account delete={
        .balance=-1,
        .id=-1,
    };
    int fd=open("./data/Account.dat",O_RDWR);
    if(fd == -1) {
      printf("Unable to open the file\n");
      exit(1);
    }
    
    static struct flock lock;
    lock.l_len=0;
    lock.l_pid=getpid();
    lock.l_start=0;
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;

    int ret=fcntl(fd, F_SETLK, &lock);
    if(ret==-1){
        int err=errno;
        printf("Attempt to set write lock failed %s\n",strerror(err));
        return -1;
    }
    //searching for the required account
    int result=-1;
    lseek(fd,0,SEEK_SET);
    while(read(fd,&account,sizeof(Account))){
        if(account.id==account_no){
            lseek(fd,-sizeof(Account),SEEK_CUR);
            write(fd,&delete,sizeof(Account));
            result=0;
        }
    }
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLK, &lock);
    close(fd);

    User user;
    fd=open("./data/Users.dat",O_RDWR);
    if(fd==-1){
        printf("Unable to open the file\n");
        exit(1);
    }
    
    static struct flock lock1;
    lock1.l_len=0;
    lock1.l_pid=getpid();
    lock1.l_start=0;
    lock1.l_type=F_WRLCK;
    lock1.l_whence=SEEK_SET;

    ret=fcntl(fd,F_SETLK, &lock1);
    if(ret==-1){
        int err=errno;
        printf("Attempt to set write lock failed %s\n",strerror(err));
        return -1;
    }
    //searching for the required user
    lseek(fd,0,SEEK_SET);
    while(read(fd,&user,sizeof(User))){
        if(user.account_id==account_no){
            user.account_id=-1;
            lseek(fd,-sizeof(User),SEEK_CUR);
            write(fd,&user,sizeof(User));
            result=1;
        }
    }
    lock1.l_type=F_UNLCK;
    fcntl(fd,F_SETLK, &lock1);
    close(fd);
    return result;
}

/*VALIDATE CREDENTIALS OF AN USER*/
int validateLogin(Credentials *logincred){
    int user_id=logincred->id;
    User user=getUser(user_id);
    int result;
    if(user.id==logincred->id){
        if(strcmp(user.password,logincred->password)==0){
            result=1;
        }
        else{
            result=-1;
        }
    }
    else{
        result=-1;
    }
    return result;
}

int validatejointLogin(Credentials *logincred){
    int user_id=logincred->id;
    User user=getUser(user_id);
    int result;
    if(user.id==logincred->id){
        if(strcmp(user.password,logincred->password)==0 && user.account_type==jointAccount){
            result=1;
        }
        else{
            result=-1;
        }
    }
    else{
        result=-1;
    }
    return result;
}

/*VALIDATE CREDENTIALS OF ADMIN*/
int ValidateAdminLogin(Admin* logincred){
    Admin validate;
    int index=0;
    int fd=open("./data/Admin.dat",O_RDONLY);
    if(fd==-1){
        printf("Unable to open the file\n");
        exit(1);
    }
    
    static struct flock lock;
    lock.l_len=0;
    lock.l_pid=getpid();
    lock.l_start=0;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    int ret=fcntl(fd,F_SETLK, &lock);
    if(ret==-1){
        int err=errno;
        printf("Attempt to set write lock failed %s\n",strerror(err));
        return -1;
    }
    int result=-1;
    lseek(fd,0,SEEK_SET);
    printf("Entered Username = %s\n",logincred->username);
    printf("Entered Password = %s\n",logincred->password);

    //validating admin by comparing usernames and their corresponding password
    while(read(fd,&validate,sizeof(validate))){
        if(strcmp(validate.username,logincred->username)==0){
            if(strcmp(validate.password,logincred->password)==0){
                return 1;
            }
        }
    }
    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLK, &lock);
    close(fd);
    return result;
}


















