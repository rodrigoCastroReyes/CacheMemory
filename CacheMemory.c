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

typedef struct Item {
    NodeList *parent;
    int index;
    char *reference;
    int bit;
} Item;

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
    NodeList *hand;
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
void circleListMake(List*list);
void circleListRemove(List*list, NodeList*it);
void circleListAdd(List*list,NodeList *node);
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

void memoryCacheInsertLRU(MemoryCache*mem,Queue*queue, char*reference);
void memoryCacheInsertLRUK(MemoryCache*mem,Queue*queue, char*reference, int k);
void memoryCacheInsertClock(MemoryCache*mem, List*list, char*reference);

int clockAlgorithm(MemoryCache *mem, List *references,Item *newItem);
void memoryCachePrint(MemoryCache*mem);
void printList(List*list);

void testLRUAlgorithm();
void testClockAlgorithm();
void testLRUKAlgorithm();


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

void circleListMake(List*list){
    if (list->front == list->end ) {
        return;
    }else{
        list->end->next = list->front;
        list->front->prev = list->end;
    }
}

void circleListRemove(List*list, NodeList*it){
    if (list->front == list->end) {
        list->front = list->end = NULL;
        return;
    }
    if (it == list->front) {
        list->front = it->next;
        it->next->prev = NULL;
        it->next = NULL;
        list->end->next = list->front;
        list->front->prev = list->end;
    } else if (it == list->end) {
        list->end = it->prev;
        it->prev->next = NULL;
        it->prev = NULL;
        it->next = NULL;
        list->end->next = list->front;
        list->front->prev = list->end;
    } else {
        NodeList *p = it->prev;
        NodeList *n = it->next;
        p->next = n;
        n->prev = p;
        it->next = NULL;
        it->prev = NULL;
    }
}

void circleListAdd(List*list,NodeList *node){
    enQueue(list,node);
    list->end->next = node;
    list->front->prev = list->end;
}



