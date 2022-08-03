#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

struct User
{
    int ID;
    char pwd[50];
    int Account_ID;
};

struct Account
{
    int ID;
    int Authenticate_JointAcc;
    int Balance;
};

struct Admin
{
    int Admin_ID;
    char Admin_Pwd[50];
};
