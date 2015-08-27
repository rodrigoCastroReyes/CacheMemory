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
	NodeList *parent;
	int index;
	char *reference;
}Item;

Item *itemNew(char*reference){
	Item *i=(Item*)malloc(sizeof(Item));
	i->parent=NULL;
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

List *hashTableGetList(HashTable*table,char*reference){
	int index=hash(reference,table->size);
	Cubeta *cubeta=table->cubetas[index];
	return cubeta->items;
}

Item *hashTableGet(HashTable *table,char *reference){
	List*list=hashTableGetList(table,reference);
	NodeList*it;
	Item* item=NULL;
	if(listIsEmpty(list)){
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

typedef struct MemoryCache{
	HashTable *table;
	Item**data;
	int misses;
	int hits;
	int size;
	int currentSize;
}MemoryCache;

MemoryCache*memoryCacheNew(int size){
	int i;
	MemoryCache*mem=(MemoryCache*)malloc(sizeof(MemoryCache));
	mem->size=size;
	mem->currentSize=0;
	mem->hits=0;
	mem->misses=0;
	mem->table=hashTableNew(size*size);
	mem->data=(Item**)malloc(sizeof(Item)*size);
	for(i=0;i<size;i++){
		mem->data[i]=NULL;
	}
	return mem;
}

int memoryCacheIsFull(MemoryCache*mem){
	if(mem->currentSize == mem->size){
		return 1;
	}
	return 0;
}

void memoryCacheInsert(MemoryCache*mem,Queue*queue, char*reference){
	Item *rPage=hashTableGet(mem->table,reference);
	if(rPage==NULL){//hubo un miss dado que la pagina no esta en la cache
		mem->misses++;
		Item *newItem=itemNew(reference);
		NodeList*parent=nodeListNew(newItem);
		newItem->parent=parent;
		if(!memoryCacheIsFull(mem)){//si hay espacio en la cache ingreso el nuevo elemento
			mem->data[mem->currentSize]=newItem;
			newItem->index=mem->currentSize;
			mem->currentSize++;
			hashTableInsert(mem->table,newItem);
			enQueue(queue,parent);
		}else{
			//algoritmos de desalojo
			int index=lruAlgorithm(mem,queue);
			mem->data[index]=newItem;
			hashTableInsert(mem->table,newItem);
			enQueue(queue,parent);
		}
	}else{//hubo un hit
		mem->hits++;
		//mover node dentro de la cola si es necesario 
		NodeList *node=rPage->parent;
		if(node==queue->front){//si node ya esta en el tope la cola permanece sin cambio
			return;
		}
		if(node==queue->end){//si node esta al final, lo vuelve a ingresar al tope
			deQueue(queue);
			enQueue(queue,node);
			return;
		}
		//si node esta en el medio lo reingresa al tope
		NodeList *p = node->prev;
		NodeList *n = node->next;

		p->next = n;
		n->prev = p;
		node->next = queue->front;
		node->prev = NULL;
		queue->front = node;
	}
}

void nodeListRemove(List*list,NodeList*it){
	if(list->front == list->end) {
		list->front = list->end = NULL ;
		return;
	}
	if(it==list->front){
		list->front=it->next;
		it->next->prev=NULL;
		it->next=NULL;	
	}else if(it==list->end){
		list->end = it->prev;
		it->prev->next=NULL;
		it->prev=NULL;
	}else{
		NodeList *p = it->prev;
		NodeList *n = it->next;
		p->next = n;
		n->prev = p;
		it->next = NULL;
		it->prev = NULL;
	}
}

int lruAlgorithm(MemoryCache*mem,Queue*queue){
	NodeList*remPage=deQueue(queue);
	//remover de la tabla de hash
	Item *remItem=(void*)remPage->value;
	List *list=hashTableGetList(mem->table,remItem->reference);
	if(list!=NULL){
		NodeList*it;
		Item*item;
		for(it=list->front;it!=NULL;it=it->next){
			item=(Item*)it->value;
			if(strcmp(item->reference,remItem->reference)==0){
				nodeListRemove(list,it);
				break;
			}
		}
	}
	//retornar el indice en donde se debe ingresar el nuevo elemento en la cache
	return remItem->index;
}

void memoryCachePrint(MemoryCache*mem){
	int i;
	for(i=0;i<mem->size;i++){
		if(mem->data[i]!=NULL){
			itemPrint(mem->data[i]);
		}
	}
}

void printList(List*list){
	NodeList*it;
	Item*item;
	for(it=list->front;it!=NULL;it=it->next){
		item=(Item*)it->value;
		itemPrint(item);
	}
}

int main(){
	MemoryCache*mem=memoryCacheNew(10);
	Queue*queue=queueNew();
	memoryCacheInsert(mem,queue,"10");
	memoryCacheInsert(mem,queue,"10");
	memoryCacheInsert(mem,queue,"50");
	memoryCacheInsert(mem,queue,"30");
	memoryCacheInsert(mem,queue,"20");
	memoryCacheInsert(mem,queue,"15");
	memoryCacheInsert(mem,queue,"20");
	memoryCacheInsert(mem,queue,"50");
	//printList(queue);
	memoryCachePrint(mem);
	printf("hits: %d\n",mem->hits);
	printf("misses: %d\n",mem->misses);
}