Item *itemNew(char*reference){
    Item *i=(Item*)malloc(sizeof(Item));
    i->parent=NULL;
    i->reference=(char*)malloc(sizeof(char)*strlen(reference)+1);
    memset(i->reference,'0',sizeof(char)*strlen(reference)+1);
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

void hashTableRemove(HashTable *table,Item*remItem){
    List *list = hashTableGetList(table, remItem->reference);
    if (list != NULL) {
        NodeList*it;
        Item*item;
        for (it = list->front; it != NULL; it = it->next) {
            item = (Item*) it->value;
            if (strcmp(item->reference, remItem->reference) == 0) {
                nodeListRemove(list, it);
                break;
            }
        }
    }
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

void memoryCacheInsertLRU(MemoryCache*mem,Queue*queue, char*reference){
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
        queue->front->prev = node;
        node->prev = NULL;
        queue->front = node;
    }
}

void memoryCacheInsertLRUK(MemoryCache*mem,Queue*queue, char*reference, int k){
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
            int index=lrukAlgorithm(mem,queue,k);
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

void nodeListRemove(List*list, NodeList*it) {
    if (list->front == list->end) {
        list->front = list->end = NULL;
        return;
    }
    if (it == list->front) {
        list->front = it->next;
        it->next->prev = NULL;
        it->next = NULL;
    } else if (it == list->end) {
        list->end = it->prev;
        it->prev->next = NULL;
        it->prev = NULL;
    } else {
        NodeList *p = it->prev;
        NodeList *n = it->next;
        p->next = n;
        n->prev = p;
        it->next = NULL;
        it->prev = NULL;
    }
}

int lruAlgorithm(MemoryCache*mem, Queue*queue) {
    NodeList*remPage = deQueue(queue);//quitar el ultimo de la cola
    Item *remItem = (void*) remPage->value;
    hashTableRemove(mem->table,remItem);//remover de la tabla de hash
    return remItem->index;//retornar el indice en donde se debe ingresar el nuevo elemento en la cache
}


int lrukAlgorithm(MemoryCache*mem,Queue*queue, int k){
    NodeList *node;
    NodeList *t;
    for ( t = queue->end; t!=NULL; t=t->prev)
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


void memoryCacheInsertClock(MemoryCache*mem, List*list, char*reference) {
    Item *rPage = hashTableGet(mem->table, reference);
    if (rPage == NULL) {//hubo un miss dado que la pagina no esta en la cache
        mem->misses++;
        Item *newItem = itemNew(reference);
        NodeList*parent = nodeListNew(newItem);
        newItem->parent = parent;
        if (!memoryCacheIsFull(mem)) {//si hay espacio en la cache ingreso el nuevo elemento
            mem->data[mem->currentSize] = newItem;
            newItem->index = mem->currentSize;
            mem->currentSize++;
            hashTableInsert(mem->table, newItem);
            circleListAdd(list,parent);
            mem->hand = list->end;
        } else {
            //algoritmos de desalojo
            int index = clockAlgorithm(mem,list,newItem);
            mem->data[index] = newItem;
            newItem->index = index;
            hashTableInsert(mem->table, newItem);
            circleListAdd(list,parent);
        }
    } else {//hubo un hit
        mem->hits++;
        rPage->bit = 1;
    }
}

int clockAlgorithm(MemoryCache *mem, List *references,Item *newItem){
    int index;
    NodeList *nodeHand = mem->hand;
    Item* value = (Item*)nodeHand->value;
    while(value->bit!=0){
        value->bit=0;//le da una segunda oportunidad
        nodeHand = nodeHand->prev;
        value = nodeHand->value;
    }
    index = ((Item*)nodeHand->value)->index;
    nodeHand->value = newItem;
    mem->hand = nodeHand->prev;
    hashTableRemove(mem->table,value); //quita el elemento de la tabla de hash
    return index;//retorna el indice en donde debe ser ubicado el nuevo elemento
}

int getNumLines(char* filename){
    FILE *pipein_fp;
    char readbuf[10];
    char str[80];

    strcpy(str, "wc -l ");
    strcat(str, filename);

       /* Create one way pipe line with call to popen() */
    if (( pipein_fp = popen(str, "r")) == NULL)
    {
        perror("popen");
        return 0;
    }

    fgets(readbuf, sizeof(readbuf), pipein_fp);

    /* Close the pipe */
    pclose(pipein_fp);
    return atoi(readbuf);
}

int main(int argc, char** argv) {

    int sizeCache;
    if (argc != 4) {
        printf("Usage: %s <POLITICA> <size-Cache> <filename>\n",argv[0]);
            return 1;
        }
    sizeCache = atoi(argv[2]); /* convert strings to integers */

    if (strcmp("LRU",argv[1]) == 0)
        testLRUAlgorithm();
    else if (strcmp("LRUK",argv[1]) == 0)
        testLRUKAlgorithm();
    else if (strcmp("CLOCK",argv[1]) == 0)
        testClockAlgorithm();

    return 0;
}

void testLRUAlgorithm(){
    FILE *fp;
    char linea[1000];
    strcpy(linea,"");
    
    if ((fp = fopen("workload.txt", "r")) == NULL){
        printf("error \n");
        return ;
    }

    MemoryCache*mem = memoryCacheNew(8000);
    Queue*queue = queueNew();
    
    while (fgets(linea,1000, fp) != NULL){
        memoryCacheInsertLRU(mem,queue,linea);
    }

    fclose(fp);

    printf("Evaluando una cache algoritmo LRU con %d referencias:\n",mem->hits + mem->misses);
    float total=mem->hits + mem->misses;
	float hitRate = mem->hits/total;
    float missRate = mem->misses/total;
	printf("Hits: %d\tHits Rate: %.4f\n",mem->hits,hitRate);
    printf("Misses: %d\tHits Rate: %.4f\n",mem->misses,missRate);
	free(mem);
    free(queue);
}


void testClockAlgorithm(){
    FILE *fp;
    char linea[1000];
    strcpy(linea,"");
    
    if ((fp = fopen("workload.txt", "r")) == NULL){
        printf("error \n");
        return ;
    }
    
    MemoryCache*mem=memoryCacheNew(800);
    List *list = listNew();
    circleListMake(list);
    
    while (fgets(linea,1000, fp) != NULL){
        memoryCacheInsertClock(mem,list,linea);
    }

    fclose(fp);

    printf("Evaluando una cache algoritmo LRU Clock con %d referencias:\n",mem->hits + mem->misses);
    float total=mem->hits + mem->misses;
	float hitRate = mem->hits/total;
    float missRate = mem->misses/total;
	printf("Hits: %d\tHits Rate: %.4f\n",mem->hits,hitRate);
    printf("Misses: %d\tHits Rate: %.4f\n",mem->misses,missRate);
    
    free(mem);
    free(list);
}

void testLRUKAlgorithm(){
    FILE *fp;
    char linea[1000];
    strcpy(linea,"");
    
    if ((fp = fopen("workload.txt", "r")) == NULL){
        printf("error \n");
        return ;
    }

    MemoryCache*mem = memoryCacheNew(8000);
    Queue*queue = queueNew();
    int i=0;
    while (fgets(linea,1000, fp) != NULL){
        memoryCacheInsertLRUK(mem,queue,linea,2);
        i++;
    }
    //printf("num referencias %d\n",i);
    fclose(fp);

    printf("Evaluando una cache algoritmo LRU-K con %d referencias:\n",mem->hits + mem->misses);
    float total=mem->hits + mem->misses;
	float hitRate = mem->hits/total;
    float missRate = mem->misses/total;
	printf("Hits: %d\tHits Rate: %.4f\n",mem->hits,hitRate);
    printf("Misses: %d\tHits Rate: %.4f\n",mem->misses,missRate);
	free(mem);
    free(queue);
}
