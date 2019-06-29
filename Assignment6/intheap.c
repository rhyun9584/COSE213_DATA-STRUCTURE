#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time
#include <assert.h>

#define MAX_ELEM	20

typedef struct
{
	int *heapArr;
	int	count;
	int	capacity;
} HEAP;

HEAP *heapCreate( int capacity);
void heapDestroy( HEAP *heap);
int heapInsert( HEAP *heap, int data);
static void _reheapUp( HEAP *heap, int index);
int heapDelete( HEAP *heap, int* data);
static void _reheapDown( HEAP *heap, int index);
void heapPrint( HEAP *heap);

int main(void)
{
	HEAP *heap;
	int data;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL));
	
	for (int i = 1; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		
		heapPrint( heap);
 	}

	while (heap->count > 0)
	{
		// delete function call
		heapDelete( heap, &data);

		fprintf( stdout, "Deleting %d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}

HEAP *heapCreate(int capacity) {
	HEAP *heap = (HEAP*)malloc(sizeof(HEAP));
	if (!heap)
		return NULL;

	int* heapArr = (int*)malloc(sizeof(int) * capacity);

	heap->capacity = capacity;
	heap->heapArr = heapArr;
	heap->count = 0;

}
void heapDestroy(HEAP *heap) {
	free(heap->heapArr);
	free(heap);
}
//i번째의 부모는 (i-1)/2
static void _reheapUp(HEAP *heap, int index) { // 위치조정
		if (index > 0) {
			int data = heap->heapArr[index];
			int parentData = heap->heapArr[(index - 1) / 2];

			if (data > parentData) {
				heap->heapArr[(index - 1) / 2] = data;
				heap->heapArr[index] = parentData;

				_reheapUp(heap, (index - 1) / 2);
			}
		}
}
int heapInsert(HEAP *heap, int data) { //마지막 위치에 넣어주고 reheapUp으로 
	heap->heapArr[heap->count] = data;
	_reheapUp(heap, heap->count);

	heap->count++;
}
//i번째의 자식은 2i+1, 2i+2
static void _reheapDown(HEAP *heap, int index) {
	int leftIndex = index * 2 + 1;
	int rightIndex = index * 2 + 2;

	int leftData = 0;
	int rightData = 0;

	if (index * 2 + 1 < heap->count) 
		leftData = heap->heapArr[leftIndex];
	if (index * 2 + 2 < heap->count) 
		rightData = heap->heapArr[rightIndex];

	if (leftData >= rightData && heap->heapArr[index] < leftData) {
		heap->heapArr[leftIndex] = heap->heapArr[index];
		heap->heapArr[index] = leftData;
		_reheapDown(heap, leftIndex);
	}
	else if (leftData < rightData && heap->heapArr[index] < rightData) {
		heap->heapArr[rightIndex] = heap->heapArr[index];
		heap->heapArr[index] = rightData;
		_reheapDown(heap, rightIndex);
	}
	
}

int heapDelete(HEAP *heap, int* data) {
	*data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->count - 1];
	heap->heapArr[heap->count - 1] = 0;
	_reheapDown(heap, 0);

	heap->count--;
}

void heapPrint(HEAP *heap) {
	for (int i = 0; heap->heapArr[i] != 0; i++)
		printf("	%d", heap->heapArr[i]);
	printf("\n");
}