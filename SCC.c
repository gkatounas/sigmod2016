#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "stack.h"
#include "SCC.h"
#include "bbfs.h"

extern uint32_t sccComponents,q;
extern NodeIndex *sccIndex;
extern BufferInfo *sccInfo;
extern ListNode *sccBuf;
extern uint32_t *visited;

SCC* estimateStronglyConnectedComponents(StaticData* sData, ptr startNode, ptr nodes){
	uint32_t index=1, i, component=0, *visited,*bt, tempPtr, id,temp,j=0,c=1,n=0,un=0;
	Node *node;
	Stack *dfs, *backtrack;
	SCC* scc=SCC_Init(nodes);

	visited=malloc(sizeof(uint32_t)*nodes);//array with value -1 if a node hasn't been visited else 1
	bt=malloc(sizeof(uint32_t)*nodes);//array with value -1 if a node is not in the backtrack stack else 1
	node=malloc(sizeof(Node)*nodes);//array nodes
		
	dfs=initStack(dfs, 20);//initializing stacks
	backtrack=initStack(backtrack,20);
	for (i=0; i<nodes; i++){//initializing lowlinks with the number of nods + 1 with is a value bigger than the biggest value lowlink can have
		node[i].lowlink=nodes+1;
	}
	memset(visited, -1, nodes*sizeof(uint32_t));//initializing arrays
	memset(bt, -1, nodes*sizeof(uint32_t));
	  
	dfs=pushStack(dfs, startNode);
	id=topStack(dfs);
	while (id!=-1){//main loop that takes every node and placing it in the component where it belongs
		if (visited[id]==-1){//if it is the first time we see a node
			n++;
			node[id].index=index;
			node[id].lowlink=index;
			visited[id]=1;
			index++;
			tempPtr=sData->mainGraph->indexOut[id].head;
			while(tempPtr<sData->mainGraph->indexOut[id].tail) {//put all of his neighbors in the dfs stack
				for(i=0; i<NEIGHBORS; i++) {
					if(visited[sData->mainGraph->bufOut[tempPtr].neighbor[i]]==-1) {
						dfs=pushStack(dfs, sData->mainGraph->bufOut[tempPtr].neighbor[i]);
				    	un++;
				    }
				}
				tempPtr=sData->mainGraph->bufOut[tempPtr].nextListNode;
				n++;
			}
			for(i=0; i<sData->mainGraph->indexOut[id].pos; i++) {
				if(visited[sData->mainGraph->bufOut[tempPtr].neighbor[i]]==-1) {
					dfs=pushStack(dfs, sData->mainGraph->bufOut[tempPtr].neighbor[i]);
					un++;
				}
				n++;
			}
			n=0;
			un=0;
		}
		else if(bt[id]==1) {//if the node is already in bt ignore it and remove it from dfs stack
			dfs=popStack(dfs);
		}
		else {//if it is not the first time we see a node and it is not in the backtrack stack
			dfs = popStack(dfs);//remove node from dfs stack
			tempPtr = sData->mainGraph->indexOut[id].head;
			while(tempPtr<sData->mainGraph->indexOut[id].tail) {//compare current node's lowlink with the lowest lowlink of its children and set current node's lowlink equal to the lowest of this values
				for(i=0; i<NEIGHBORS; i++) {
				    if(node[id].lowlink>node[sData->mainGraph->bufOut[tempPtr].neighbor[i]].lowlink) {
				    	node[id].lowlink=node[sData->mainGraph->bufOut[tempPtr].neighbor[i]].lowlink;
				    }
				}
				tempPtr=sData->mainGraph->bufOut[tempPtr].nextListNode;
			}
			for(i=0; i<sData->mainGraph->indexOut[id].pos; i++) {
				if(node[id].lowlink>node[sData->mainGraph->bufOut[tempPtr].neighbor[i]].lowlink){
					node[id].lowlink=node[sData->mainGraph->bufOut[tempPtr].neighbor[i]].lowlink;
				}
			}
			if(node[id].index==node[id].lowlink) {//if node's index and lowlink are equal this node is a root node
				bt[id]=1;
				scc=SCC_Insert(scc, component, id);//insert this node in a new component
				temp=topStack(backtrack);
				if(temp!=-1) {//all nodes
					while(node[temp].lowlink>=node[id].lowlink) {//insert all nodes with lowlink greater or equal in the new component
						scc=SCC_Insert(scc, component, temp);
						c++;
						backtrack=popStack(backtrack);
						node[temp].lowlink=nodes+1;//set lowlink at the highest value so it is not used later in other comparisons
						temp=topStack(backtrack);
						if(temp==-1) break;
					}
				}
				if(c!=1) {
					c=1;
				}
				scc->components_count++;
				component++;
				node[id].lowlink=nodes+1;//set lowlink at the highest value so it is not used later in other comparisons
			}
			else {// if it is not a root node just put it in the backtrack stack
				bt[id]=1;
				backtrack=pushStack(backtrack, id);
			}
		}
		i=0;
		if(isEmpty(dfs)) {// if dfs is empty we have finished with a component and we need to start looking if there are other components left
			for(i=j; i<nodes; i++) {
				if(visited[i]==-1) {
					j=i;
					dfs=pushStack(dfs, i);
					break;
				}
			}
		}
		if(i==nodes)	break;//if we have visited all nodes then stop searching for more components
		id=topStack(dfs);
	}
	sccComponents=scc->components_count;
	freeStack(dfs);//free dynamic data
	freeStack(backtrack);
	free(visited);
	free(bt);
	free(node);
	return scc;
}

