#pragma once

#include <string>
#include <iostream>
#include <cstring>

#define IP        "127.0.0.1"
#define PORT      6666
#define BUFF_SIZE 1024

struct Message 
{
    char buf[BUFF_SIZE];
    int len;
};
