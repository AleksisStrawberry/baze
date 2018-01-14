#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <time.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

typedef SYSTEMTIME DateTime;
typedef struct weapon
{
	int id;
	char maker[50];
	char mark[50];
	double price;
	DateTime date;
}Weapon;

typedef struct node
{
	Weapon data;

	struct node* left;
	struct node* right;
} node;