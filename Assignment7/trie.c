#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	char 			*entry; //마지막 subtree에만
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

	newTrie->entry = NULL;

	return newTrie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root) {
	if (!root)
		return;
	for (int i = 0; i < MAX_DEGREE; i++)
		trieDestroy(root->subtrees[i]);

	if (root->entry)
		free(root->entry);

	free(root);
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str) {
	int length = strlen(str);

	for (int i = 0; i < length; i++) { //영문자 외 문자 예외처리
		str[i] = tolower(str[i]);
		int index = getIndex(str[i]);
		if (index < 0 || index > MAX_DEGREE - 1)
			return 0;
	}

	if (trieSearch(root,str)) // 중복체크
		return 0;

	TRIE* pTemp = root;

	for (int i = 0; i < length; i++) {
		int index = getIndex(str[i]);

		if (!pTemp->subtrees[index])
			pTemp->subtrees[index] = trieCreateNode();
		
		pTemp = pTemp->subtrees[index];
	}

	char* inputStr = (char*)malloc(sizeof(char)*length + 1); //
	strcpy(inputStr, str);
	pTemp->entry = inputStr;
}

/* Retrieve trie for the requested key
	return	1 key found
			0 key not found
*/
int trieSearch(TRIE *root, char *str) {
	int length = strlen(str);

	for (int i = 0; i < length; i++) { //영문자 외 문자 예외처리
		str[i] = tolower(str[i]);
		int index = getIndex(str[i]);
		if (index < 0 || index > MAX_DEGREE - 1)
			return 0;
	}

	TRIE* pTemp = root;
	for (int i = 0; i < length; i++) {
		int index = getIndex(str[i]);
		if (!pTemp->subtrees[index])
			return 0;
		pTemp = pTemp->subtrees[index];
	}
	if (pTemp->entry) {
		int ret = strcmp(pTemp->entry, str);
		if (!ret)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE *root) {
	if (root->entry)
		printf("%s\n", root->entry);

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (root->subtrees[i])
			trieList(root->subtrees[i]);
	}
}

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
	using trieList function
*/
void triePrefixList(TRIE *root, char *str) {
	TRIE* pTemp = root;
	int length = strlen(str);

	for (int i = 0; i < length; i++) {
		int index = getIndex(str[i]);	

		if (!pTemp->subtrees[index])
			return;

		pTemp = pTemp->subtrees[index];
	}
	
	trieList(pTemp);
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms(char *str, char *permuterms[]) {
	int length = strlen(str) + 1;

	strcat(str, "$");
	permuterms[0] = (char*)malloc(sizeof(char)*length + 1);
	strcpy(permuterms[0], str);
	//printf("%s\n", permuterms[0]);

	for (int i = 1; i < length; i++) {
		permuterms[i] = (char*)malloc(sizeof(char)*(length + 2));
		strcpy(permuterms[i], &(permuterms[i-1][1]));
		permuterms[i][length - 1] = permuterms[i-1][0];
		permuterms[i][length] = '\0';

		//printf("%s\n", permuterms[i]);
	}

	return length;
}

/* recycles memory for permuterms
*/
void clear_permuterms(char *permuterms[], int size) {
	for (int i = 0; i < size; i++)
		free(permuterms[i]);
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
void trieSearchWildcard(TRIE *root, char *str) {
	int length = strlen(str);

	for (int i = 0; i < length; i++) { //영문자 외 문자 예외처리
		str[i] = tolower(str[i]);
		if (str[i] == '*')
			continue;

		int index = getIndex(str[i]);
		if (index < 0 || index > MAX_DEGREE - 1)
			return;
	}
	
	char* findStr = (char*)malloc(sizeof(char)*length + 1);
	for (int i = 0; i < length; i++) {
		char* empty = "\0";
		strcpy(&(findStr[i]), empty);
	}
	char* str1 = (char*)malloc(sizeof(char)*length + 1);
	char* str2 = (char*)malloc(sizeof(char)*length + 1);
	char* pEOW = "$";

	if (str[length - 1] == '*' && str[0] == '*') { //*ab* complete
		strcpy(str1, &(str[1]));
		strncpy(findStr, str1, length - 2);
	}
	else if (str[length - 1] == '*') { //ab*
		strcpy(findStr, pEOW);
		strncat(findStr, str, length - 1);
	}
	else if (str[0] == '*') { //*ab
		strcpy(str1, &(str[1]));
		strcpy(findStr, str1);
		strcat(findStr, pEOW);
	}
	else { //a*b
		int index;
		for (int i = 0; i < length; i++) { //'*'의 index 찾기
			if (str[i] == '*') {
				index = i;
				break;
			}
		}

		strncpy(str1, str, index);
		strncpy(str2, &(str[index + 1]), length - index - 1);

		strncpy(findStr, str2, length - index - 1);
		strcat(findStr, pEOW);
		strncat(findStr, str1, index);
	}

	//printf("findStr : %s\n", findStr); //디버깅용
	triePrefixList(root, findStr);

	free(findStr);
	free(str1);
	free(str2);
}

int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	printf( "Inserting to trie...\t");
	while (fscanf( fp, "%s", str) == 1) // words file
	{	
		ret = trieInsert( trie, str);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
				trieInsert( permute_trie, permuterms[i]);
		
			clear_permuterms( permuterms, num_p);
		}
	}
	
	printf( "[done]\n"); // Inserting to trie

	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) == 1)
	{
		if (strcmp(str, "end") == 0)
			break;
		if (strchr( str, '*')) // wildcard search term
		{
			trieSearchWildcard( permute_trie, str);
		}
		else // search term
		{
			ret = trieSearch( trie, str);
			printf( "[%s]%s found!\n", str, ret ? "": " not");
		}
		printf( "\nQuery: ");
	}

	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}
