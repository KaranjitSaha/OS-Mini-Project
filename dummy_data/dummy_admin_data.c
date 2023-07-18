#include "../include/syscalls.h"
#include "../include/datastructures.h"
int main()
{
    int adminfd=open("../data/Admin.dat",O_CREAT|O_WRONLY,0744);
    if(adminfd==-1){
        printf("Error opening admin data file\n");
        exit(0);
    }
    lseek(adminfd,0,SEEK_SET);
    
    Admin admin1={
        .username="Karanjit",
        .password="Null",
    };

    write(adminfd,&admin1,sizeof(admin1));

    Admin admin2={
        .username="admin2",
        .password="iiitb",
    };

    write(adminfd,&admin2,sizeof(admin1));

    close(adminfd);

}