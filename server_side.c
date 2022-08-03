#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include "all_databases.h"
#define PORT_NUMBER 5500

int If_Not_Admin_User(int, int);
void* Validate_Client(void *vargp);
struct User* Val_Client_Login(int, char*);
struct Account* getAccount(int);
struct Account* Deposit_Account(int,int);
struct Account* ammount_withdraw(int, int);
void Change_Pwd(int, char*);
int Admin_User_Login(int);
void Admin_Add_Acc(int);
void Admin_Dlt_Acc(int);
void Admin_find_Acc(int);

int main()
{
    int sd;
    int number = 1;
    struct sockaddr_in server, client;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == 0)
    {
        printf("Oops, Error in making socket\n");
        exit(0);
    }
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &number, sizeof(number));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUMBER);
    bind(sd, (void*)&server, sizeof(server));
	int client_size = sizeof(client);
    listen(sd, 5);
    while(1)
    {
        int new_sd = accept(sd, (void*)&client,&client_size);
        if (new_sd > 0)
        {
            pthread_t thread_id;
            pthread_create(&thread_id,NULL,Validate_Client,(void*)&new_sd);
        }
        else
            close(new_sd);
    }

    
    return 0;
}

void* Validate_Client(void *vargp)
{
    int loginChoice;
    int new_sd = *(int*)vargp;
    read(new_sd, &loginChoice, sizeof(loginChoice));
    int flag = 0;
    switch(loginChoice)
    {
        case 1:
            If_Not_Admin_User(new_sd, loginChoice);
            break;
        case 2:
            If_Not_Admin_User(new_sd, loginChoice);
            break;
        case 3:
            Admin_User_Login(new_sd);
            break;

        default:
            close(new_sd);
            flag = 1;
    } 

    if (!flag)
        Validate_Client(vargp);
    return 0;
}

int If_Not_Admin_User(int new_sd, int loginChoice)
{
    int userID, flag1 = 0;
    char password[100];
    struct Account* account_check = (struct Account*)malloc(sizeof(struct Account));
    read(new_sd, &userID, sizeof(userID));
    read(new_sd,&password,sizeof(password));
    struct User* user = Val_Client_Login(userID, password);
    account_check = getAccount(user->Account_ID);
    bool Authenticate_It = false;
    if(user->ID == userID && !strcmp(password, user->pwd) && account_check->Authenticate_JointAcc == loginChoice - 1)
    {
        Authenticate_It = true;
        write(new_sd,&Authenticate_It, sizeof(Authenticate_It));

        while(flag1 ==0)
        {
            int flag = 0;
            int option_chosen_by_client;
            read(new_sd, &option_chosen_by_client, sizeof(option_chosen_by_client));
            int Account_ID = user->Account_ID;
            int amount;
            char new_password[100];
            struct Account* account = (struct Account*)malloc(sizeof(struct Account));
            struct Account* account1 = (struct Account*)malloc(sizeof(struct Account));
            switch(option_chosen_by_client)
            {
                case 1:
                read(new_sd,&amount, sizeof(amount));
                account = Deposit_Account(Account_ID,amount);
                write(new_sd,account,sizeof(struct Account));
                break;
                
                case 2:
                read(new_sd,&amount, sizeof(amount));
                account1 = getAccount(Account_ID);
                account = ammount_withdraw(Account_ID,amount);
                bool balancehasChanged = true;
                if (account1->Balance == account->Balance)
                {
                    balancehasChanged = false;
                }
                write(new_sd,&balancehasChanged,sizeof(balancehasChanged));
                write(new_sd,account,sizeof(struct Account));
                break;

                case 3:
                account = getAccount(Account_ID);
                write(new_sd,account,sizeof(struct Account));
                break;

                case 4:
                read(new_sd,&new_password, sizeof(new_password));
                Change_Pwd(user->ID,new_password);
                break;

                case 5:
                account = getAccount(Account_ID);
                write(new_sd,account,sizeof(struct Account));
                break;

                case 6:
                    close(new_sd);
                    flag1 =1;
                    break;
                break;

                default:
                    break;
            }
        }
    }

    write(new_sd,&Authenticate_It, sizeof(Authenticate_It));
}

struct User* Val_Client_Login(int userID, char password[]) 
{
    struct User* user = (struct User*)malloc(sizeof(struct User));
    int seekAmount = userID -1; 
    int fd = open("user_database.dat", O_RDONLY);
    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = seekAmount*sizeof(struct User);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct User);
    lock.l_pid = getpid();
    

    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct User),SEEK_SET);
    read(fd, user, sizeof(struct User));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return user;
} 

struct Account* getAccount(int Account_ID)
{
    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    int seekAmount = Account_ID - 1; 
    int fd = open("account_database.dat", O_RDONLY,0744);
    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = seekAmount*sizeof(struct Account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Account);
    lock.l_pid = getpid();
    

    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    read(fd, account, sizeof(struct Account));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return account;
}

struct Account* Deposit_Account(int Account_ID, int amount)
{
    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    int seekAmount = Account_ID - 1; 
    int fd = open("account_database.dat", O_RDWR);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(struct Account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Account);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);

    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    read(fd, account, sizeof(struct Account));
    
    account->Balance = account->Balance + amount;
    lseek(fd, seekAmount*sizeof(struct Account),SEEK_SET);
    write(fd, account, sizeof(struct Account));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return account;
}

struct Account* ammount_withdraw(int Account_ID, int amount)
{
    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    int seekAmount = Account_ID - 1; 
    int fd = open("account_database.dat", O_RDWR);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(struct Account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Account);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);

    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    read(fd, account, sizeof(struct Account));
    
    if (account->Balance >= amount)
    {
        account->Balance = account->Balance - amount;
        lseek(fd, seekAmount*sizeof(struct Account),SEEK_SET);
        write(fd, account, sizeof(struct Account));
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return account;
}

