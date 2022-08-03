#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "all_databases.h"

#define PORT_NUMBER 5500

void Validate_Client(int);
void Normal_User_Login(int,int);
void Show_Account_Details(int);
void Ammount_Deposit(int);
void Ammount_Widhdraw(int);
void ChangePwd(int);
void Admin_User_Login(int);
void Admin_Add_Acc(int);
void Admin_Dlt_Acc(int);
void Admin_Mod_Acc(int);
void Admin_find_Acc(int);


int main()
{
    int sd;
    struct sockaddr_in server;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == 0)
    {
        printf("Oops, Error in making socket\n");
        exit(0);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUMBER);
    int connection_result = connect(sd, (void*)&server, sizeof(server));
    if (connection_result < 0)
    {
        printf("Oops, Error in connect()\n");
    }

    Validate_Client(sd);
    return 0;
}

void Validate_Client(int sd)
{
    write(1,"1 -> For Normal User Login\n",sizeof("0 -> For Normal User Login\n"));
    write(1,"2 -> For Login as A Joint account user\n",sizeof("2 -> For Login as A Joint account user\n"));
    write(1,"3 -> For login as an Admin\n",sizeof("1 -> For login as an Admin\n"));
    write(1,"Any Other Number except 1/2/3 == Exit\n",sizeof("Any Other Number except 1/2/3 == Exit\n"));

    int option;
    scanf("%d",&option);

    if (option == 1)
    {
        write(1,"You are going to Login as a normal user\n",sizeof("You are going to Login as a normal user\n"));
        Normal_User_Login(sd, option);
    }

    else if (option == 2)
    {
        write(1,"You are going to Login as a joint account user \n",sizeof("You are going to Login as a joint account user \n"));
        Normal_User_Login(sd, option);
    }

    else if (option == 3)
    {
        write(1,"You are going to Login as an administrative user \n",sizeof("You are going to Login as an administrative user \n"));
        Admin_User_Login(sd);
    }

    else
    {
        write(1,"Thank you for using our BMS(Banking Management System)\n",sizeof("Thank you for using our BMS(Banking Management System)\n"));
        option = 4;
        write(sd, &option, sizeof(option));
        close(sd);
        exit(0);
    }

    Validate_Client(sd);
}


void Normal_User_Login(int sd, int option)
{
    // int loginoption = 1;
    write(sd, &option, sizeof(option));
    
    int userID;
    char password[100];
    printf("Please Enter your User ID: ");
    scanf("%d", &userID);
    printf("\n");
    printf("Please Enter password: ");
    scanf("%s", password);
    
    write(sd, &userID, sizeof(userID));
    write(sd,&password,sizeof(password));

    bool Authenticate_It;
    read(sd, &Authenticate_It, sizeof(Authenticate_It));

    if (Authenticate_It)
    {
      
        write(1,"Congrtulations, Successfully Logged In\n",sizeof("Congrtulations, Successfully Logged In\n"));

        while(true)
        {
            write(1, "1 -> Want to Deposit\n", sizeof("1 -> Want to Deposit\n")); 
            write(1, "2 -> Want to Withdraw\n", sizeof("2 -> Want to Withdraw\n")); 
            write(1, "3 -> Balance Enquiry?\n", sizeof("3 -> Balance Enquiry?\n")); 
            write(1, "4 -> Want to Change password?\n", sizeof("4 -> Want to Change password?\n")); 
            write(1, "5 -> View account details\n", sizeof("5 -> View account details\n")); 
            write(1, "6 -> Exit\n", sizeof("6 -> Exit\n")); 

            struct Account* account = (struct Account*)malloc(sizeof(struct Account));
            int option;
            scanf("%d", &option);
            switch(option)
            {
                case 1:
                    Ammount_Deposit(sd);
                    break;
                case 2:
                    Ammount_Widhdraw(sd);
                    break;
                case 3:
                    option = 3;
                    write(sd,&option, sizeof(option));
                    read(sd,account, sizeof(struct Account));
                    printf("Account Balance: %d\n", account->Balance);
                    break;
                case 4:
                    ChangePwd(sd);
                    break;
                case 5:
                    Show_Account_Details(sd);
                    break;
                case 6:
                    option = 6;
                    write(sd,&option, sizeof(option));
                    close(sd);
                    exit(0);
                    break;
            }
        }
    }

    else
    {
        write(1,"Oops, Error in Authenicating the User\n",sizeof("Oops, Error in Authenicating the User\n"));
        write(1,"Sorry , Login Failed\n",sizeof("Sorry , Login Failed\n"));
        Validate_Client(sd);
    }
}

void Show_Account_Details(int sd)
{
    int option = 5;
    write(sd,&option, sizeof(option));

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    read(sd,account, sizeof(struct Account));

    printf("Account details are:\n");
    printf("Account ID: %d\n",account->ID);
    printf("Account Balance: %d\n",account->Balance);
    if (account->Authenticate_JointAcc == 1)
        printf("Account Type: Joint Account\n");
    else if (account->Authenticate_JointAcc == 0)
        printf("Account Type: Normal Account\n");
}

