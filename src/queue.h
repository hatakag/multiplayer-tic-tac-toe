#include "server.h"
#define MaxQ 2

typedef ClientNode* EltypeQ;
typedef struct nodeQ nodeQ;
struct nodeQ {
  EltypeQ elQ;
  nodeQ *nextQ;
  nodeQ *prevQ;
};
typedef struct {
  nodeQ *front,*rear;
}Queue;

void makeNullQueue(Queue *q) {
  q->front=NULL;
  q->rear=NULL;
}

int emptyQueue(Queue q) {
  return (q.front==NULL && q.rear==NULL);
}

int fullQueue(Queue q) {
  nodeQ *c=q.front;
  int i=0;
  while (c!=NULL) {
    c=c->nextQ;
    ++i;
  }
  return i==MaxQ;
}

void enQueue(EltypeQ x,Queue *q) {
  if (fullQueue(*q)) printf("Queue is full!\n");
  else {
    nodeQ *new=(nodeQ*)malloc(sizeof(nodeQ));
    new->elQ=x;
    if (emptyQueue(*q)) {
      new->nextQ=NULL;new->prevQ=NULL;
      q->front=new;q->rear=new;
    } else {
      q->rear->nextQ=new;
      new->prevQ=q->rear;
      new->nextQ=NULL;
      q->rear=new;
    }
  }
}

EltypeQ deQueue(Queue *q) {
  if (!emptyQueue(*q)) {
    nodeQ* t;
    EltypeQ x;
    t=q->front;x=t->elQ;
    if (q->front==q->rear) {
      q->front=NULL;q->rear=NULL;
    } else {
      q->front=q->front->nextQ;
      q->front->prevQ=NULL;
    }
    free(t);
    return x;
  }
  else printf("Queue is empty!\n");
}
/*
void printQueue(Queue q) {
  nodeQ *c=q.front;
  while (c!=NULL) {
    printf("%d",c->elQ);
    c=c->nextQ;
  }
}
*/
