#include <stdio.h>
#include <stdlib.h> //malloc, free

typedef struct {
	int vertexCount;
	int** matrix;
}Graph;

Graph* makeGraph(int size);
void destroyGraph(Graph* graph);
void addEdge(Graph* graph, int ver1, int ver2);
void printDFS(Graph* graph);
void printBFS(Graph* graph);

int main(int argc, char **argv){
	int size;
	char str1[1024];
	char str2[1024];
	FILE* fp;

	fp = fopen(argv[1], "r");
	
	fscanf(fp, "%s", str1);
	fscanf(fp, "%d", &size);

	Graph* graph = makeGraph(size);
	if (!graph) {
		printf("Cannot create Graph\n");
		return 0;
	}

	fscanf(fp, "%s", str2);
	int ver1, ver2;
	while (fscanf(fp,"%d %d", &ver1, &ver2) == 2) {
		addEdge(graph, ver1, ver2);
	}
/*
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%d ", graph->matrix[i][j]);
		}
		printf("\n");
	}
*/

	fclose(fp);
//	printf("Vertex Count : %d\n", graph->vertexCount);

	printDFS(graph);
	printBFS(graph);

	destroyGraph(graph);
	return 0;
}

Graph* makeGraph(int size) {
	Graph* newGraph = (Graph*)malloc(sizeof(Graph));
	if (!newGraph)
		return NULL;

	newGraph->vertexCount = size;

	newGraph->matrix = (int**)malloc(sizeof(int *) * size);
	for (int i = 0; i < size; i++)
		newGraph->matrix[i] = (int*)malloc(sizeof(int) * size);

	return newGraph;
}
void destroyGraph(Graph* graph) {
	int size = graph->vertexCount;

	for (int i = 0; i < size; i++)
		free(graph->matrix[i]);

	free(graph->matrix);

	free(graph);
}
void addEdge(Graph* graph, int ver1, int ver2) {
	graph->matrix[ver1-1][ver2-1] = 1;
	graph->matrix[ver2-1][ver1-1] = 1;
}

int* _makeFlag(int size) {
	int* flag = (int*)malloc(sizeof(int) * size);

	for (int i = 0; i < size; i++)
		flag[i] = 0; // processed flag 값 초기화

	return flag;
}

void printDFS(Graph* graph) {
	printf("DFS :");

	int size = graph->vertexCount;
	int* flag = _makeFlag(graph->vertexCount);
	int* stack = (int*)malloc(sizeof(int) * size);
	int stackSize = 0;
	
	for(int i = 0; i < size; i++){
		if(flag[i])
			continue;

		stack[0] = i + 1;
		stackSize++;
		flag[i] = 1;

		while(stack[stackSize - 1] != 0){
			//popStack
			int popData = stack[stackSize - 1];

			stack[stackSize - 1];
			stackSize--;

			printf(" %d", popData);

			for(int j = 0 ; j < size; j++)
				if(!flag[j] && (graph->matrix[popData - 1][j]) == 1){
					stack[stackSize] = j + 1;
					stackSize++;
					flag[j] = 1;
				}
		}
	}

	free(flag);
	free(stack);
	printf("\n");
}

void printBFS(Graph* graph) {
	printf("BFS :");

	int size = graph->vertexCount;
	int* flag = _makeFlag(size);
	int* queue = (int*)malloc(sizeof(int) * size);
	int queueSize = 0;

	for(int i = 0; i<size; i++){
		if(flag[i])
			continue;

		queue[0] = i + 1;
		queueSize++;
		flag[i] = 1;

		while(queue[0] != 0){
			//deQueue
			int deQueue = queue[0];
			for(int j = 0; j < queueSize - 1; j++)
				queue[j] = queue[j + 1];

			queue[queueSize - 1] = 0;
			queueSize--;

			printf(" %d", deQueue);

			for(int j = 0 ; j < size; j++)
				if(!flag[j] && (graph->matrix[deQueue - 1][j]) == 1){
					queue[queueSize] = j + 1;
					queueSize++;
					flag[j] = 1;
				}
		}
	}

	free(queue);
	free(flag);
	printf("\n");
}