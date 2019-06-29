#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define DELETE			4

// User structure type definition
typedef struct
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
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
tTOKEN *createToken(char *str);

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
		pList->pos = pList->rear;

		if (pList->count == 1) {
			destroyToken(pList->pos->dataPtr);
			free(pList->pos);
			pList->rear = NULL;
			pList->head = NULL;
			pList->count = 0;

			break;
		}
		else if (pList->count > 1) {
			destroyToken(pList->pos->dataPtr);
			pList->pos = pList->pos->llink;
			free(pList->rear);
			pList->rear = pList->pos;
			pList->pos->rlink = NULL;
			pList->count--;
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
	if (pList->count == 0) {
		if (_insert(pList, pList->head, dataInPtr))
			return 0;
		else
			return -1;
	}

	pList->pos = pList->head;

	int find = _search(pList, &pList->pos->llink, &pList->pos, dataInPtr->token);

	if (find) {
		pList->pos->dataPtr->freq++;
		return 1;
	}
	else {
		if (strcmp(pList->head->dataPtr->token, dataInPtr->token) == 0)
			if (_insert(pList, pList->head, dataInPtr))
				return 0;
			else
				return -1;

		pList->pos = pList->head;

		while (pList->pos->rlink != NULL) {
			if (strcmp(pList->pos->rlink->dataPtr->token, dataInPtr->token) < 0)
				pList->pos = pList->pos->rlink;
			else
				break;
		}

		if (_insert(pList, pList->pos, dataInPtr))
			return 0;
		else
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

	int find = _search(pList, &pList->pos->llink, &pList->pos, keyPtr);

	if (find) {
		if (strcmp(pList->head->dataPtr->token, keyPtr) == 0)  // head change
			_delete(pList, pList->pos, pList->pos, dataOut);
		else
			_delete(pList, pList->pos->llink, pList->pos, dataOut);

		return 1;
	}

	return 0;
}

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList(LIST *pList, char *pArgu, tTOKEN **pDataOut) {
	if (pList->count == 0)
		return 0;

	if (pList->count == 1)
		if (strcmp(pList->head->dataPtr->token, pArgu) == 0) {
			pDataOut = &pList->head->dataPtr;
			return 1;
		}

	int find = _search(pList, &pList->pos->llink, &pList->pos, pArgu);

	if (find) {
		pDataOut = &pList->pos->dataPtr;
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

/* prints data from list (forward)
*/
void printList(LIST *pList) {
	pList->pos = pList->head;

	while (pList->pos != NULL) {
		printf("%s	%d\n", pList->pos->dataPtr->token, pList->pos->dataPtr->freq);
		pList->pos = pList->pos->rlink;
	}
}

/* prints data from list (backward)
*/
void printListR(LIST *pList) {
	pList->pos = pList->rear;

	while (pList->pos != NULL) {
		printf("%s	%d\n", pList->pos->dataPtr->token, pList->pos->dataPtr->freq);
		pList->pos = pList->pos->llink;
	}
}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert(LIST *pList, NODE *pPre, tTOKEN *dataInPtr) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode)
		return 0;

	newNode->dataPtr = dataInPtr;

	if (pList->count == 0) {
		newNode->llink = NULL;
		newNode->rlink = NULL;
		pList->head = newNode;
		pList->rear = newNode;
	}
	else {
		if (strcmp(pList->head->dataPtr->token, dataInPtr->token) > 0) {
			newNode->rlink = pList->head;
			newNode->rlink->llink = newNode;
			newNode->llink = NULL;
		}
		else {
			newNode->rlink = pPre->rlink;
			pPre->rlink = newNode;
			if (newNode->rlink != NULL)
				newNode->rlink->llink = newNode;
			newNode->llink = pPre;
		}
	}
	if (newNode->llink == NULL)
		pList->head = newNode;
	if (newNode->rlink == NULL)
		pList->rear = newNode;

	pList->count++;
	return 1;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr) {

	if (pPre == pLoc) { //remove head
		*dataOutPtr = pLoc->dataPtr;

		if (pList->count == 1) {
			free(pLoc);
			pList->head = NULL;
			pList->rear = NULL;
		}
		else {
			pList->head = pLoc->rlink;
			free(pLoc);
			pList->head->llink = NULL;
		}
	}
	else {

		*dataOutPtr = pLoc->dataPtr;
		pPre->rlink = pLoc->rlink;

		free(pLoc);

		if (pPre->rlink == NULL)
			pList->rear = pPre;
		else
			pPre->rlink->llink = pPre;
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
			return 1;
		}
		else
			return 0;
	}

	pList->pos = pList->head;

	while (pList->pos->rlink != NULL) {
		if (strcmp(pList->pos->dataPtr->token, pArgu) < 0) {
			if (strcmp(pList->pos->rlink->dataPtr->token, pArgu) > 0) {
				return 0;
			}
			else if (strcmp(pList->pos->rlink->dataPtr->token, pArgu) == 0) {
				pList->pos = pList->pos->rlink;
				
				pPre = &pList->pos->llink;
				pLoc = &pList->pos;
				return 1;
			}

			pList->pos = pList->pos->rlink;
		}
		else if (strcmp(pList->pos->dataPtr->token, pArgu) == 0) // head change
			return 1;
		else
			pList->pos = pList->pos->rlink;
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

	if (!newToken->token){
		free(newToken);
		return NULL;	
	}

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

/* gets user's input
*/
int get_action()
{
	char ch;
	scanf("%c", &ch);
	ch = toupper(ch);
	switch (ch)
	{
	case 'Q':
		return QUIT;
	case 'F':
		return FORWARD_PRINT;
	case 'B':
		return BACKWARD_PRINT;
	case 'D':
		return DELETE;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (!fp)
	{
		fprintf(stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}

	// creates a null list
	list = createList();
	if (!list) {
		printf("Cannot create list\n");
		return 100;
	}

	while (fscanf(fp, "%s", str) == 1)
	{
		pToken = createToken(str);

		// insert function call
		ret = addNode(list, pToken);

		if (ret == 1) // duplicated 
			destroyToken(pToken);
	}

	fclose(fp);

	fprintf(stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");

	while (1)
	{
		int action = get_action();

		switch (action)
		{
		case QUIT:
			destroyList(list);
			return 0;

		case FORWARD_PRINT:
			printList(list);
			break;

		case BACKWARD_PRINT:
			printListR(list);
			break;

		case DELETE:
			fprintf(stdout, "Input a string to delete: ");
			fscanf(stdin, "%s", str);

			int ret = removeNode(list, str, &pToken);

			if (ret) {
				fprintf(stdout, "%s deleted\n", pToken->token);
				destroyToken(pToken);
			}
			else
				fprintf(stdout, "%s not found\n", str);
			break;
		}

		if (action) fprintf(stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");
	}
	return 0;
}

