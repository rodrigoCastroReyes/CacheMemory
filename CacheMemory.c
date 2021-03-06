#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
//http://www.mathcs.emory.edu/~cheung/Courses/355/Syllabus/9-virtual-mem/SC-replace.html

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

/*heap*/
typedef void *Generic;
typedef int (*cmpfn)(Generic, Generic);
typedef enum {ASC, DESC} TSort;
typedef struct Heap{
	Generic *Data;
	int max;
	int nData;
	TSort type;
    cmpfn cmp;
}Heap;
/*heap*/

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

typedef struct Entry{
    char *reference;
    int index;
}Entry;


//Encabezados
/*******************************************************/

/*List*/
List *listNew();
Queue *queueNew();
NodeList *nodeListNew(void*value);
int listIsEmpty(List*l);
NodeList *deQueue(List*queue);
void enQueue(List *queue,NodeList *node);
void nodeListRemove(List*list,NodeList*it);
void printList(List*list);
/*CircleList*/
void circleListMake(List*list);
void circleListRemove(List*list, NodeList*it);
void circleListAdd(List*list,NodeList *node);
/*Item*/
Item *itemNew(char*reference);
void itemPrint(Item *item);
int itemCmp(Item *i,char*reference);
/*Entry: Algorithm Opt*/
Entry *entryNew(char *reference);
int integerCmp(Generic a,Generic b);
int entryComparar(Entry *e,char *reference);
/*Hashtable*/
Cubeta *cubetaNew();
HashTable *hashTableNew(int size);
int hash(char *reference,int size);
void hashTableInsert(HashTable *table,Item*item);
List *hashTableGetList(HashTable*table,char*reference);
Generic hashTableGet(HashTable *table,char *reference,cmpfn itemCmp);
/*Memory Cache*/
MemoryCache *memoryCacheNew(int size);
int memoryCacheIsFull(MemoryCache*mem);
void memoryCachePrint(MemoryCache*mem);
/*Heap*/
static int heapIdxLeft(Heap *H, int idxfather);
static int heapIdxRight(Heap *H, int idxfather);
static int heapIdxFather(Heap *H, int idx);
static void heapFix(Heap *H, int idx);
static int heapCompareXType(Heap *H, int idxa, int idxb);
static void heapSwap(Heap *H, int idxa, int idxb);
int heapIsEmpty(Heap *H);
Heap *heapNew(int max, TSort type, cmpfn cmp);
Generic heapDeQueue(Heap *H);
void heapEnQueue(Heap *H, Generic G);
int heapGetSize(Heap *H);
void heapMake(Heap *H);
Generic heapGetDatum(Heap *H, int i);

/*Algoritmo LRU*/
void memoryCacheInsertLRU(MemoryCache*mem,Queue*queue, char*reference);
int lruAlgorithm(MemoryCache*mem,Queue*queue);

/*Algoritmo LRUk*/
void memoryCacheInsertLRUK(MemoryCache*mem,Queue*queue, char*reference, int k);
int lrukAlgorithm(MemoryCache*mem,Queue*queuem, int k);

/*Algoritmo Clock*/
void memoryCacheInsertClock(MemoryCache*mem, List*list, char*reference);
int clockAlgorithm(MemoryCache *mem, List *references,Item *newItem);

/*Algoritmo Optimo*/
int getNumLines(char* filename);
char **listReference(char *filePath,int *nlines);
void memoryCacheInsertOptimal(MemoryCache*mem,char**listReferences,char *reference,int currentIndex,int sizeList 
	,HashTable * references);
int optimalAlgorithm(MemoryCache *mem, char **references, int currentIndex, int sizeList, HashTable *table);
void hashTableInsertEntry(HashTable*table, char *ref,int index);

/*Test*/
void testLRUAlgorithm(char *filePath, int sizeCache);
void testLRUKAlgorithm(char *filePath, int sizeCache);
void testClockAlgorithm(char *filePath, int sizeCache);
void testOptimalAlgorithm(char *filePath,int sizeCache);

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
    i->bit = 1;
    return i;
}

Generic integerNew(int newvalue){
    Generic g;
    g = malloc(sizeof(int));
    *(int *)g = newvalue;
    return g;
}

int integerCmp(Generic a,Generic b){
    int x,y;
    x = *((int*)a);
    y = *((int*)b);
    if(x>y){
        return 1;
    }
    if(x<y){
        return -1;
    }
    return 0;
}

