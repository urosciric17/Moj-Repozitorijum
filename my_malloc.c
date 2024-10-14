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
struct chunk_t* next;
bool free;
}chunk_t;

byte memory[HEAP_SIZE];

chunk_t* free_list=NULL;

chunk_t* create_chunk(void)
{
chunk_t* newChunk=sbrk(sizeof(chunk_t));
newChunk->size=0;
//newChunk->prev=NULL;
newChunk->next=NULL;
newChunk->free=false;
return newChunk;
}

/*void coalesce(void)
{
if(free_list==NULL)
return;
chunk_t* current=free_list;
while(current->next!=NULL)
{
if(current->free==true && current->next->free==true)
current=current->next;
}
}*/

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
chunk_t* newChunk=sbrk(sizeof(chunk_t));
newChunk->size=diff;
newChunk->free=true;
newChunk->mem_ptr=current->mem_ptr+size;
chunk_t* tmp=current->next;
current->next=newChunk;
newChunk->next=tmp;
return current;
}
current=current->next;
}
return NULL;
}

bool m_free(void* ptr)
{
chunk_t* current=free_list;
while(current!=NULL)
{
if((byte*)ptr==current->mem_ptr && current->free!=true)
{
current->free=true;
return true;
}
current=current->next;
}
//coalesce(head);
return false;
}

byte* m_alloc(uint size)
{
if(size<=0)
return NULL;
if(size%4!=0)
size=(size/4)*4+4; //ensures alignment of 4 bytes
static long remaining_size=HEAP_SIZE;
chunk_t* free_chunk=find_free_chunk(size);
if(free_chunk==NULL)
{
remaining_size=remaining_size-size;
if(remaining_size>=0)
{
chunk_t* newChunk=create_chunk();
newChunk->mem_ptr=memory+(HEAP_SIZE-remaining_size);
newChunk->size=size;
newChunk->free=false;
newChunk->next=free_list;
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
printf("size of uint is %lu\n",sizeof(uint));
printf("address of memory is %lu\n",memory);
uint* ptr=(uint *)m_alloc(6*sizeof(uint));
printf("address of p1 (6 uints) is %lu\n",ptr);
uint* ptr2=(uint *)m_alloc(sizeof(uint));
printf("address of p2 (1 uint) is %lu\n",ptr2);
uint* ptr3=(uint *)m_alloc(2*sizeof(uint));
printf("address of p3 (2 uints) is %lu\n",ptr3);
m_free(ptr);
uint* ptr4=(uint *)m_alloc(2*sizeof(uint));
printf("address of p4 (2 uint) (post free) is %lu\n",ptr4);
return 0;
}