SCC* SCC_Init(uint32_t nodes) {//init of scc struct
	SCC *scc=malloc(sizeof(SCC));
	int i;
	
	scc->components=malloc(10*sizeof(Component));
	scc->components_count=0;
	scc->components_size=10;
	scc->id_belongs_to_component=malloc(sizeof(uint32_t)*nodes);
	for (i=0; i<10; i++){
		scc->components[i].included_nodes_count=0;
		scc->components[i].included_nodes_size=1;
		scc->components[i].included_node_ids=malloc(sizeof(uint32_t));
	}

	return scc;
}

SCC* SCC_Insert(SCC *scc, uint32_t comp, uint32_t idNode) {// insert a new node in the component
	uint32_t i;

	if(scc->components_count==scc->components_size) {//if the struct with the components is full realloc so there is space for the new component
		scc->components=(Component*) realloc(scc->components, (2*scc->components_size)*(sizeof(Component)));
		for(i=scc->components_size; i<(2*scc->components_size); i++) {
			scc->components[i].included_nodes_count=0;
			scc->components[i].included_nodes_size=1;
			scc->components[i].included_node_ids=malloc(sizeof(uint32_t));
		}
		scc->components_size*=2;
	}
	if(scc->components[comp].included_nodes_count==scc->components[comp].included_nodes_size) {// if the component is full realloc so there is space for the new node
		scc->components[comp].included_node_ids=(uint32_t*) realloc(scc->components[comp].included_node_ids,(2*scc->components[comp].included_nodes_size)*sizeof(uint32_t));
		scc->components[comp].included_nodes_size*=2;
	}
	scc->components[comp].included_node_ids[scc->components[comp].included_nodes_count++]=idNode;
	
	scc->id_belongs_to_component[idNode]=comp;
	return scc;
}

void SCC_Print(SCC *scc) {
	uint32_t i;
	
	for(i=0; i<scc->components_count; i++){
		printf("Comp %d: nodes %d\n", i,scc->components[i].included_nodes_count);
	}
}

uint32_t findNodeStronglyConnectedComponentID(SCC* components, uint32_t nodeId) {//return the id of component node with id nodeId belongs
	return components->id_belongs_to_component[nodeId];
}

int iterateStronglyConnectedComponentID(SCC* components, ComponentCursor* cursor){
	int i=0;
	for(i;i<components->components_count;i++){
		printf("component:%d-->%d\n",i,(components->components[i]).included_nodes_count);
		cursor->component_ptr=&(components->components[i]);
	} 
	return 1;
}

