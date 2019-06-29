#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "trie.h"

// 역색인 헤더 정보에 대한 구조체
typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// document frequency
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
static char *trim( char *str);

// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header( char *filename){
	FILE* fHead = fopen(filename, "rb");
	if(!fHead)
		return NULL;

	tHEADER* tHead = (tHEADER*)malloc(sizeof(tHEADER) * 25700);
	if(!tHead)
		return NULL;

	fread(tHead, sizeof(tHEADER), 25700, fHead);
	fclose(fHead);

	return tHead;
}

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting( char *filename){
	FILE* fpost = fopen(filename, "rb");
	if(!fpost)
		return NULL;
	
	int* posting = (int*)malloc(sizeof(int) * 410000);
	if(!posting)
		return NULL;

	fread(posting, sizeof(int), 410000, fpost);
	fclose(fpost);

	return posting;
}

// 문서 집합을 화면에 출력한다.
void showDocuments( int *docs, int numdocs){
	for(int i = 0; i < numdocs; i++)
		printf(" %d", docs[i]);

	printf("\n");
}

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs){
	int* newDocs = (int*)malloc(sizeof(int) * (numdocs + numdocs2));
	int num = 0;
	int now1 = 0;
	int now2 = 0;
	//printf("%d %d \n", numdocs, numdocs2);

	while(now1 < numdocs && now2 < numdocs2){
		if(docs[now1] < docs2[now2])
			now1++;
		else if(docs[now1] > docs2[now2])
			now2++;
		else{ //docs[now1] == docs2[now2]
			newDocs[num] = docs[now1];
			now1++;
			now2++;
			num++;
		}
	}

	newDocs = (int*)realloc(newDocs, sizeof(int) * num);
	*newnumdocs = num;

	free(docs);
	free(docs2);

	return newDocs;
}

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs){
	int* newDocs = (int*)malloc(sizeof(int) * (numdocs + numdocs2));
	int num = 0;
	int now1 = 0;
	int now2 = 0;
	//printf("%d %d \n", numdocs, numdocs2);

	while(now1 <= numdocs && now2 <= numdocs2){
		if(now1 == numdocs && now2 == numdocs2)
			break;
		else if(now1 == numdocs){
			while(now2 < numdocs2){
				newDocs[num] = docs2[now2];
				now2++;
				num++;
			}
			break;
		}
		else if(now2 == numdocs2){
			while(now1 < numdocs){
				newDocs[num] = docs[now1];
				now1++;
				num++;
			}
			break;
		}

		else if(docs[now1] < docs2[now2]){
			newDocs[num] = docs[now1];
			now1++;
		}
		else if(docs[now1] > docs2[now2]){
			newDocs[num] = docs2[now2];
			now2++;
		}
		else{//docs[now1] == docs2[now2]
			newDocs[num] = docs[now1];
			now1++;
			now2++;
		}
		num++;
	}

	newDocs = (int*)realloc(newDocs, sizeof(int) * num);
	*newnumdocs = num;

	free(docs);
	free(docs2);

	return newDocs;
}

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *getDocuments( tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs){
	term = trim(term);
	int ret = trieSearch(trie, term); //index == dic 속 index
	if(ret == -1){
		*numdocs = 0;
		return NULL;
	}

	int startIndex = header[ret].index;
	*numdocs = header[ret].df;

	int* docs = (int*)malloc(sizeof(int) * (*numdocs));

	for(int i = 0; i < *numdocs; i++)
		docs[i] = posting[startIndex + i];
		
	return docs;
}

// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *searchDocuments( tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs){
	query = trim(query);

	for(int i = 0; i < strlen(query); i++){ //소문자, &, | 외 문자 예외처리
		int index = query[i];
		if(isalpha(index) || index == '&' || index == '|' || index == ' ')
			continue;
		else
			return NULL;
	}

	for(int i = 0; i < strlen(query); i++){
		if(isalpha(query[i])){
			query[i] = tolower(query[i]);
		}
	}
	char* ptr1 = strrchr(query,'|');
	char* ptr2 = strrchr(query,'&');

	if (ptr1 || ptr2){ 
		if(ptr1 && !ptr2){
			ptr1 = strtok(ptr1, "|");

			int newLen = strlen(query) - (strlen(ptr1) + 1);

			char* ptr3 = (char*)malloc(sizeof(char) * newLen);
			strncpy(ptr3, query, newLen);
			ptr3[newLen] = '\0';

			int numdocs1;
			int* docs1 = searchDocuments(header, posting, trie, ptr3, &numdocs1);

			int numdocs2;
			int* docs2 = getDocuments(header, posting, trie, ptr1, &numdocs2);

			//printf("%s %s\n", ptr3, ptr1);
			free(ptr3);

			if(docs1 && docs2)
				return unionDocuments(docs1, numdocs1, docs2, numdocs2, numdocs);
			else{
				if(!docs1 && !docs2)
					return NULL;
				else if(!docs1 && docs2){
					*numdocs = numdocs2;
					return docs2;
				}
				else{
					*numdocs = numdocs1;
					return docs1;
				}
			}
		}
		else if(!ptr1 && ptr2){
			ptr2 = strtok(ptr2, "&");
			int newLen = strlen(query) - (strlen(ptr2) + 1);

			char* ptr3 = (char*)malloc(sizeof(char) * newLen);
			strncpy(ptr3, query, newLen);
			ptr3[newLen] = '\0';

			int numdocs1;
			int* docs1 = searchDocuments(header, posting, trie, ptr3, &numdocs1);

			int numdocs2;
			int* docs2 = getDocuments(header, posting, trie, ptr2, &numdocs2);
			
			//printf("%s %s\n", ptr3, ptr2);
			free(ptr3);
			if(docs1 && docs2)
				return intersectDocuments(docs1, numdocs1, docs2, numdocs2, numdocs);
			else 
				return NULL;
		}
		else {
			if(strlen(ptr1) < strlen(ptr2)){
				ptr1 = strtok(ptr1, "|");

				int newLen = strlen(query) - (strlen(ptr1) + 1);

				char* ptr3 = (char*)malloc(sizeof(char) * newLen);
				strncpy(ptr3, query, newLen);
				ptr3[newLen] = '\0';

				int numdocs1;
				int* docs1 = searchDocuments(header, posting, trie, ptr3, &numdocs1);

				int numdocs2;
				int* docs2 = searchDocuments(header, posting, trie, ptr1, &numdocs2);

				free(ptr3);

				if(docs1 && docs2)
					return unionDocuments(docs1, numdocs1, docs2, numdocs2, numdocs);
				else{
					if(!docs1 && !docs2)
						return NULL;
					else if(!docs1 && docs2){
						*numdocs = numdocs2;
						return docs2;
					}
					else {
						*numdocs = numdocs1;
						return docs1;
					}
				}			
			}	
			else{
				ptr2 = strtok(ptr2, "&");

				int newLen = strlen(query) - (strlen(ptr2) + 1);

				char* ptr3 = (char*)malloc(sizeof(char) * newLen);
				strncpy(ptr3, query, newLen);
				ptr3[newLen] = '\0';

				int numdocs1;
				int* docs1 = searchDocuments(header, posting, trie, ptr3, &numdocs1);

				int numdocs2;
				int* docs2 = searchDocuments(header, posting, trie, ptr2, &numdocs2);
				
				free(ptr3);
				if(docs1 && docs2)
					return intersectDocuments(docs1, numdocs1, docs2, numdocs2, numdocs);
				else
					return NULL;
			}
		}
		
	}
	else {//single term
		return getDocuments(header, posting, trie, query, numdocs);
	}
}

////////////////////////////////////////////////////////////////////////////////
static char *rtrim( char *str)
{
	char *p = str + strlen( str) - 1;
	
	while (p >= str)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') *p = 0;
		else return str;
		p--;
	}
}

static char *ltrim( char *str)
{
	char *p = str;
	
	while (*p)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') p++;
		else 
		{
			return p;
		}
	}
	return NULL;
}

static char *trim( char *str)
{
	if( str == NULL || *str == 0) return str;
	
	return rtrim(ltrim(str));
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	tHEADER *header;
	int *posting;
	TRIE *trie;
	char query[100];
	int index;
	
	header = load_header( "header.idx");
	if (header == NULL) return 1;
	
	posting = load_posting( "posting.idx");
	if (posting == NULL) return 1;
		
	printf("Inserting to trie...	");
	trie = dic2trie( "dic.txt");
	printf("[done]\n");

#if	DEBUG
	trieList(trie);
#endif

	printf( "\nQuery: ");
	while (fgets( query, 100, stdin) != NULL)
	{
	
		int numdocs;
		int *docs = searchDocuments( header, posting, trie, query, &numdocs);
		
		if (docs == NULL) printf( "not found!\n");
		else 
		{
			showDocuments( docs, numdocs);
			free( docs);
		}
		printf( "\nQuery: ");
	}
	
	free( header);
	free( posting);
	trieDestroy( trie);
	
	return 0;
}
