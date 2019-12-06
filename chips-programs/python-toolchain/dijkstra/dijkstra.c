#define ENABLE_DYNAMIC_MEMORY
#define MALLOC_SIZE 100
#include <stdlib.h>

#define NUM_NODES                          100
#define NONE                               9999

int input_1 = input("input_1");
int output_1 = output("output_1");

typedef struct
{
  int iDist;
  int iPrev;
} NODE;

typedef struct
{
  int iNode;
  int iDist;
  int iPrev;
  int* qNext; //previously struct _QITEM*
} QITEM;

int *qHead = NULL; //previously QITEM*

             
             
             
int AdjMatrix[NUM_NODES][NUM_NODES];

int g_qCount = 0;
NODE rgnNodes[NUM_NODES];
int ch;
int iPrev, iNode;
int i, iCost, iDist;


void print_path (NODE *rgnNodes, int chNode)
{
  if (rgnNodes[chNode].iPrev != NONE)
    {
      print_path(rgnNodes, rgnNodes[chNode].iPrev);
    }
  fputc(chNode, output_1);
//  printf (" %d", chNode);
//  fflush(stdout);
}


void enqueue (int iNode, int iDist, int iPrev)
{
  //QITEM *qNew = (QITEM *) malloc(sizeof(QITEM));
  int *qNewAlloc = malloc(16);
  QITEM* qNew = (QITEM*)qNewAlloc;
  int *qLast = qHead;
  QITEM*qLastN = (QITEM*)qHead;

  
  if (qNewAlloc != NULL) 
    {
      //fprintf(stderr, "Out of memory.\n");
      fputc(42, output_1);
      //stop();
    }
  qNew->iNode = iNode;
  qNew->iDist = iDist;
  qNew->iPrev = iPrev;
  qNew->qNext = NULL;
  
  if (qLast == NULL) 
    {
      qHead = qNewAlloc;
    }
  else
    {
      while (qLastN->qNext != NULL) qLastN = (QITEM*)qLastN->qNext;
      qLastN->qNext = qNewAlloc;
    }
  g_qCount++;
  //               ASSERT(g_qCount);
}


void dequeue (int *piNode, int *piDist, int *piPrev)
{
  int *qKill = qHead;
  QITEM* qHeadN = (QITEM*)qHead;
  
  if (qHead != NULL)
    {
      //                 ASSERT(g_qCount);
      *piNode = qHeadN->iNode;
      *piDist = qHeadN->iDist;
      *piPrev = qHeadN->iPrev;
      qHead = qHeadN->qNext;
      free(qKill);
      g_qCount--;
    }
}


int qcount ()
{
  return(g_qCount);
}

int dijkstra(int chStart, int chEnd) 
{
  
  for (ch = 0; ch < NUM_NODES; ch++)
    {
      rgnNodes[ch].iDist = NONE;
      rgnNodes[ch].iPrev = NONE;
    }

  if (chStart == chEnd) 
    {
      //printf("Shortest path is 0 in cost. Just stay where you are.\n");
      fputc(0, output_1);
    }
  else
    {
      rgnNodes[chStart].iDist = 0;
      rgnNodes[chStart].iPrev = NONE;
      
      enqueue (chStart, 0, NONE);
      
     while (qcount() > 0)
	{
	  dequeue (&iNode, &iDist, &iPrev);
	  for (i = 0; i < NUM_NODES; i++)
	    {
	      if ((iCost = AdjMatrix[iNode][i]) != NONE)
		{
		  if ((NONE == rgnNodes[i].iDist) || 
		      (rgnNodes[i].iDist > (iCost + iDist)))
		    {
		      rgnNodes[i].iDist = iDist + iCost;
		      rgnNodes[i].iPrev = iNode;
		      enqueue (i, iDist + iCost, iNode);
		    }
		}
	    }
	}
      
//      printf("Shortest path is %d in cost. ", rgnNodes[chEnd].iDist);
//      printf("Path is: ");
      fputc(rgnNodes[chEnd].iDist, output_1);
      print_path(rgnNodes, chEnd);
//      printf("\n");
    }

  return 0;
}

void main() {
  int i,j,k;

report(1);
  //_malloc_init();
  _memory[0] = LAST_BLOCK;
  _memory[0] |= WORD_SIZE(MALLOC_SIZE);
report(2);
  
  /* make a fully connected matrix */
  for (i=0;i<NUM_NODES;i++) {
    for (j=0;j<NUM_NODES;j++) {
      /* make it more sparce */
      AdjMatrix[i][j]= fgetc(input_1);
    }
  }
  

  /* finds 10 shortest paths between nodes */
  for (i=0,j=NUM_NODES/2;i<3;i++,j++) {
			j=j%NUM_NODES;
      dijkstra(i,j);
  }

  return;
}
