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
    int fd = open("user_database.dat", O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        printf("Error in opening user database file\n");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    struct User user1;
    user1.ID = 1;
    user1.Account_ID = 1;
    strcpy(user1.pwd,"pwd1");
    write(fd, &user1, sizeof(struct User));
    

    struct User user2;
    user2.ID = 2;
    user2.Account_ID = 2;
    strcpy(user2.pwd,"pwd2");
    write(fd, &user2, sizeof(struct User));
    
    struct User user3;
    user3.ID = 3;
    user3.Account_ID = 3;
    strcpy(user3.pwd,"pwd3");
    write(fd, &user3, sizeof(struct User));
    
    struct User user4;
    user4.ID = 4;
    user4.Account_ID = 3;
    strcpy(user4.pwd,"pwd4");
    write(fd, &user4, sizeof(struct User));

    
    
    lseek(fd, 0, SEEK_SET);

    struct User trial;
    for(int i = 0; i < 4; i++)
    {
        read(fd, &trial,sizeof(struct User));
        printf("%d\n",trial.ID);
        printf("%s\n",trial.pwd);
        printf("%d\n",trial.Account_ID);
        printf("\n\n");
    }

    close(fd);
    return 0;
}