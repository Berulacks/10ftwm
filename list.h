#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct node
{
	int data;
	struct node* next;
};

struct node* createList(int data)
{
	struct node* x = malloc( sizeof( struct node ) );
	x->data = data;
	x->next = NULL;

	return x;
}

int sizeOfList(struct node head)
{
	if(isnan(head.data) || isinf(head.data) )
		return 0;
	int size = 0;
	struct node* x = &head;	

	while(x->next != NULL)
	{
		size++;
		x = x->next;
	}

	return size;
}

int getFromList(struct node head, int index)
{
	if(index >= sizeOfList(head))
	{
		printf("ERROR: index out of range\n");
		//This will always break something since we only ever
		//use lists with unsigned ints.
		return -1;
	}

	struct node* x = &head;

	for(int i = 0; i <= index; i++)
		x = x->next;

	return x->data;
}

void appendToList(struct node* head, int data)
{
	struct node* x = malloc( sizeof(struct node) );
	x->data = data;
	x->next = NULL;

	if(head->next == NULL)
	{
		head->next = x;
	}
	else
	{
		struct node* itr = head;
		for(int i = 0; i < sizeOfList(*head); i++)
			itr = itr->next;
		itr->next = x;
	}
	
}

void removeFromList(struct node* head, int index)
{
	int size = sizeOfList(*head);
	if(size == 0)
		return;
	if( size == 1 )
	{
		head->next = NULL;
		return;
	}

	struct node* x = head;
	//Iterate all the way to i-1
	for( int i = 0; i < index; i++)
		x = x->next;

	struct node* toDie = x->next;
	x->next = toDie->next;
	
	toDie->next = NULL;
	free(toDie);
}

int indexOf( struct node list, int data )
{
	int index;
	int size = sizeOfList(list);
	struct node* x = &list;

	for(index = 0; index < size; index++)
	{
		x = x->next;
		if(x->data == data)
			return index;
	}	

	return index;

}

void printList ( struct node list )
{
	int size = sizeOfList(list);
	printf("List has size of %i, and contains the data:\n", size);
	for(int i = 0; i < size; i++)
		printf("[ %i ]\n", getFromList(list, i) );
}

/*int main (int argc, char **argv)
{
	int a = 1;
	struct node list = *createList(a);
	int b = 2;
	int c = 3;
	int d = 4;

	appendToList(&list, a);
	printList(list);
	appendToList(&list, b);
	printList(list);
	appendToList(&list, c);
	printList(list);
	removeFromList(&list, 1);
	printList(list);
	appendToList(&list, d);
	printList(list);
}*/
