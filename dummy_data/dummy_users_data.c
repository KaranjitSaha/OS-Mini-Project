#include "../include/syscalls.h"
#include "../include/datastructures.h"
int main()
{
    int usersfd=open("../data/Users.dat",O_CREAT|O_WRONLY,0744);
    if(usersfd==-1){
        printf("Error opening user data file\n");
        exit(0);
    }
    lseek(usersfd,0,SEEK_SET);
    
    User user1={
        .id=1,
        .account_id=1,
        .password="abc",
        .account_type=singleAccount,
    };
    write(usersfd,&user1,sizeof(user1));

    User user2={
        .id=2,
        .account_id=2,
        .password="abcd",
        .account_type=singleAccount,
    };
    write(usersfd,&user2,sizeof(user1));

    User user3={
        .id=3,
        .account_id=3,
        .password="abcde",
        .account_type=singleAccount,
    };
    write(usersfd,&user3,sizeof(user1));

    User user4={
        .id=4,
        .account_id=4,
        .password="abcdef",
        .account_type=singleAccount,
    };
    write(usersfd,&user4,sizeof(user1));

    close(usersfd);

}