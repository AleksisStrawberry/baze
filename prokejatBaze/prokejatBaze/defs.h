#pragma once
#define WIN32_LEAN_AND_MEAN
#define FB 5

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <time.h>
#include <math.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

typedef SYSTEMTIME DateTime;

typedef struct indTreeNode {
	int id1;
	int id2;
	int addrId1;
	int addrId2;
}IndTreeNode;


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

node* insert_node(node *root, Weapon w);
void dispose(node* root);
void display_tree(node* nd);
void treeIntoFile(node* nd, FILE* file);