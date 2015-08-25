#include <stdio.h>
#include <stdlib.h>

typedef struct Item{
	int key;
	char *reference;
}Item;

typedef struct Page{
	struct Page *next;
	Item *value;
}Page;

typedef struct Queue
{
	struct Page *front,*end;
}Queue;

typedef struct HashTable
{
	int size;
	int current_size;
	Page **pages;
}HashTable;


HashTable *hashTableNew(int size){
	HashTable *h = (HashTable*)malloc(sizeof(HashTable));
	int i;
	h->size=size;
	h->current_size=0;
	h->pages = (Page**) malloc(sizeof(Page*)*size);
	for(i=0;i<size;i++){
		h->pages[i] = NULL;
	}
	return h;
}

int hashtableIsEmpty(HashTable *table){
	return table->current_size == 0;
}

int hashtableIsFull(HashTable*table){
	return table->current_size==table->size;
}

Page *pageNew(Item*value){
	Page *page=(Page*)malloc(sizeof(Page));
	page->next=NULL;
	page->value=value;
	return page;
}

Item *itemNew(int key,char*reference){
	
}

Queue *queueNew(){
	Queue *q=(Queue*)malloc(sizeof(Queue));
	q->front=NULL;
	q->end=NULL;
	return q;
}

void enQueue(Queue *q,Item *item);
Item *deQueue(Queue*q);

void insertPage(HashTable* hashtable, int key,char *reference){
	if(!hashtableIsFull(hashtable)){
		//insertar en la memoria cache
	}else{
		//algoritmos de desalojo
	}
}


void referencePage(HashTable *hashtable,char **string_reference){


}


int main(){
	printf("hello world \n");
}



