#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

static void _destroy(NODE *root);
static NODE *_makeNode(int data);
NODE *_delete(NODE *root, int dltKey, int *success, NODE* pPre);
NODE *_retrieve(NODE *root, int key);
static void _traverse(NODE *root);
static void _infix_print(NODE *root, int level);

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create(void) {
	TREE* tree = (TREE*)malloc(sizeof(tree));
	if (!tree)
		return NULL;

	tree->root = NULL;

	return tree;
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *BST_Destroy(TREE *pTree) {
	if (pTree) {
		_destroy(pTree->root);
	}

	free(pTree);
	return NULL;
}

static void _destroy(NODE *root) {
	if (root == NULL)
		return;

	_destroy(root->left);
	_destroy(root->right);
	free(root);

	return;
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert(TREE *pTree, int data) {
	NODE* newNode = _makeNode(data);
	if (!newNode)
		return 0;

	NODE* pPre = pTree->root;
	if (pPre == NULL) {
		pTree->root = newNode;
		return 1;
	}
	while (1) {
		if (newNode->data >= pPre->data) { //right subtree�� ������
			if (pPre->right == NULL) {
				pPre->right = newNode;
				break;
			}
			else {
				pPre = pPre->right;
				continue;
			}
		}
		else {//left subtree
			if (pPre->left == NULL) {
				pPre->left = newNode;
				break;
			}
			else {
				pPre = pPre->left;
				continue;
			}
		}
	}
	return 1;
}

static NODE *_makeNode(int data) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (!newNode)
		return 0;
	
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete(TREE *pTree, int dltKey) {
	int success;
	_delete(pTree->root, dltKey, &success, NULL);
	
	return success;
}

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
			NULL dltKey not found
*/
NODE *_delete(NODE *root, int dltKey, int *success, NODE* pPre) {
	if (!root) {
		*success = 0;
		return NULL;
	}

	if (dltKey < root->data) {
		return _delete(root->left, dltKey, success, root);
	}
	else if (dltKey > root->data) {
		return _delete(root->right, dltKey, success, root);
	}
	else { //dltKey == root->data
		NODE* dltNode = root;

		if (root->left == NULL) {
			if (pPre) {
				if (pPre->left == root)
					pPre->left = root->right;
				else
					pPre->right = root->right;
			}
			else 
				root = root->right;
	
			free(dltNode);
			*success = 1;
			return root;
		}
		else if (root->right == NULL) {
			if (pPre) {
				if (pPre->left == root)
					pPre->left = root->left;
				else
					pPre->right = root->left;
			}
			else
				root = root->left;

			free(dltNode);
			*success = 1;
			return root;
		}
		else {//have two subtree
			NODE* largest = root->left;
			NODE* pLar = root;
			int temp;
			while (largest->right) {
				pLar = largest;
				largest = largest->right;
			}
			root->data = largest->data;
			_delete(largest, largest->data, &temp, pLar);
			*success = 1;
			return root;
		}
	}
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve(TREE *pTree, int key) {
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
NODE *_retrieve(NODE *root, int key) {
	if (!root)
		return NULL;

	if (key > root->data)
		return _retrieve(root->right, key);
	else if (key < root->data)
		return _retrieve(root->left, key);
	else
		return root;

}

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE *pTree) {
	_traverse(pTree->root);
}
static void _traverse(NODE *root) {
	if (!root)
		return;

	_traverse(root->left);
	printf("%d ", root->data);
	_traverse(root->right);
}

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE *pTree) {
	_infix_print(pTree->root, 0);
}
/* internal traversal function
*/
static void _infix_print(NODE *root, int level) {
	if (!root)
		return;

	_infix_print(root->right, level + 1);

	for (int i = 0; i < level; i++) {
		printf("	");
	}
	printf("%d\n", root->data);
	
	_infix_print(root->left, level + 1);
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	int data;
	
	// creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 1; i < 20; i++)
	{
		data = rand() % 100 + 1; // 1 ~ 100 random number
		
		fprintf( stdout, "%d ", data);
		
		// insert funtion call
		BST_Insert( tree, data);
 	}
	fprintf( stdout, "\n");
			
	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	int ret;
	do
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		ret = scanf( "%d", &num);
		if (ret != 1) break;
		
		ret = BST_Delete( tree, num);
		if (!ret) fprintf( stdout, "%d not found\n", num);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		
	} while(1);
	
	BST_Destroy( tree);

	return 0;
}

