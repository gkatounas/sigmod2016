#ifndef SCC_H
#define SCC_H

#include "grail.h"
#include "index.h"
#include "graph.h"

uint32_t sccComponents;

typedef struct Component{
uint32_t included_nodes_count;						// Number of nodes in component
uint32_t included_nodes_size;							// Size of array
uint32_t* included_node_ids;							// Ids of included nodes
} Component;

typedef struct SCC{
	Component* components;								// Components index - a vector which stores the components information
	uint32_t components_count;
	uint32_t components_size;
	uint32_t *id_belongs_to_component;				// Inverted index
} SCC;

typedef struct StaticData {							// Struct for static graphs 
	Graph* mainGraph;										// Main graph of nodes
	GrailIndex *grailIndex[NUM_OF_GRAILS];			// Grails of graph
	SCC *scc;												// Strongly Connencted Components
} StaticData; 

typedef struct ComponentCursor{
	Component* component_ptr;							// Pointer to current’s iteration component
} ComponentCursor;

SCC* SCC_Init(uint32_t nodes);						// Initializing SCC struct
SCC* SCC_Insert(SCC *scc, uint32_t comp, uint32_t idNode);// Inserting a node in a component in the stuct with all the components
void SCC_Print(SCC *scc);								// Print all components
SCC* estimateStronglyConnectedComponents(StaticData* sData, ptr startNode, ptr nodes);//Tarjan algorithm to create a struct with all the components
uint32_t findNodeStronglyConnectedComponentID(SCC* components, uint32_t nodeId);		// Find the component nodeIDd belongs
int iterateStronglyConnectedComponentID(SCC* components, ComponentCursor* cursor);
int next_StronglyConnectedComponentID(SCC* components, ComponentCursor* cursor);
int estimateShortestPathStronglyConnectedComponents(SCC* scc,NodeIndex *index1,NodeIndex *index2,ListNode *buf1,ListNode *buf2,uint32_t query,uint32_t id1,uint32_t id2,GrailIndex *index,uint32_t thread_id);			// Find the shortest path between id1,id2 using a grail index
void findStronglyConnectedComponentsNeighbors(SCC *scc, NodeIndex *index, ListNode *buf, BufferInfo *info);		// Create hypergraph
int destroyStronglyConnectedComponents(SCC* components);		// Free dynamic data

#endif
