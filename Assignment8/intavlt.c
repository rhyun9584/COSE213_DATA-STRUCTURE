#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time

#define MAX_ELEM 20
#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
static void _destroy(NODE *root);
static NODE *_makeNode(int data);
static NODE *_insert(NODE *root, NODE *newPtr);
static int getHeight(NODE *root);
static NODE *rotateRight(NODE *root);
static NODE *rotateLeft(NODE *root);
static NODE *RR(NODE* root);
static NODE *LL(NODE* root);
static NODE *LR(NODE* root);
static NODE *RL(NODE* root);
static void _infix_print(NODE *root, int level);
static void _traverse(NODE *root);
static NODE *_retrieve(NODE *root, int key);

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create(void) {
	AVL_TREE *newTree = (AVL_TREE*)malloc(sizeof(AVL_TREE));
	if (!newTree)
		return NULL;

	newTree->root = NULL;
	newTree->count = 0;

	return newTree;
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
AVL_TREE *AVL_Destroy(AVL_TREE *pTree) {
	if (pTree)
		_destroy(pTree->root);

	free(pTree);
	return NULL;
}
static void _destroy(NODE *root) {
	if (!root)
		return;

	_destroy(root->left);
	_destroy(root->right);

	free(root);
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert(AVL_TREE *pTree, int data) {
	NODE* newNode = _makeNode(data);

	if (!newNode)
		return 0;
	
	pTree->root = _insert(pTree->root, newNode);
	pTree->root->height = getHeight(pTree->root);
	pTree->count++;

	return 1;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert(NODE *root, NODE *newPtr) { //node insert
	int rotation = 0;
	int LH, RH, diff;

	if (!root)
		return newPtr;
	else if (newPtr->data < root->data) {//go to left subtree
		root->left = _insert(root->left, newPtr);
		root->height = getHeight(root);

		LH = getHeight(root->left);
		RH = getHeight(root->right);
		diff = LH - RH;

		if (diff > 1) 
			if (newPtr->data < root->left->data)
				root = LL(root);
			else
				root = LR(root);
	}
	else {
		root->right = _insert(root->right, newPtr);
		root->height = getHeight(root);
	
		LH = getHeight(root->left);
		RH = getHeight(root->right);
		diff = LH - RH;

		if (diff < -1)
			if (newPtr->data < root->right->data)
				root = RL(root);
			else
				root = RR(root);
	}

	return root;
}

static NODE *_makeNode(int data) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode)
		return NULL;

	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->height = 1;

	return newNode;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *AVL_Retrieve(AVL_TREE *pTree, int key) {
	NODE* pLoc = _retrieve(pTree->root, key);

	if (pLoc)
		return &pLoc->data;
	else
		return NULL;
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve(NODE *root, int key) {
	if (!root)
		return NULL;

	if (key > root->data)
		return _retrieve(root->right, key);
	else if (key < root->data)
		return _retrieve(root->left, key);
	else
		return root;
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse(AVL_TREE *pTree) {
	_traverse(pTree->root);
}

static void _traverse(NODE *root) {
	if (!root)
		return;

	_traverse(root->left);
	printf("%d ", root->data);
	_traverse(root->right);
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree(AVL_TREE *pTree) {
	_infix_print(pTree->root, 0);
}
/* internal traversal function
*/
static void _infix_print(NODE *root, int level) {
	if (!root)
		return;

	_infix_print(root->right, level + 1);
	
	for (int i = 0; i < level; i++)
		printf("	");
	printf("%d", root->data);
	printf("\n");

	_infix_print(root->left, level + 1);

}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight(NODE *root) {
	int LH = 0, RH = 0;

	if (!root)
		return 0;
	else {
		if (root->left) {
			root->left->height = getHeight(root->left);
			LH = root->left->height;
		}
		if (root->right){
			root->right->height = getHeight(root->right);
			RH = root->right->height;
		}

		return max(LH, RH) + 1;
	}
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight(NODE *root) {
	NODE* newRoot = root->left;
	root->left = newRoot->right;
	newRoot->right = root;

	newRoot->height = getHeight(newRoot);

	return newRoot;
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft(NODE *root) {
	NODE* newRoot = root->right;
	root->right = newRoot->left;
	newRoot->left = root;

	newRoot->height = getHeight(newRoot);

	return newRoot;
}

static NODE *RR(NODE* root) {
	root = rotateLeft(root);
	return root;
}
static NODE *LL(NODE* root) {
	root = rotateRight(root);
	return root;
}
static NODE *RL(NODE* root) {
	root->right = rotateRight(root->right);
	root = rotateLeft(root);
	return root;
}
static NODE *LR(NODE* root) {
	root->left = rotateLeft(root->left);
	root = rotateRight(root);
	return root;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	int data;
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 0; i < MAX_ELEM; i++)
	{
		data = rand() % (MAX_ELEM * 3) + 1; // random number
		// data = i+1; // sequential number
		
		fprintf( stdout, "%d ", data);

		// insert function call
		AVL_Insert( tree, data);

		//test
//		printf("\n------------------------------------\n");
//		printTree(tree);
//		printf("------------------------------------\n\n");
	}
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
//	tree->root->height = getHeight(tree->root);

	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	AVL_Destroy( tree);

	return 0;
}