Entry *entryNew(char *reference){
    Entry *e = (Entry*)malloc(sizeof(Entry));
    e->reference=(char*)malloc(sizeof(char)*strlen(reference)+1);
    memset(e->reference,'0',sizeof(char)*strlen(reference)+1);
    strcpy(e->reference,reference);
    return e;
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
/*
int hash( char*reference, int size ) {
	unsigned long int hashval;
	int i = 0;
	/* Convert our string to an integer 
	while( hashval < ULONG_MAX && i < strlen( reference ) ) {
		hashval = hashval << 8;
		hashval += reference[ i ];
		i++;
	}
	return hashval % size;
}*/

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

int itemCmp(Item *i,char*reference){
    return strcmp(i->reference,reference);
}

int entryComparar(Entry *e,char *reference){
    return strcmp(e->reference,reference);
}

Generic hashTableGet(HashTable *table,char *reference, cmpfn compare){
    List*list=hashTableGetList(table,reference);
    NodeList*it;
    if(listIsEmpty(list)){
        return NULL;
    }
    for(it=list->front;it!=NULL;it=it->next){
        if(compare(it->value,reference)==0){
            return it->value;
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
    Item *rPage=hashTableGet(mem->table,reference,(cmpfn)itemCmp);
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
        if(node == queue->front){//si node ya esta en el tope la cola permanece sin cambio
            return;
        }
        if(node == queue->end){//si node esta al final, lo vuelve a ingresar al tope
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
    Item *rPage=hashTableGet(mem->table,reference,(cmpfn)itemCmp);
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

/*Heap*/
int heapIsEmpty(Heap *H){
	return (H->nData == 0);
}

Heap *heapNew(int max, TSort type, cmpfn cmp){
	Heap *H;
	int i;
	H = malloc(sizeof(Heap));
	H->Data = malloc(sizeof(Generic)* max);
	for(i = 0; i < max; i++)
		H->Data[i] = NULL;
	H->max = max;
	H->nData = 0;
	H->type = type;
        H->cmp = cmp;
	return H;
}

Generic heapDeQueue(Heap *H){
	Generic gmax;
	if(!heapIsEmpty(H)){
		gmax = H->Data[0];
		heapSwap(H,0,H->nData-1);
		H->nData --;
                heapFix(H, 0);
		return gmax;
	}
	return NULL;
}

void heapEnQueue(Heap *H, Generic G){
	int padre, i;
	if(H->nData < H->max){
		H->Data[H->nData] = G;
		H->nData++;
		i = H->nData-1;
		padre = heapIdxFather(H,i);
		while((i>=0 && padre>=0) && heapCompareXType(H,i, padre)){
			heapSwap(H,i,padre);
			i = padre;
			padre = heapIdxFather(H,i);
		}
	}
}


int heapIdxLeft(Heap *H, int pospadre){
	int indiceizq;
	indiceizq = pospadre *2	+1;
	if(indiceizq < H->nData) return indiceizq;
	return -1;
}
int heapIdxRight(Heap *H, int pospadre){
	int indiceder;
	indiceder = pospadre *2	+2;
	if(indiceder < H->nData) return indiceder;
	return -1;
}
int heapIdxFather(Heap *H, int poshijo){
	int indicepadre;
	indicepadre = (poshijo-1)/2;
	if(poshijo!=0) return indicepadre;
	return -1;
}

static void heapFix(Heap *H, int posnodo){
	int pos_mayor, izq, der;
	pos_mayor = posnodo;
	izq = heapIdxLeft(H, posnodo);
	der = heapIdxRight(H, posnodo);
	if(izq>=0 && heapCompareXType(H,izq,posnodo))
		pos_mayor = izq;
	if(der>=0 && heapCompareXType(H,der,pos_mayor))
		pos_mayor = der;
	if(pos_mayor != posnodo){
		heapSwap(H,pos_mayor,posnodo);
		heapFix(H,pos_mayor);
	}
}

static int heapCompareXType(Heap *H, int idxa, int idxb){
	if(H->type == DESC)
		return (H->cmp(H->Data[idxa],H->Data[idxb]) > 0);
	else if(H->type == ASC)
		return (H->cmp(H->Data[idxa],H->Data[idxb]) < 0);
	return -1;
}

Generic heapGetDatum(Heap *H, int i){
	return H->Data[i];
}

int heapGetSize(Heap *H){
    return H->nData;
}

void heapMake(Heap *H){
	int i;
	for(i = H->nData/2-1; i >= 0; i --){
		heapFix(H, i);
	}
}

static void heapSwap(Heap *H, int idxa, int idxb){
    Generic tmp;
    tmp = H->Data[idxa];
    H->Data[idxa] = H->Data[idxb];
    H->Data[idxb] = tmp;
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

void itemPrint(Item *item){
    printf("%d %s",item->bit,item->reference);
}

void memoryCachePrint(MemoryCache*mem){
    int i;
    for(i=0;i<mem->size;i++){
        if(mem->data[i]!=NULL){
            itemPrint(mem->data[i]);
        }
    }
    printf("\n");
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
    Item *rPage = hashTableGet(mem->table, reference,(cmpfn)itemCmp);
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
            //circleListAdd(list,parent);
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

//Algoritmo optimo 

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

int compararItems(Item *a,Item *b){
	if(a->index == b->index){
		return 0;
	}
	if(a->index > b->index){
		return 1;
	}
	return -1;
}

char **listReference(char *filePath,int *nlines){
    char**list;
    int num_lines = getNumLines(filePath);
    *nlines = num_lines;
    printf("num lineas: %d\n",num_lines);
    int i=0;
    FILE *fp;
    char linea[1340];
    strcpy(linea,"");

    list = (char **)malloc(sizeof(char*)*num_lines);
    
    if ((fp = fopen(filePath, "r")) == NULL){
        printf("error \n");
        return ;
    }

    while (fgets(linea,1340, fp) != NULL){
        list[i]=(char*)malloc(sizeof(char)*strlen(linea)+1);
        strcpy(list[i],linea);
        i++;
    }
    fclose(fp);
    return list;
}



void testOptimalAlgorithm(char *filePath,int sizeCache){
    MemoryCache*mem = memoryCacheNew(sizeCache);
    HashTable *references = hashTableNew(sizeCache*2);
    int flag = 0;
    int i , j;
    int max;
    FILE *fp;
    char linea[1340];
    strcpy(linea,"");
    if ((fp = fopen(filePath, "r")) == NULL){
        printf("error \n");
        return ;
    }
    char **list = listReference(filePath,&max);
    Entry *entry;
    for(i = 0 ; i<max; i++){
    	memoryCacheInsertOptimal(mem,list,list[i],i,max,references);
        if(memoryCacheIsFull(mem)){
        	//llenar la cache con la info del futuro;
        	if(!flag){
        		flag = 1;
        		int size;
        		if( max - i > sizeCache){
			        size = sizeCache;
			    }else{
			        size = max - i - 1;
			    }
        		for(j = i + 2 ; j < i + 2 + size ; j++ ){
        			//printf("elementos: %s\n",list[j]);
        			hashTableInsertEntry(references,list[j],j);
        		}
        	}else{
        		if( max - (i + 1) > sizeCache ){
        			//remover list[i+1] de references
        			List *l = hashTableGetList(references,list[i+1]);
        			Entry *e;
        			if(list!=NULL){
        				NodeList *p;
        				for(p=l->front; p!=NULL;p = p->next){
        					e = (Entry*)p->value;
        					if ( ( i + 1 == e->index) &&( strcmp(e->reference, list[i + 1]) == 0 ) ) {
        			       		nodeListRemove(l,p);
        			        	break;
        			    	}
        				}
        			}
        			//agregar list[i + size + 1] de references
					hashTableInsertEntry(references,list[i + 1 + sizeCache ], ( i + 1 + sizeCache ));
        		}
        	}
        }
    }

    printf("Evaluando una cache algoritmo Optimo con %d referencias:\n",mem->hits + mem->misses);
    float total=mem->hits + mem->misses;
    float hitRate = mem->hits/total;
    float missRate = mem->misses/total;
    printf("Hits: %d\tHits Rate: %.4f\n",mem->hits,hitRate);
    printf("Misses: %d\tHits Rate: %.4f\n",mem->misses,missRate);
    free(mem);
}

void hashTableInsertEntry(HashTable*table, char *ref,int i){
	Entry *entry = entryNew(ref);
	entry->index = i;
	int index = hash(entry->reference,table->size);
	Cubeta *cubeta = table->cubetas[index];
	List *list = cubeta->items;
	enQueue(list,nodeListNew(entry));
	NodeList *it;
	for(it=list->front;it!=NULL;it=it->next){
		Entry *e=it->value;
	}
}

void memoryCacheInsertOptimal(MemoryCache*mem,char**listReferences,char *reference,int currentIndex,int sizeList,
	HashTable * references){
    Item *rPage = hashTableGet(mem->table,reference,(cmpfn)itemCmp);
    if (rPage == NULL) {//hubo un miss dado que la pagina no esta en la cache
        mem->misses++;
        Item *newItem = itemNew(reference);
        if (!memoryCacheIsFull(mem)) {//si hay espacio en la cache ingreso el nuevo elemento
            mem->data[mem->currentSize] = newItem;
            newItem->index = mem->currentSize;
            mem->currentSize++;
            hashTableInsert(mem->table, newItem);
        } else {
            //algoritmo de desalojo
            int index = optimalAlgorithm(mem,listReferences,currentIndex,sizeList,references);
            mem->data[index] = newItem;
            newItem->index = index;
            hashTableInsert(mem->table, newItem);
        }
    } else {//hubo un hit
        mem->hits++;     
    }
}

int optimalAlgorithm(MemoryCache *mem, char **references, int currentIndex, int sizeList, HashTable *table){
    int i, j , size;
    int flag = 0;
    Item *item,*itemTmp,*reqPage;
    Entry *e ;
    if( sizeList - currentIndex > mem->size){
        size = mem->size;
    }else{
        size = sizeList - currentIndex - 1;
    }
    for(i = 0; i < mem->size ; i++){
        item = mem->data[i];
        //printf(" elemento en cache %s\n",item->reference);
      	e = hashTableGet(table,item->reference,(cmpfn)entryComparar); //comprobar si existe en el futuro cercano
        if(e == NULL){
            int index = item->index;
            hashTableRemove(mem->table,item);//desalojar
            return index;
        }
    }
    for( i = currentIndex + size ; i > currentIndex  ; i--){
        //buscar si list[i] existe en la cache: 
        reqPage = hashTableGet(mem->table,references[i],(cmpfn)itemCmp); //si existe entonces es el item mas lejano de referenciar y debe desalojarse
        if(reqPage!=NULL){
            int index = reqPage->index;
            hashTableRemove(mem->table,reqPage);//desalojar
            //printf("indice out: %d\n",index);
            return index;
        }
    }
    return 0;
}


int main(int argc, char** argv) {
    
    int sizeCache;
    if (argc != 4) {
        printf("Usage: %s <POLITICA> <size-Cache> <filename>\n",argv[0]);
            return 1;
        }
    sizeCache = atoi(argv[2]);

    if (strcmp("LRU",argv[1]) == 0)
        testLRUAlgorithm(argv[3],sizeCache);

    else if (strcmp("LRUK",argv[1]) == 0)
        testLRUKAlgorithm(argv[3],sizeCache);
    
    else if (strcmp("CLOCK",argv[1]) == 0)
        testClockAlgorithm(argv[3],sizeCache);
    
    else if (strcmp("OPTIMO",argv[1])==0)
    	testOptimalAlgorithm(argv[3],sizeCache);
    return 0;
}

void testLRUAlgorithm(char *filePath, int sizeCache){
    FILE *fp;
    char linea[1340];
    strcpy(linea,"");
    
    if ((fp = fopen(filePath, "r")) == NULL){
        printf("error \n");
        return ;
    }

    MemoryCache*mem = memoryCacheNew(sizeCache);
    Queue*queue = queueNew();
    
    while (fgets(linea,1340, fp) != NULL){
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

void testClockAlgorithm(char *filePath, int sizeCache){
    FILE *fp;
    char linea[1340];
    strcpy(linea,"");
    
    if ((fp = fopen(filePath, "r")) == NULL){
        printf("error \n");
        return ;
    }
    
    MemoryCache*mem=memoryCacheNew(sizeCache);
    List *list = listNew();
    circleListMake(list);
    
    while (fgets(linea,1340, fp) != NULL){
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

void testLRUKAlgorithm(char *filePath, int sizeCache){
    FILE *fp;
    char linea[1340];
    strcpy(linea,"");
    
    if ((fp = fopen("workload.txt", "r")) == NULL){
        printf("error \n");
        return ;
    }

    MemoryCache*mem = memoryCacheNew(sizeCache);
    Queue*queue = queueNew();
    int i=0;
    while (fgets(linea,1340, fp) != NULL){
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
