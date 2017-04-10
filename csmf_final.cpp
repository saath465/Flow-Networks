/* This program is an implementation of the Capacity scaling Maximum flow algorithm*/

/* The source code inspiration is from ffLab.c from http://ranger.uta.edu/~weems/NOTES5311/ffLab.c */

/* program is written in c/c++ and uses the capacity scaling technique to find the best flow from a source node (0) to sink (n-1)
	The implementation idea of the capacity scaling algorithm has been adopted from the following websites links and notes:
			http://www.cs.princeton.edu/~wayne/cs423/lectures/max-flow-4up.pdf
			http://cseweb.ucsd.edu/classes/sp11/cse202-a/lecture8-final.pdf
			https://www.topcoder.com/community/data-science/data-science-tutorials/maximum-flow-augmenting-path-algorithms-comparison/
			https://courses.engr.illinois.edu/cs473/fa2010/Lectures/lecture17.pdf

	The program uses 3 modes to keep track of the edges between the source and the sink.
	Breath first Search (bfs) is used to find the adjacent nodes from the source to the sink.
	Adjacency list is used to store the information about the edges and the nodes. */

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>

using namespace std;

#define NOT_DISCOVERED 0			//indicates that the node has not been discovered
#define DISCOVERED 1				//Indicates when the node is discovered 
#define VISISTED 2					//Indicated when the node has been visited
#define inc 100000					// defines a value for the increment

int n;				//number of nodes in the network
int resedge;			//number of residual edges in the network
int delta = 0;				//capacity scale value
int max_capacity = 0;		//the maximum capacity in the graph/network

/* Structure to hold info about the edge*/

struct edge{
	int tail, head, capacity, flow, inverse;
};

typedef struct edge edgeType;

edgeType *edgeTab;

int *firstedge;

int *dis;		
int *pred;
int *pred_edge;

/* utility function to find the minimum*/

int min(int x, int y)
{
	return x<y ? x : y;
}

int head, tail;
int *q;

/* utility functions for queue operations*/

void enqueue(int x)
{
	q[tail] = x;
	tail++;
	dis[x] = DISCOVERED;
}

int dequeue()
{
	int x = q[head];
	head++;
	dis[x] = VISISTED;
	return x;
}

/* function for BFS implementation*/

int bfs(int source, int sink)
{
	int u, v, i;
	//int found_sink = 0;
	for(u=0;u<n;u++)
		dis[u] = NOT_DISCOVERED;				//set all the nodes initially to NOT_DISCOVERED
	head = tail = 0;	//re-initialize for each bfs search
	enqueue(source);
	pred[source] = -1;
	while(head != tail)
	{
		u = dequeue();
		if(u == sink)
		{
			 return 1;
		}
		
		for(i=firstedge[u]; i<firstedge[u+1]; i++)
		{
			v = edgeTab[i].head;
			if(dis[v] == NOT_DISCOVERED && edgeTab[i].capacity - edgeTab[i].flow>0 && edgeTab[i].capacity >=delta)
			{
				/* If the edge has not been discovered and the capacity is greater than delta*/
				enqueue(v);
				pred[v] = u;
				pred_edge[v] = i;
			}
		}
	}
	return 0;
}

/* function to find the maximum flow of the network*/

int max_flow(int source, int sink)
{
	int i,u;
	int max_flow;
	int count = 0;

	dis = (int*) malloc(n*sizeof(int));
	pred = (int*) malloc(n*sizeof(int));
	pred_edge = (int*) malloc(n*sizeof(int));
	q = (int*) malloc(n*sizeof(int));

	if(!dis || !pred || !pred_edge || !q)
	{
		cout<<"malloc failed"<<__LINE__<<endl;
	}

	// Initialize the flow, set flow to 0

	max_flow = 0;

	for(i=0; i<resedge; i++)
	{
		edgeTab[i].flow = 0;
	}

	while(bfs(source, sink))
	{
		int increment = inc;
		count++;
		//determine the increment
		for(u=sink; pred[u]!=(-1); u=pred[u])
		{
			i = pred_edge[u];
			cout<<i<<endl;
			int cap = edgeTab[i].capacity-edgeTab[i].flow;
			increment = min(increment,cap);
		}
		//increment the flow
		for(u=sink; pred[u]!=(-1); u = pred[u])
		{
			i = edgeTab[pred_edge[u]].inverse;
			edgeTab[pred_edge[u]].flow += increment;
			edgeTab[i].flow -=increment; //reverse the residual
		}

		for(u=sink; pred[u]!=(-1); u=pred[u])
		{
			cout<<u<<"<-";
			cout<<source<<"adds"<<increment<<"incremental flow";
		}
		max_flow +=increment;
	}
	cout<<endl;
	cout<<"Augmenting paths ::\t"<<count;

	free(dis);
	free(pred);
	free(pred_edge);
	free(q);
	return max_flow;
}