int next_StronglyConnectedComponentID(SCC* components, ComponentCursor* cursor){
	uint32_t temp,temp1,tail,pos,counter=0,i,undefined=0;
	Stack* stoiva;
	
	stoiva=initStack(stoiva, components->components_count);

	visited=malloc((components->components_count)*sizeof(uint32_t));
	memset(visited, 0, (components->components_count)*sizeof(uint32_t));

	stoiva->counter=0;
	stoiva=pushStack(stoiva, 0);				// Push start node in stack
	visited[0]=1;									// Mark start node as visited
	while (counter!=components->components_count) {  // While we haven't visited all nodes
		while (!isEmpty(stoiva)) {             	// While we are in one component
			temp=topStack(stoiva);					// Get the top element from stack
			stoiva=popStack(stoiva);					// Decrease stack size
			printf("Compo: %d\n", temp);
			counter++;										// Increase counter of nodes
			temp1=sccIndex[temp].head;	
			tail=sccIndex[temp].tail;		
			pos=sccIndex[temp].pos;			
			while (temp1<tail) {									// For each listnode
				for (i=0; i<NEIGHBORS; i++) {  				// For each neighbour
					if (visited[sccBuf[temp1].neighbor[i]]!=1) {	// If we haven't visited him
						visited[sccBuf[temp1].neighbor[i]]=1;		// Mark him as visited
						stoiva=pushStack(stoiva,sccBuf[temp1].neighbor[i]);    // and push him in stack
					}
				}
				temp1=sccBuf[temp1].nextListNode;
			}
			for (i=0; i<pos; i++){								// Do the same for last listnode
				if (visited[sccBuf[temp1].neighbor[i]]!=1) {
					visited[sccBuf[temp1].neighbor[i]]=1;
					stoiva=pushStack(stoiva,sccBuf[temp1].neighbor[i]);    
				}
			}
		}
		for (i=undefined; i<components->components_count; i++) {  // Push the first unvisited node in stack to start a new component
			if (visited[i]!=1) {							// We use k to avoid initialization of visited array
				stoiva=pushStack(stoiva,i);
				visited[i]=1;
				undefined=i;
				break;
			}
    	}
	}
	return 0; 
}

