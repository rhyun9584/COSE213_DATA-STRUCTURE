#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <string.h> // strlen

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *createTree(void) {
	TREE* tree = (TREE*)malloc(sizeof(TREE));
	if (!tree)
		return NULL;

	tree->root = NULL;

	return tree;
}

static void _destroy(NODE *root) { //postorder LRN
	if (!root)
		return;

	_destroy(root->left);
	_destroy(root->right);
	free(root);
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *destroyTree(TREE *pTree) {
	if (pTree)
		_destroy(pTree->root);

	free(pTree);
	return NULL;
}

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE *_makeNode(char ch) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode)
		return NULL;

	newNode->data = ch;

	return newNode;
}

/*  Discriminate expression is postfix
	return  1 is postfix
			0 does not
*/
int isPostfix(char* expr) {
	int stackCount = 0;
	int operatorsNum = 0;
	int operandsNum = 0;
	if (strlen(expr) == 1 && isdigit(expr[0]) != 0)
		return 1;

	for (int i = 0; i < strlen(expr); i++) {
		if (isdigit(expr[i]) == 0) { //operators
			stackCount--;
			operatorsNum++;
			if (operatorsNum >= operandsNum)
				return 0;
		}
		else {
			stackCount++;
			operandsNum++;
		}
	}

	if (stackCount == 1 && isdigit(expr[0]) && isdigit(expr[1]) && !isdigit(expr[strlen(expr) - 1]))
		return 1;
	else
		return 0;
}

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree(char *expr, TREE *pTree) { // right to left predorder

	NODE* stackArr[MAX_STACK_SIZE];
	int stackCount = 0;
	if (isPostfix(expr) == 0)
		return 0;


	for (int i = 0; i < strlen(expr); i++) {

		if (isdigit(expr[i]) == 0) { //expr[i] is operators

			//"pop stack node1 node2">> node1이 먼저 Pop 된것
			NODE* node1 = stackArr[stackCount - 1];
			NODE* node2 = stackArr[stackCount - 2];
			stackCount -= 2;

			if (node1 && node2) {
				NODE* newNode = _makeNode(expr[i]);
				newNode->left = node2;
				newNode->right = node1;

				//Push newNode int stack
				stackArr[stackCount] = newNode;
				stackCount++;
			}
			else {
				for (int i = 0; stackArr[i] != NULL; i++)
					free(stackArr[i]);
				return 0;
			}
		}
		else { //expr[i] is operand

			NODE* newNode = _makeNode(expr[i]);
			newNode->right = NULL;
			newNode->left = NULL;

			//Push newNode into stack
			stackArr[stackCount] = newNode;
			stackCount++;
		}
	}
	stackCount--;

	pTree->root = stackArr[0];
	return 1;
}

/* Print node in tree using inorder traversal
*/
void traverseTree( TREE *pTree);

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse(NODE *root) {
	if (root == NULL)
		return;

	if (isdigit(root->data) == 0) {//연산자
		printf("(");
		_traverse(root->left);
		printf("%c", root->data);
		_traverse(root->right);
		printf(")");
	}
	else {
		printf("%c", root->data);
	}
}

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);

/* internal traversal function
*/
static void _infix_print(NODE *root, int level) {
	if (root == NULL)
		return;

	_infix_print(root->right, level + 1);

	for (int i = 0; i < level; i++) 
		printf("	");
	printf("%c", root->data);
	printf("\n");

	
	_infix_print(root->left, level + 1);
}

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix(char *expr) {
	float ret = 0;

	float stackArr[MAX_STACK_SIZE];
	int stackCount = 0;

	for (int i = 0; i < strlen(expr) ; i++) {
		if (isdigit(expr[i]) == 0) {
			float num1, num2; //num1이 먼저 pop
			float tempRet = 0;
			//Pop int to num1, num2  with (float)atoi()
			num1 = stackArr[stackCount - 1];
			num2 = stackArr[stackCount - 2];
			stackCount -= 2;

			switch (expr[i]) {
			case '+':
				tempRet = num2 + num1;
				break;
			case '-':
				tempRet = num2 - num1;
				break;
			case '*':
				tempRet = num2 * num1;
				break;
			case '/':
				tempRet = num2 / num1;
				break;
			}
			//Push tempRet to stack;
			stackArr[stackCount] = tempRet;
			stackCount++;
		}
		else { //expr[i]` is operand
			//push into stack
			stackArr[stackCount] = expr[i] - 48;
			stackCount++;
		}
	}
	//Pop last result and put to ret
	ret = stackArr[0];

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse (pTree->root);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();
		
		if (!tree)
		{
			printf( "Cannot create tree\n");
			return 100;
		}
		
		// postfix expression -> expression tree
		int ret = postfix2tree( expr, tree);
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			destroyTree(tree);
			continue;
		}
		
		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree(tree);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);
		
		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);
		
		// destroy tree
		destroyTree( tree);
		
		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
