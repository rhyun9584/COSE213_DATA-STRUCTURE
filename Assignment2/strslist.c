#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp

// User structure type definition
typedef struct
{
	char	*token;
	int		freq; //��
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*link;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

static int _insert(LIST *pList, NODE *pPre, tTOKEN *dataInPtr);
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);
tTOKEN *destroyToken(tTOKEN *pToken);

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(void) {
	LIST* list = (LIST*)malloc(sizeof(LIST));
	if (!list)
		return list;

	list->count = 0;
	list->pos = NULL;
	list->head = NULL;
	list->rear = NULL;

	return list;
}

/* Deletes all data in list and recycles memory
	return	NULL head pointer
*/
LIST *destroyList(LIST *pList) {

	while (1) {
		pList->pos = pList->head;

		for (int i = 1; i < pList->count - 1; i++) {
			pList->pos = pList->pos->link;           // move to preNode of lastNode
		}

		if (pList->count > 1) {
			destroyToken(pList->rear->dataPtr);
			free(pList->rear);
			pList->pos->link = NULL;
			pList->rear = pList->pos;
			pList->count--;
		}
		else if (pList->count == 1) {
			destroyToken(pList->rear->dataPtr);
			free(pList->rear);
			pList->rear = NULL;
			pList->head = NULL;
			pList->count = 0;

			break;                     //complete remove all Node
		}
	}

	free(pList);

	return NULL;
}

/* Inserts data into list
	return	-1 if overflow
			0 if successful
			1 if duplicated key
*/
int addNode(LIST *pList, tTOKEN *dataInPtr) {	

	if (pList->count == 0) {  //List is empty
		if (_insert(pList, pList->head, dataInPtr))
			return 0;
		else			//overflow
			return -1; 
	}

	pList->pos = pList->head;
	
	int find = _search(pList, &pList->pos, &pList->pos->link, dataInPtr->token);

	if (find) {		// duplicated key
		if (pList->count == 1)
			pList->pos->dataPtr->freq++;
		else
			pList->pos->link->dataPtr->freq++;

		return 1;
	}
	else {			// not duplicated 
		if (strcmp(pList->head->dataPtr->token, dataInPtr->token) > 0) {  //head must be changed
			if (_insert(pList, pList->head, dataInPtr))
				return 0;
			else			//overflow
				return -1;
		}

		pList->pos = pList->head;

		while (pList->pos->link != NULL) {
			if (strcmp(pList->pos->link->dataPtr->token, dataInPtr->token) < 0)
				pList->pos = pList->pos->link;
			else
				break;
		}

		if (_insert(pList, pList->pos, dataInPtr))
			return 0;
		else			//overflow
			return -1;
	}
}

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode(LIST *pList, char *keyPtr, tTOKEN **dataOut) {
	if (pList->count == 0)
		return 0;

	pList->pos = pList->head;

	int find = _search(pList, &pList->pos, &pList->pos->link, keyPtr);

	if (find) {
		if (strcmp(pList->head->dataPtr->token, keyPtr)) 
			_delete(pList, pList->pos, pList->pos, dataOut);
		
		else 
			_delete(pList, pList->pos, pList->pos->link, dataOut);
		
		
		return 1;
	}
	else 
		return 0;
}

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList(LIST *pList, char *pArgu, tTOKEN **pDataOut){
	if(pList->count == 0)
		return 0;
	
	if(pList->count == 1){
		if(strcmp(pList->head->dataPtr->token, pArgu)){
			pDataOut = &pList->head->dataPtr;
			return 1;
		}
	}
	
	int find = _search(pList, &pList->pos, &pList->pos->link, pArgu);
	
	if(find){
		pDataOut = &pList->pos->link->dataPtr;
		return 1;
	}
	else
		return 0;
}

/* returns number of nodes in list
*/
int listCount(LIST *pList) {
	return pList->count;
}

/* returns	1 empty
			0 list has data
*/
int emptyList(LIST *pList) {
	if (pList->count == 0)
		return 1;
	else
		return 0;
}

//int fullList( LIST *pList);

/* prints data from list
*/
void printList(LIST *pList) {
	pList->pos = pList->head;

	while (pList->pos != NULL) {
		printf("%s	%d\n", pList->pos->dataPtr->token, pList->pos->dataPtr->freq);
		pList->pos = pList->pos->link;
	}
}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert(LIST *pList, NODE *pPre, tTOKEN *dataInPtr) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode) //overflow
		return 0;

	newNode->dataPtr = dataInPtr;

	if (pList->count == 0) {
		pList->head = newNode;
		newNode->link = NULL;
	}
	else {
		if (strcmp(pList->head->dataPtr->token, dataInPtr->token) > 0) {
			newNode->link = pList->head;
			pList->head = newNode;
		}
		else {
			newNode->link = pPre->link;
			pPre->link = newNode;
		}
	}

	if (newNode->link == NULL)
		pList->rear = newNode;
	
	pList->count++;
	return 1;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr) {


	if (pPre == pLoc) {
		*dataOutPtr = pList->head->dataPtr;
		if (pList->count == 1) {
			free(pLoc);
			pList->head = NULL;
			pList->rear = NULL;
		}
		else {
			pList->head = pLoc->link;
			free(pLoc);
		}
	}
	else {
		*dataOutPtr = pLoc->dataPtr;
		pPre->link = pLoc->link;
		free(pLoc);
		
		if (pPre->link == NULL) {
			pList->rear = pPre;
		}
	}

	pList->count--;
}

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu) {
	if (pList->count == 0)
		return 0;
	
	if (pList->count == 1) { //
		if (strcmp(pList->head->dataPtr->token, pArgu) == 0) {
			//*pPre = pList->head;
			//*pLoc = pList->head;
			return 1;
		}
		else
			return 0;
	}

	pList->pos = pList->head;

	while (pList->pos->link != NULL) {
		if (strcmp(pList->pos->dataPtr->token, pArgu) < 0) {
			if (strcmp(pList->pos->link->dataPtr->token, pArgu) > 0) {
				return 0;
			}
			else if (strcmp(pList->pos->link->dataPtr->token, pArgu) == 0) {
				pPre = &pList->pos;
				pLoc = &pList->pos->link;
				return 1;
			}

			pList->pos = pList->pos->link;
		}
		else if (strcmp(pList->pos->dataPtr->token, pArgu) == 0)
			return 1;
		else
			pList->pos = pList->pos->link;
	}

	return 0;
}

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken(char *str) {
	tTOKEN* newToken = (tTOKEN*)malloc(sizeof(tTOKEN));

	if (!newToken)
		return NULL;

	newToken->freq = 1;
	newToken->token = strdup(str);
	
	if (!newToken->token)
		return NULL;
	
	return newToken;
}

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
tTOKEN *destroyToken(tTOKEN *pToken) {

	free(pToken->token);
	free(pToken);

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;

	// creates a null list
	list = createList();
	if (!list)
	{
		printf("Cannot create list\n");
		return 100;
	}

	while (scanf("%s", str) == 1)
	{
		pToken = createToken( str);

		// insert function call
		ret = addNode( list, pToken);

		if (ret == 1) // duplicated 
			destroyToken( pToken);

	}
	// print function call
	printList( list);

	destroyList( list);

	return 0;
}
