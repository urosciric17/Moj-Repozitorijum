#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#define PAGE_SIZE 4096
#define HEAP_SIZE 10*PAGE_SIZE
typedef unsigned char byte;
typedef unsigned int uint;
typedef struct chunk_t
{
uint size;
//struct chunk_t* prev;
byte* mem_ptr;
struct chunk_t* prev;
struct chunk_t* next;
bool free;
}chunk_t;

byte memory[HEAP_SIZE];

chunk_t* free_list=NULL;

chunk_t* create_chunk(void)
{
chunk_t* newChunk=sbrk(sizeof(chunk_t));
newChunk->size=0;
newChunk->prev=NULL;
newChunk->next=NULL;
newChunk->free=false;
return newChunk;
}

void coalesce(chunk_t* chunk)
{
if(chunk->next==NULL && chunk->prev==NULL)
return;
chunk_t *prev_ptr, *next_ptr;
chunk_t* prv=chunk->prev;
chunk_t* nxt=chunk->next;
chunk_t* tmp;
byte* mem_ptr=chunk->mem_ptr;
uint size=chunk->size;
while(nxt!=NULL)
{
if(nxt->free==true)
{
size=size+nxt->size;
if(nxt->mem_ptr<mem_ptr)
mem_ptr=nxt->mem_ptr;
tmp=nxt;
nxt=nxt->next;
tmp->next=NULL;
tmp->prev=NULL;
tmp->mem_ptr=NULL;
}
else break;
}
next_ptr=nxt;
while(prv!=NULL)
{
if(prv->free==true)
{
size=size+prv->size;
if(prv->mem_ptr<mem_ptr)
mem_ptr=prv->mem_ptr;
tmp=prv;
prv=prv->prev;
tmp->next=NULL;
tmp->prev=NULL;
tmp->mem_ptr=NULL;
}
else break;
prev_ptr=prv;
}
chunk->prev=prev_ptr;
chunk->next=next_ptr;
chunk->mem_ptr=mem_ptr;
chunk->size=size;
}

chunk_t* find_free_chunk(uint size)
{
if (free_list==NULL)
return NULL;
chunk_t* current=free_list;
while(current!=NULL)
{
if(current->size>=size && current->free==true)
{
uint diff=current->size-size;
current->size=size;
puts("found it :)");
if(diff!=0)
{
chunk_t* newChunk=sbrk(sizeof(chunk_t));
newChunk->size=diff;
newChunk->free=true;
newChunk->mem_ptr=current->mem_ptr+size;
chunk_t* tmp=current->next;
current->next=newChunk;
newChunk->next=tmp;
if(tmp!=NULL)
tmp->prev=newChunk;
newChunk->prev=current;
}
return current;
}
current=current->next;
}
return NULL;
}

bool m_free(void* ptr)
{
if(ptr==NULL)
return false;
chunk_t* current=free_list;
while(current!=NULL)
{
if((byte*)ptr==current->mem_ptr && current->free!=true)
{
current->free=true;
coalesce(current);
return true;
}
current=current->next;
}
return false;
}

byte* m_alloc(uint size)
{
if(size<=0)
return NULL;
if(size%4!=0)
size=(size/4)*4+4; //ensures alignment of 4 bytes
static long remaining_size=HEAP_SIZE;
static uint offset=0;
chunk_t* free_chunk=find_free_chunk(size);
if(free_chunk==NULL)
{
remaining_size=remaining_size-size;
if(remaining_size>=0)
{
chunk_t* newChunk=create_chunk();
newChunk->mem_ptr=memory+offset;
offset+=size;
newChunk->size=size;
newChunk->free=false;
newChunk->next=free_list;
if(free_list!=NULL)
free_list->prev=newChunk;
free_list=newChunk;
return newChunk->mem_ptr;
}
else 
{
remaining_size=remaining_size+size;
return NULL;
}
}
else 
{
free_chunk->free=false;
return free_chunk->mem_ptr;
}
}
int main(void)
{
printf("address of memory is %lu\n",memory);
uint* ptr=(uint*)m_alloc(8*sizeof(uint));
printf("address of p1 (6 uints) is %lu\n",ptr);
uint* ptr2=(uint *)m_alloc(sizeof(uint));
printf("address of p2 (1 uint) is %lu\n",ptr2);
uint* ptr3=(uint *)m_alloc(2*sizeof(uint));
printf("address of p3 (2 uints) is %lu\n",ptr3);
if(m_free(ptr2))
puts("freed p2");
if(m_free(ptr3))
puts("freed p3");
uint* ptr4=(uint *)m_alloc(3*sizeof(uint));
printf("address of p4 (7 uints) (post free) is %lu\n",ptr4);
return 0;
}
