#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOM_ARCHIVO  "/Users/Jhon/Dropbox/Espol/Sistemas Operativos/Proyecto/Evaluacion de Cache/EvaluadorCache/workload.txt"

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

typedef struct Item{
    NodeList *parent;
    int index;
    char *reference;
}Item;

typedef struct Cubeta{
    List *items;
}Cubeta;

typedef struct HashTable{
    int size;
    Cubeta **cubetas;
}HashTable;

typedef struct MemoryCache{
    HashTable *table;
    Item**data;
    int misses;
    int hits;
    int size;
    int currentSize;
}MemoryCache;

//Encabezados
/*******************************************************/

int lruAlgorithm(MemoryCache*mem,Queue*queue);
List *listNew();
Queue *queueNew();
NodeList *nodeListNew(void*value);
NodeList *deQueue(List*queue);
int listIsEmpty(List*l);
void enQueue(List *queue,NodeList *node);
Item *itemNew(char*reference);
void itemPrint(Item *item);
Cubeta *cubetaNew();
HashTable *hashTableNew(int size);
int hash(char *reference,int size);
void hashTableInsert(HashTable *table,Item*item);
List *hashTableGetList(HashTable*table,char*reference);
Item *hashTableGet(HashTable *table,char *reference);
MemoryCache *memoryCacheNew(int size);
int memoryCacheIsFull(MemoryCache*mem);
void nodeListRemove(List*list,NodeList*it);
int lruAlgorithm(MemoryCache*mem,Queue*queue);
int lrukAlgorithm(MemoryCache*mem,Queue*queuem, int k);
void memoryCachePrint(MemoryCache*mem);
void printList(List*list);

/*******************************************************/

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

void enQueue(List *queue,NodeList *node){//listAddNodeFirst
    if(queue->front==NULL && queue->end == NULL){
        queue->front=queue->end=node;
    }else{
        node->prev=NULL;
        node->next=queue->front;
        queue->front->prev=node;
        queue->front=node;
    }
}

NodeList *deQueue(List*queue){//listRemoveNodeLast
    NodeList *node = queue->end;
    if(queue->front==queue->end){
        queue->front=queue->end=NULL;
        return node;
    }else{
        
        queue->end=node->prev;
        node->prev->next=NULL;
        node->prev=NULL;
        
        return node;
    }
}


int listIsEmpty(List*l){
    if(l->front == NULL && l->end == NULL){
        return 1;
    }
    return 0;
}


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


Cubeta *cubetaNew(){
    Cubeta *c=(Cubeta*)malloc(sizeof(Cubeta));
    c->items=listNew();
    return c;
}

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


MemoryCache *memoryCacheNew(int size){
    int i;
    MemoryCache *mem = (MemoryCache*)malloc(sizeof(MemoryCache));
    mem -> size = size;
    mem -> currentSize = 0;
    mem -> hits = 0;
    mem -> misses = 0;
    mem -> table = hashTableNew(size*2);
    mem -> data = (Item**)malloc(sizeof(Item)*size);
    for(i = 0; i < size; i++){
        mem -> data[i] = NULL;
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
            int index=lrukAlgorithm(mem,queue,2);
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
        queue->front->prev = node;
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

int lrukAlgorithm(MemoryCache*mem,Queue*queue, int k){
    NodeList *node;
    for (NodeList* t = queue->end; t!=NULL; t=t->prev)
    {
        k--;
        if (k == 0){
            node = t;
            break;
        }
    }

    NodeList *p = node->prev;
    NodeList *n = node->next;
    p->next = n;
    n->prev = p;
    node->next = NULL;
    node->prev = NULL;
    
    //remover de la tabla de hash
    Item *remItem=(void*)node->value;
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
    FILE *fp;
    char linea[350];
    
    if ((fp = fopen(NOM_ARCHIVO, "r")) == NULL){
        perror(NOM_ARCHIVO);
        return EXIT_FAILURE;
    }
    
    MemoryCache*mem=memoryCacheNew(100);
    Queue*queue=queueNew();
    
    while (fgets(linea, 300, fp) != NULL)
    {
        memoryCacheInsert(mem,queue,linea);
    }
    fclose(fp);
    
//    memoryCacheInsert(mem,queue,"1");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"2");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"3");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"4");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"1");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"2");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"5");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"1");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"2");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"6");
//    //memoryCachePrint(mem);
//    memoryCacheInsert(mem,queue,"5");
//    memoryCacheInsert(mem,queue,"4");
    
//    memoryCachePrint(mem);
    
    printf("hits: %d\n",mem->hits);
    printf("misses: %d\n",mem->misses);
    
    free(mem);
    free(queue);
}

