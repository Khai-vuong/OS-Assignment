#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#define FALSE 0;
int empty(struct queue_t *q)
{
        if (q == NULL)
                return 1;
        return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
        /* TODO: put a new process to queue [q] */
        if (q == NULL)
        {
                perror("Queue is NULL !\n");
                exit(1);
        }
        int size = q->size;
        if (size >= MAX_QUEUE_SIZE || size >= (MAX_PRIO - proc->prio))
        {
                perror("Queue is full !\n");
                exit(1);
        }
        else
        {
                q->proc[size] = proc;
                q->size++;
        }
}

struct pcb_t *dequeue(struct queue_t *q)
{
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        if (q == NULL)
        {
                perror("Queue is NULL !\n");
                exit(1);
        }
        int size = q->size;
        if (size<=0)
        {
                return NULL;
        }
        else
        {
                struct pcb_t *result = q->proc[0];
                for (int i = 0; i < size -1; i++)
                {
                        q->proc[i] = q->proc[i+1];
                }
                q->size--;
                return result;
        }
}   