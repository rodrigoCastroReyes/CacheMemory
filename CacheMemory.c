#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NodeList{
	struct NodeList *next;
	struct NodeList *prev; 
	void *value;
}NodeList;

typedef struct List
{
	struct NodeList *front,*end;
}List;

typedef List Queue;

List *listNew(){
	List *l=(List*)malloc(sizeof(List));
	l->front=NULL;
	l->end=NULL;
	return l;
}

Queue *queueNew(){
	return listNew();
}

NodeList *nodeListNew(void*value){
	NodeList *node=(NodeList*)malloc(sizeof(NodeList));
	node->next=NULL;
	node->value=value;
	return node;
}

void enQueue(List *l,NodeList *node){//listAddNodeFirst
	if(l->front==NULL && l->end == NULL){
		l->front=l->end=node;
	}else{
		node->prev=NULL;
		node->next=l->front;
		l->front->prev=node;
		l->front=node;		
	}
}

NodeList *deQueue(List*l){//listRemoveNodeLast
	NodeList *fin = l->end;
	if(l->front==l->end){
		l->front=l->end=NULL;
		return fin;
	}else{
		l->end=fin->prev;
		fin->prev->next=NULL;
		fin->prev=NULL;
		return fin;
	}
}

int listIsEmpty(List*l){
	if(l->front == NULL && l->end == NULL){
		return 1;
	}
	return 0;
}

typedef struct Item{
	char *reference;
}Item;

Item *itemNew(char*reference){
	Item *i=(Item*)malloc(sizeof(Item));
	i->reference=(char*)malloc(sizeof(char)*strlen(reference));
	strcpy(i->reference,reference);
	return i;
}

void itemPrint(Item *item){
	printf("%s\n",item->reference);
}

typedef struct Cubeta{
	List *items;
}Cubeta;

Cubeta *cubetaNew(){
	Cubeta *c=(Cubeta*)malloc(sizeof(Cubeta));
	c->items=listNew();
	return c;
}

typedef struct HashTable{
	int size;
	Cubeta **cubetas;
}HashTable;

HashTable *hashTableNew(int size){
	HashTable *h = (HashTable*)malloc(sizeof(HashTable));
	int i;
	h->size=size;
	h->cubetas = (Cubeta**) malloc(sizeof(Cubeta*)*size);
	for(i=0;i<size;i++){
		h->cubetas[i]=cubetaNew();
	}
	return h;
}

int hash(char *reference,int size){
	return (unsigned int)strlen((char *)reference)%size;
}

void hashTableInsert(HashTable *table,Item*item){
	int index=hash(item->reference,table->size);
	Cubeta *cubeta=table->cubetas[index];
	List *list=cubeta->items;
	enQueue(list,nodeListNew(item));
}

Item *hashTableGet(HashTable *table,char *reference){
	int index=hash(reference,table->size);
	Cubeta *cubeta=table->cubetas[index];
	List *list=cubeta->items;
	NodeList*it;
	Item* item=NULL;
	if(listIsEmpty(list)){
		printf("vacio\n");
		return NULL;
	}
	for(it=list->front;it!=NULL;it=it->next){
		item=(Item*)it->value;
		if(strcmp(item->reference,reference)==0){
			return item;
		}
	}
	return NULL;
}


int main(){
	printf("Demo Data Structures \n");
	List *list=listNew();
	Item *i=itemNew("/documents/holamundo");
	Item *l=itemNew("/documents/holamendk");
	Item *j=itemNew("/documents/chao");
	Item *k=itemNew("/documents/hola");
	
	HashTable*tabla=hashTableNew(20);
	hashTableInsert(tabla,i);
	hashTableInsert(tabla,j);
	hashTableInsert(tabla,k);
	hashTableInsert(tabla,l);

	Item *buscado=hashTableGet(tabla,"/documents/holamendk");
	if(buscado){
		itemPrint(buscado);
	}else{
		printf("no esta \n");
	}
	
}
