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
    int fd = open("admin_database.dat", O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        printf("Error in opening admin database file\n");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    struct Admin admin1;
    admin1.Admin_ID = 1;
    strcpy(admin1.Admin_Pwd ,"admin1password");
    write(fd, &admin1, sizeof(struct Admin));
    

    struct Admin admin2;
    admin2.Admin_ID = 2;
    strcpy(admin2.Admin_Pwd ,"admin2password");
    write(fd, &admin2, sizeof(struct Admin));
    
    struct Admin admin3;
    admin3.Admin_ID = 3;
    strcpy(admin3.Admin_Pwd ,"admin3password");
    write(fd, &admin3, sizeof(struct Admin));

    
    lseek(fd, 0, SEEK_SET);

    struct Admin trial;
    for(int i = 0; i < 3; i++)
    {
        read(fd, &trial,sizeof(struct Admin));
        printf("%d\n",trial.Admin_ID);
        printf("%s\n",trial.Admin_Pwd);
        printf("\n\n");
    }

    close(fd);
    return 0;
}