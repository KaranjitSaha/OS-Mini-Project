#include "../include/syscalls.h"
#include "../include/datastructures.h"
int main()
{
    int accountfd=open("../data/Account.dat",O_CREAT|O_WRONLY,0744);
    if(accountfd==-1){
        printf("Error opening account data file\n");
        exit(0);
    }
    lseek(accountfd,0,SEEK_SET);
    
    Account account;
    account.id=1;
    account.balance=5000;
    write(accountfd,&account,sizeof(Account));
    
    account.id=2;
    account.balance=3000;
    write(accountfd,&account,sizeof(Account));

    account.id=3;
    account.balance=3750;
    write(accountfd,&account,sizeof(Account));

    account.id=4;
    account.balance=9000;
    write(accountfd,&account,sizeof(Account));
    close(accountfd);
}