/* Utility function to sort the input edges*/

int tailfirst(const void* xin, const void* yin)
{
	int result;
	edgeType *x, *y;

	x = (edgeType*) xin;
	y = (edgeType*) yin;
	result = x->tail - y->tail;
	if(result!=0)
	{
		return result;
	}
	else
	{
		return x->head - y->head;
	}
}

/* function to read input from the terminal*/

void read_input()
{
	int tail, head, capacity, i, j;
	int input_edges;
	int working_edge;
	

	edgeType work;
	edgeType *ptr;

	cin>>n;
	cin>>input_edges;

	edgeTab = (edgeType*) malloc(2*input_edges*sizeof(edgeType));
	if(!edgeTab)
	{
		cout<<"invalid table allocation"<<endl;
		exit(0);
	}

	working_edge = 0;

	for(i=0; i<input_edges; i++)
	{
		cin>> tail;
		cin>> head;
		cin>> capacity;

		if(tail<0 || head<1 || tail > n-1 || head >=n || capacity<0)
		{
			cout<<"invalid entry"<<tail<<head<<capacity<<endl;
			exit(0);
		}

		//Insert the values into the table

		edgeTab[working_edge].tail = tail;
		edgeTab[working_edge].head = head;
		edgeTab[working_edge].capacity = capacity;
		
		working_edge++;

		edgeTab[working_edge].tail = head;
		edgeTab[working_edge].head = tail;
		edgeTab[working_edge].capacity = capacity;
		working_edge++;

		if(capacity>=max_capacity)
		{
			max_capacity = capacity;			//update max capacity to find the scaling parameter after each edge has been read in
		}
	}

	
	//sorting the edges
	qsort(edgeTab, working_edge, sizeof(edgeType), tailfirst);

	//check for parallel case

	resedge = 0;
	for(i=1; i<working_edge; i++)
	{
		if(edgeTab[resedge].tail == edgeTab[i].tail && edgeTab[resedge].head == edgeTab[i].head)
		{
			edgeTab[resedge].capacity += edgeTab[i].capacity;		// parallel case
		}
		else
		{
			resedge++;
			edgeTab[resedge].tail = edgeTab[i].tail;
			edgeTab[resedge].head = edgeTab[i].head;
			edgeTab[resedge].capacity = edgeTab[i].capacity;
		}
	}
	resedge++;

	//setting points for inverse

	for(i=0; i<resedge; i++)
	{
		work.tail = edgeTab[i].head;
		work.head = edgeTab[i].tail;

		ptr = (edgeType*) bsearch(&work,edgeTab,resedge,sizeof(edgeType),tailfirst);

		if(ptr == NULL)
		{
			cout<<"bsearch failed"<<endl;
			exit(0);
		}
		edgeTab[i].inverse = ptr - edgeTab;		//ptr arithmetic to get subscript
	}

	//find the first edge

	firstedge = (int*) malloc((n+1)*sizeof(int));
	if(!firstedge)
	{
		cout<<"malloc for firstedge setting failed"<<endl;
		exit(0);
	}

	j=0;
	for(i=0; i<n; i++)
	{
		firstedge[i] = j;
		for( ; j<resedge && edgeTab[j].tail == i; j++)
		{
			;
		}
	}
	firstedge[n] = resedge;		//end points
}

/* function to implement the capacity scaling algorithm*/

int capacity_scaling(int src, int sink)
{
	int m;
	//cout<<"max Capacity of the graph is::"<<max_capacity<<endl;
	double power = log((double)max_capacity)/log(2.0);			 	//find the value of the delta

	/* initially delta is equal to or larger than max_capaity*/

	delta = (int)pow(2, power);
	while(delta>=1)
	{
		cout<<"scaling parameter value::\t"<<delta<<endl;
		m = max_flow(src, sink);							//call the max flow function to find the maaximum flow in the network
		delta = delta/2;									//decrease delta by half
	}

	return m;
}

/* main function*/

int main()
{
	read_input();
	int max_flow = capacity_scaling(0,(n-1));
	cout<<"Max Flow for the network/graph is:\t"<<max_flow<<endl;
	free(edgeTab);
	free(firstedge);
}
