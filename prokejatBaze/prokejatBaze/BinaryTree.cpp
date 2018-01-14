#include "defs.h"



int compare(int left, int right)
{
	if (left > right)
		return 1;
	if (left < right)
		return -1;
	return 0;
}
node* create_node(Weapon w)
{
	node *new_node = (node*)malloc(sizeof(node));
	if (new_node == NULL)
	{
		fprintf(stderr, "Out of memory!!! (create_node)\n");
		exit(1);
	}
	new_node->data = w;
	
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}


node* insert_node(node *root, Weapon w) 
{

	if (root == NULL)
	{
		root = create_node(w); 
	}
	else
	{
		int is_left = 0;
		int r = 0;
		node* cursor = root;
		node* prev = NULL;

		while (cursor != NULL)
		{
			r = compare(w.id, cursor->data.id); 
			prev = cursor;
			if (r < 0)
			{
				is_left = 1;
				cursor = cursor->left;
			}
			else if (r > 0)
			{
				is_left = 0;
				cursor = cursor->right;
			}
		}
		if (is_left)
			prev->left = create_node(w);
		else
			prev->right = create_node(w); 
	}
	return root;
}

void dispose(node* root)
{
	if (root != NULL)
	{
		dispose(root->left);
		dispose(root->right);
		free(root);
	}
}

void display_tree(node* nd)
{
	if (nd == NULL)
		return;
	
	display_tree(nd->left);
	
	//printf("%d %s %s %d %d %u\n", nd->data.id, nd->firstName, nd->lastName, nd->year, nd->ects, nd->department);
	printf("ID is %d\nMaker is %s\nMark is %s\nPrice is %lf\nDate is: %u %u %u %u %u %u\n", nd->data.id, nd->data.maker, nd->data.mark, nd->data.price, nd->data.date.wDay, nd->data.date.wMonth, nd->data.date.wYear, nd->data.date.wHour, nd->data.date.wMinute, nd->data.date.wSecond);
	display_tree(nd->right);
}


	