void Change_Pwd(int userID, char password[])
{
    struct User* user = (struct User*)malloc(sizeof(struct User));
    int seekAmount = userID -1; 
    int fd = open("user_database.dat", O_RDWR);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(struct User);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct User);
    lock.l_pid = getpid();
    

    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct User),SEEK_SET);
    read(fd, user, sizeof(struct User));

    
    strcpy(user->pwd, password);
    lseek(fd,seekAmount*sizeof(struct User),SEEK_SET);
    write(fd, user, sizeof(struct User));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

struct Admin* validateAdminLogin(int adminID, char admin_password[])
{
    struct Admin* admin = (struct Admin*)malloc(sizeof(struct Admin));
    int seekAmount = adminID -1; 
    int fd = open("admin_database.dat", O_RDONLY);
    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = seekAmount*sizeof(struct Admin);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Admin);
    lock.l_pid = getpid();
    

    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct Admin),SEEK_SET);
    read(fd, admin, sizeof(struct Admin));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    if (admin->Admin_ID == adminID)
    {
        if (strcmp(admin->Admin_Pwd, admin_password) == 0)
        {
            return admin;
        }
        return admin;
    }
    return admin;
}

int Admin_User_Login(int new_sd)
{
    int flag = 0;
    int adminID;
    char password[100];
    read(new_sd, &adminID, sizeof(adminID));
    read(new_sd,&password,sizeof(password));
    struct Admin* admin = (struct Admin*)malloc(sizeof(struct Admin));

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));

    admin = validateAdminLogin(adminID, password);
    bool Authenticate_It = false;
    if(admin->Admin_ID == adminID && !strcmp(password, admin->Admin_Pwd))
    {
        Authenticate_It = true;
        write(new_sd,&Authenticate_It, sizeof(Authenticate_It));

        while(flag == 0)
        {
            int option_chosen_by_client;
            int Account_ID;
            read(new_sd, &option_chosen_by_client, sizeof(option_chosen_by_client));
            switch(option_chosen_by_client)
            {
                case 1:
                Admin_Add_Acc(new_sd);
                break;
                
                case 2:
                Admin_Dlt_Acc(new_sd);
                break;

                case 3:
                Admin_find_Acc(new_sd);
                break;

                case 4:
                read(new_sd,&Account_ID, sizeof(Account_ID));
                account = getAccount(Account_ID);
                write(new_sd,account, sizeof(struct Account));

                break;

                case 5:
                close(new_sd);
                flag = 1;
                break;

                default:
                break;
            }
        }
    }
    write(new_sd,&Authenticate_It, sizeof(Authenticate_It));
}

int get_total_numberber_of_accounts()
{
    int fd = open("account_database.dat", O_RDWR,0744);
    
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = -1*sizeof(int);
    lock.l_whence = SEEK_END;
    lock.l_len = sizeof(int);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);
    lseek(fd,-1*sizeof(int),SEEK_END);
    
    int max_Account_ID;
    read(fd,&max_Account_ID,sizeof(max_Account_ID));
    max_Account_ID += 1;

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return max_Account_ID;
}


void Admin_Add_Acc(int new_sd)
{
    int account_balance;
    read(new_sd, &account_balance, sizeof(account_balance));

    int Authenticate_JointAcc;
    read(new_sd, &Authenticate_JointAcc, sizeof(Authenticate_JointAcc));

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    
    int max_Account_ID = get_total_numberber_of_accounts();
    account->Balance = account_balance;
    account->Authenticate_JointAcc = Authenticate_JointAcc;
    account->ID = max_Account_ID;
    int seekAmount = max_Account_ID - 1; 

    int fd = open("account_database.dat", O_RDWR,0744);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(struct Account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Account)+ sizeof(int);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    write(fd, account, sizeof(struct Account));

    write(fd,&max_Account_ID, sizeof(max_Account_ID));
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

void Admin_Dlt_Acc(int new_sd)
{
    int Account_ID;
    read(new_sd, &Account_ID, sizeof(Account_ID));

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    account = getAccount(Account_ID);

    int seekAmount = account->ID - 1;

    int fd = open("account_database.dat", O_RDWR,0744);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(struct Account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Account);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    account->ID = -1;
    write(fd, account, sizeof(struct Account));
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);


    struct User* user = (struct User*)malloc(sizeof(struct User));
    int fd1 = open("user_database.dat", O_RDWR);

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    fcntl(fd1, F_SETLK, &lock);
    lseek(fd1,0,SEEK_SET);
    while(read(fd1, user, sizeof(struct User)))
    {
        int uid = user->Account_ID;
        if (user->Account_ID == Account_ID)
        {
            user->Account_ID = -1;
            lseek(fd1,-1*sizeof(struct User), SEEK_CUR);
            write(fd1, user, sizeof(struct User));
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd1, F_SETLK, &lock);
    close(fd1);
}

void Admin_find_Acc(int new_sd)
{
    int Account_ID;
    read(new_sd, &Account_ID, sizeof(Account_ID));

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    account = getAccount(Account_ID);

    int seekAmount = Account_ID - 1;

    int fd = open("account_database.dat", O_RDWR,0744);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(struct Account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct Account);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);
    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    read(fd, account, sizeof(struct Account));

    if (account->Authenticate_JointAcc == 0)
        account->Authenticate_JointAcc = 1;
    else if (account->Authenticate_JointAcc == 1)
        account->Authenticate_JointAcc = 0;
    
    lseek(fd,seekAmount*sizeof(struct Account),SEEK_SET);
    write(fd, account, sizeof(struct Account));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}