int estimateShortestPathStronglyConnectedComponents(SCC* scc,NodeIndex *index1,NodeIndex *index2,ListNode *buf1,ListNode *buf2,uint32_t query,uint32_t id1,uint32_t id2, GrailIndex *index, uint32_t thread_id) {
	uint32_t node, position, i, j, source_comp, target_comp, comp1, comp2, level1=0, level2=0;						// Initialize the first level with zero
	int flag=0;
	if (id1==id2) return 0;					// Check if the nodes are the same, returning 0 at this case
	visitedOut[thread_id]=Id_Init(visitedOut[thread_id],index1,&outSize[thread_id],&outChildren[thread_id],id1,query);// Put the first values at the proper fields of the arrays and inits the variables
	
	visitedIn[thread_id]=Id_Init(visitedIn[thread_id],index2,&inSize[thread_id],&inChildren[thread_id],id2,query);
	id_nodesOut[thread_id]=Id_nodesInit(id_nodesOut[thread_id],id1);	// Add the nodes at the array that shows which nodes we have visited
	id_nodesIn[thread_id]=Id_nodesInit(id_nodesIn[thread_id],id2); 

	source_comp=findNodeStronglyConnectedComponentID(scc, id1);
	target_comp=findNodeStronglyConnectedComponentID(scc, id2);

	while(1) {
		if(outChildren[thread_id]<inChildren[thread_id]) {	// Start from the side with the less children
			outChildren[thread_id]=0;				// Initialize the counter of the children again
			while(id_nodesOut[thread_id]->first<id_nodesOut[thread_id]->last) {	// Add the children of all nodes at this level
				node=id_nodesOut[thread_id]->array[id_nodesOut[thread_id]->first++];// Take the first element from the queue
				position=index1[node].head;		// Start from the first listnode
				while(position != index1[node].tail) {	// Search at all listnodes until the last
					for(i=0; i<NEIGHBORS; i++) {	// Take all the neighbors
						if(buf1[position].neighbor[i]>=inSize[thread_id]) {
							visitedIn[thread_id]=Id_Increase(buf1[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);	// Check if we need to increase the
						}
						if(visitedIn[thread_id][buf1[position].neighbor[i]]==query) {	// size of the arrays with the visitd nodes and check the node
							return level1+level2+1;									// Return the path if we found the node
							}
						if(buf1[position].neighbor[i]>=outSize[thread_id]) {
							visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
						}
						if(visitedOut[thread_id][buf1[position].neighbor[i]]!=query) {	// Otherwise we continue the search to the listnode
							comp1=findNodeStronglyConnectedComponentID(scc, buf1[position].neighbor[i]);
							if(isReachableGrailIndex(index, comp1,target_comp)!=NO) {
								flag=1;
								visitedOut[thread_id][buf1[position].neighbor[i]]=query;	// Giving the right level
								if(id_nodesOut[thread_id]->counter>=id_nodesOut[thread_id]->size) {	// Check if suits at visited array
									id_nodesOut[thread_id]=Id_nodesIncrease(id_nodesOut[thread_id]);		// renewing the size
								}
								id_nodesOut[thread_id]->array[id_nodesOut[thread_id]->counter++]=buf1[position].neighbor[i];	// Add it there
								if(index1[buf1[position].neighbor[i]].head != -1)// Add the number of it's children if it has
									outChildren[thread_id]+=index1[buf1[position].neighbor[i]].tail*NEIGHBORS+index1[buf1[position].neighbor[i]].pos;
							}
						}
					}
					position=buf1[position].nextListNode;	// Continue the search with the next listnode
				}
				for(i=0; i<index1[node].pos; i++) {	// Search at the last listnode until the position that is not empty
					if(buf1[position].neighbor[i]>=inSize[thread_id])
						visitedIn[thread_id]=Id_Increase(buf1[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);	// doing exactly the same process
					if(visitedIn[thread_id][buf1[position].neighbor[i]]==query)
						return level1+level2+1;
					if(buf1[position].neighbor[i]>=outSize[thread_id])
						visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
					if(visitedOut[thread_id][buf1[position].neighbor[i]]!=query) {
						comp1=findNodeStronglyConnectedComponentID(scc, buf1[position].neighbor[i]);
						if(isReachableGrailIndex(index, comp1,target_comp)!=NO) {
							flag=1;
							visitedOut[thread_id][buf1[position].neighbor[i]]=query;
							if(id_nodesOut[thread_id]->counter>=id_nodesOut[thread_id]->size) {
								id_nodesOut[thread_id]=Id_nodesIncrease(id_nodesOut[thread_id]);
							}
							id_nodesOut[thread_id]->array[id_nodesOut[thread_id]->counter++]=buf1[position].neighbor[i];
							if(index1[buf1[position].neighbor[i]].head != -1)
								outChildren[thread_id]+=index1[buf1[position].neighbor[i]].tail*NEIGHBORS+index1[buf1[position].neighbor[i]].pos;
						}
					}
				}
				if(flag==0)	return -1;
				else	flag=1;
			}
			if(id_nodesOut[thread_id]->counter==id_nodesOut[thread_id]->first) return -1;
			id_nodesOut[thread_id]->last=id_nodesOut[thread_id]->counter;
			level1++;							// Increase the level of graph
		}
		else{									// Exactly the same with above for the other side of the graph
			inChildren[thread_id]=0;
			while(id_nodesIn[thread_id]->first<id_nodesIn[thread_id]->last) {
				node=id_nodesIn[thread_id]->array[id_nodesIn[thread_id]->first++];
				position=index2[node].head;
				while(position != index2[node].tail){
					for(i=0; i<NEIGHBORS; i++){
						if(buf2[position].neighbor[i]>=outSize[thread_id])
							visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
						if(visitedOut[thread_id][buf2[position].neighbor[i]]==query)
							return level1+level2+1;
						if(buf2[position].neighbor[i]>=inSize[thread_id])
							visitedIn[thread_id]=Id_Increase(buf2[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);
						if(visitedIn[thread_id][buf2[position].neighbor[i]]!=query) {
							comp2=findNodeStronglyConnectedComponentID(scc, buf2[position].neighbor[i]);
							if(isReachableGrailIndex(index, source_comp, comp2)!=NO) {
								flag=1;
								visitedIn[thread_id][buf2[position].neighbor[i]]=query;
								if(id_nodesIn[thread_id]->counter>=id_nodesIn[thread_id]->size){
									id_nodesIn[thread_id]=Id_nodesIncrease(id_nodesIn[thread_id]);
								}
								id_nodesIn[thread_id]->array[id_nodesIn[thread_id]->counter++]=buf2[position].neighbor[i];
								if(index2[buf2[position].neighbor[i]].head != -1)
									inChildren[thread_id]+=index2[buf2[position].neighbor[i]].tail*NEIGHBORS+index2[buf2[position].neighbor[i]].pos;
							}
						}
					}
					position=buf2[position].nextListNode;
				}
				for(i=0; i<index2[node].pos; i++) {
					if(buf2[position].neighbor[i]>=outSize[thread_id]){
						visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
					}
					if(visitedOut[thread_id][buf2[position].neighbor[i]]==query)
						return level1+level2+1;
					if(buf2[position].neighbor[i]>=inSize[thread_id])
						visitedIn[thread_id]=Id_Increase(buf2[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);
					if(visitedIn[thread_id][buf2[position].neighbor[i]]!=query) {
						comp2=findNodeStronglyConnectedComponentID(scc, buf2[position].neighbor[i]);
						if(isReachableGrailIndex(index, source_comp, comp2)!=NO) {
							flag=1;
							visitedIn[thread_id][buf2[position].neighbor[i]]=query;
							if(id_nodesIn[thread_id]->counter>=id_nodesIn[thread_id]->size){
								id_nodesIn[thread_id]=Id_nodesIncrease(id_nodesIn[thread_id]);
							}
							id_nodesIn[thread_id]->array[id_nodesIn[thread_id]->counter++]=buf2[position].neighbor[i];
							if(index2[buf2[position].neighbor[i]].head != -1)
								inChildren[thread_id]+=index2[buf2[position].neighbor[i]].tail*NEIGHBORS+index2[buf2[position].neighbor[i]].pos;
						}
					}
				}
				if(flag==0)	return -1;
				else	flag=1;
			}
			if(id_nodesIn[thread_id]->counter==id_nodesIn[thread_id]->first) return -1;
			id_nodesIn[thread_id]->last=id_nodesIn[thread_id]->counter;
			level2++;
		}
	}
}

void findStronglyConnectedComponentsNeighbors(SCC *scc, NodeIndex *index, ListNode *buf, BufferInfo *info) {//hypergraph creation
	uint32_t i, j, lis, n, comp;
	
	for(i=0; i<sccComponents; i++) {//for every component search every node and for every node search all of its neighbor and if a neighbor is in a defferent component connect the two components
		for(j=0; j<scc->components[i].included_nodes_count; j++) {
			if((lis=index[scc->components[i].included_node_ids[j]].head)!=-1) {
				sccIndex=insertNodeIndex(sccIndex, i, &sccComponents, &sccInfo->available);
				while(lis!=index[scc->components[i].included_node_ids[j]].tail) {
					for(n=0; n<NEIGHBORS; n++) {
						comp=scc->id_belongs_to_component[buf[lis].neighbor[n]];
						if(!neighbors(sccBuf,sccIndex[i].pos,sccIndex[i].head,sccIndex[i].tail,i,comp)) {
							sccBuf=allocNewNode(sccBuf, sccInfo, &sccIndex[i].pos, &sccIndex[i].tail, i, comp, 0);
						}
					}
					lis=buf[lis].nextListNode;
				}
				for(n=0; n<index[scc->components[i].included_node_ids[j]].pos; n++) {
					comp=scc->id_belongs_to_component[buf[lis].neighbor[n]];
					if(!neighbors(sccBuf,sccIndex[i].pos,sccIndex[i].head,sccIndex[i].tail,i,comp)) {
						sccBuf=allocNewNode(sccBuf, sccInfo, &sccIndex[i].pos, &sccIndex[i].tail, i, comp, 0);
					}
				}
			}
		}
	}
}

int destroyStronglyConnectedComponents(SCC *scc){//free dynamic data
	uint32_t i;
	
	for(i=0; i<scc->components_size; i++) {
		free(scc->components[i].included_node_ids);
	}
	free(scc->components);
	free(scc->id_belongs_to_component);
	free(scc);

	return 0;
}
