
#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW

// TRIE type definition
typedef struct trieNode {
	int 			index; // 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
int trieSearch(TRIE *root, char *str);

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void) {
	TRIE* newTrie = (TRIE*)malloc(sizeof(TRIE));
	if (!newTrie)
		return NULL;

	for (int i = 0; i < MAX_DEGREE; i++)
		newTrie->subtrees[i] = NULL;

	newTrie->index = -1;

	return newTrie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root) {
	if (!root)
		return;

	for (int i = 0; i < MAX_DEGREE; i++)
		trieDestroy(root->subtrees[i]);

	free(root);
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index){
	int length = strlen(str);

	if(trieSearch(root, str) > -1) //중복 존재
		return 0;

	TRIE* pTemp = root;

	for(int i = 0; i < length; i++){
		int index = str[i] - 'a';

		if(!pTemp->subtrees[index])
			pTemp->subtrees[index] = trieCreateNode();

		if(i < length - 1)
			pTemp = pTemp->subtrees[index];
	}

	pTemp->index = dic_index;

	return 1;
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str){
	int length = strlen(str);

	TRIE* pTemp = root;

	for (int i = 0; i < length; i++){
		int index = str[i] - 'a';
		if(!pTemp->subtrees[index] || index < 0 || index > MAX_DEGREE - 1)
			return -1;
		pTemp = pTemp->subtrees[index];
	}

	if(pTemp->index > -1)
		return pTemp->index;
	else
		return -1;
}

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root);

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
	using trieList function
*/
void triePrefixList( TRIE *root, char *str);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str);

/* makes a trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2trie( char *dicfile){
	FILE* fdic = fopen(dicfile, "rt");
	if(!fdic)
		return NULL;

	TRIE* newTrie = trieCreateNode();
	if(!newTrie)
		return NULL;

	char str[64];
	int i = 0;
	while (fgets(str, sizeof(str), fdic)){
		trieInsert(newTrie, str, i);
		i++;
	}

	fclose(fdic);

	return newTrie;
}

/* makes a permuterm trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2permute_trie( char *dicfile);