void Ammount_Deposit(int sd)
{
    int option = 1;
    write(sd,&option, sizeof(option));

    printf("Please Enter Amount to be Deposited:\n");
    int amount;
    scanf("%d",&amount);

    write(sd,&amount, sizeof(amount));

    printf("Amount has been successfully Deposited\n");

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    read(sd,account, sizeof(struct Account));

    printf("Current Balance: %d\n",account->Balance);
}

void Ammount_Widhdraw(int sd)
{
    int option = 2;
    write(sd,&option, sizeof(option));

    printf("Please Enter Amount to be withdrawn:\n");
    int amount;
    scanf("%d",&amount);

    write(sd,&amount, sizeof(amount));

    printf("Amount has been successfully withdrawn\n");

    bool Balancehaschanged;
    read(sd,&Balancehaschanged,sizeof(Balancehaschanged));
    if (Balancehaschanged == false)
        printf("Sorry, Account Balance insufficient\n");

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    read(sd,account, sizeof(struct Account));

    printf("Current Balance: %d\n",account->Balance);
}

void ChangePwd(int sd)
{
    int option = 4;
    write(sd,&option, sizeof(option));

    printf("Kindly enter your new password\n");
    char new_password[100];
    scanf("%s",new_password);

    write(sd,&new_password, sizeof(new_password));
}

void Admin_User_Login(int sd)
{
    int loginoption = 3;
    write(sd, &loginoption, sizeof(loginoption));
    
    int Admin_ID;
    char password[100];
    printf("Please enter Admin User ID: ");
    scanf("%d", &Admin_ID);
    printf("\n");
    printf("Enter Admin's password: ");
    scanf("%s", password);
    
    write(sd, &Admin_ID, sizeof(Admin_ID));
    write(sd,&password,sizeof(password));

    bool Authenticate_It;
    read(sd, &Authenticate_It, sizeof(Authenticate_It));

    if (Authenticate_It)
    {
        write(1,"You are now logged in as ADMIN\n",sizeof("You are now logged in as ADMIN\n"));

        while(true)
        {
            write(1, "1 -> Want to Add account\n", sizeof("1 -> Want to Add account\n")); 
            write(1, "2 -> Want to Delete Account\n", sizeof("2 -> Want to Delete Account\n")); 
            write(1, "3 -> Want to Modify Account\n", sizeof("3 -> Want to Modify Account\n")); 
            write(1, "4 -> Want to find account's details\n", sizeof("4 -> Want to find account's details\n")); 
            write(1, "6 -> Exit\n", sizeof("6 -> Exit\n")); 

            int option;
            
            scanf("%d", &option);
            switch(option)
            {
                case 1:
                    Admin_Add_Acc(sd);
                    break;
                case 2:
                    Admin_Dlt_Acc(sd);
                    break;
                case 3:
                    
                    Admin_Mod_Acc(sd);
                    break;
                case 4:
                    Admin_find_Acc(sd);
                    break;
                case 5:
                    
                    break;
                case 6:
                    option = 6;
                    write(sd,&option, sizeof(option));
                    close(sd);
                    exit(0);
                    break;
            }
        }
    }

    else
    {
        write(1,"Oops, Error in admin authentication\n",sizeof("Oops, Error in admin authentication\n"));
        write(1,"Sorry Admin Login Failed\n",sizeof("Sorry Admin Login Failed\n"));
        Validate_Client(sd);
    }
}

void Admin_Add_Acc(int sd)
{
    int option = 1;
    write(sd, &option, sizeof(option));

    printf("Enter the Account Balance: ");
    int Acc_Balance;
    scanf("%d",&Acc_Balance);
    write(sd, &Acc_Balance, sizeof(Acc_Balance));

    printf("Please Type '0' if it is a Normal account / '1' if it is a Joint account: ");
    int Authenticate_JointAcc;
    scanf("%d",&Authenticate_JointAcc);
    write(sd, &Authenticate_JointAcc, sizeof(Authenticate_JointAcc));
}

void Admin_Dlt_Acc(int sd)
{
    int option = 2;
    write(sd, &option, sizeof(option));

    printf("Enter the Account ID to delete the account related: ");
    int account_id;
    scanf("%d",&account_id);
    write(sd, &account_id, sizeof(account_id));
}

void Admin_Mod_Acc(int sd)
{
    int option = 3;
    write(sd, &option, sizeof(option));

    printf("Enter the account ID of account to modify it: ");
    int account_id;
    scanf("%d",&account_id);
    write(sd, &account_id, sizeof(account_id));
}

void Admin_find_Acc(int sd)
{
    int option = 4;
    write(sd, &option, sizeof(option));

    printf("Enter account ID of account you want to searc: ");
    int account_id;
    scanf("%d",&account_id);
    
    write(sd, &account_id, sizeof(account_id));

    struct Account* account = (struct Account*)malloc(sizeof(struct Account));
    read(sd,account, sizeof(struct Account));

    printf("Account details are:\n");
    printf("Account ID: %d\n",account->ID);
    printf("Account Balance: %d\n",account->Balance);
    if (account->Authenticate_JointAcc == 1)
        printf("Account Type: Joint Account\n");
    else if (account->Authenticate_JointAcc == 0)
        printf("Account Type: Normal Account\n");
}