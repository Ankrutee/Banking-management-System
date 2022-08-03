#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "all_databases.h"

int main()
{
    int fd = open("account_database.dat", O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        printf("Error in opening account database file\n");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    struct Account account1;
    account1.ID = 1;
    account1.Balance = 50000;
    account1.Authenticate_JointAcc = 0;
    write(fd, &account1, sizeof(struct Account));
    

    struct Account account2;
    account2.ID = 2;
    account2.Balance = 40000;
    account2.Authenticate_JointAcc = 0;
    write(fd, &account2, sizeof(struct Account));
    
    struct Account account3;
    account3.ID = 3;
    account3.Balance = 30000;
    account3.Authenticate_JointAcc = 1;
    write(fd, &account3, sizeof(struct Account));
    
    struct Account account4;
    account4.ID = 4;
    account4.Balance = 20000;
    account4.Authenticate_JointAcc = 1;
    write(fd, &account4, sizeof(struct Account));

    int total_number_of_accounts = 4;
    lseek(fd, 0, SEEK_END);
    write(fd,&total_number_of_accounts,sizeof(total_number_of_accounts));

    lseek(fd, 0, SEEK_SET);
    struct Account trial;
    for(int i = 0; i < 11; i++)
    {
        read(fd, &trial,sizeof(struct Account));
        printf("%d\n",trial.ID);
        printf("%d\n",trial.Balance);
        printf("%d\n",trial.Authenticate_JointAcc);
        printf("\n\n");
    }

    int max;
    lseek(fd, -1*sizeof(int), SEEK_END);
    read(fd, &max,sizeof(max));
    printf("max is %d\n",max);

    close(fd);
    return 0;
}