#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// 토큰-문서 구조체
typedef struct {
	char	*token;	// 토큰
	int		docid;	// 문서번호(document ID)
} tTokenDoc;

typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// 문서 빈도(document frequency)
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
static int _compare(const void *n1, const void *n2);

// 토큰 구조체로부터 역색인 파일을 생성한다.
void invertedIndex( tTokenDoc *tokens, int num_tokens, char *dicfilename, char *headerfilename, char *postingfilename){
	tHEADER* tHead = (tHEADER*)malloc(sizeof(tHEADER) * num_tokens);
	int sizeHead = 0;
	int realNum = 0;

	int* tPost = (int*)malloc(sizeof(int) * num_tokens);

	FILE* fDic = fopen(dicfilename,"wt");
	FILE* fHead = fopen(headerfilename, "wb");
	FILE* fPost = fopen(postingfilename, "wb");

	for(int i = 0; i < num_tokens; i++){
		if(i == 0){
			fputs(tokens[0].token, fDic);
			fputs("\n", fDic);

			tHead[sizeHead].index = 0;
			tHead[sizeHead].df = 1;
			sizeHead++;
		}
		else if(strcmp(tokens[i].token, tokens[i-1].token) == 0){ //중복 : header df++, i++
			if(tokens[i].docid == tokens[i-1].docid)
				continue;

			tHead[sizeHead - 1].df++;
		}
		else{
			fputs(tokens[i].token, fDic);
			fputs("\n", fDic);

			tHead[sizeHead].index = tHead[sizeHead-1].index + tHead[sizeHead-1].df;
			tHead[sizeHead].df = 1;
			sizeHead++;
		}

		tPost[realNum] = tokens[i].docid;
//		printf("!%s	%d / %d / %d %d\n", tokens[i].token, tokens[i].docid, tPost[realNum],
//						tHead[sizeHead - 1].index, tHead[sizeHead-1].df);
		realNum++;
	}

	tHead = (tHEADER*)realloc(tHead, sizeof(tHEADER) * sizeHead);
	tPost = (int*)realloc(tPost, sizeof(int) * realNum);

	fwrite(tHead, sizeof(tHEADER), sizeHead, fHead);
	fwrite(tPost, sizeof(int), realNum, fPost);
//	printf("%d %d\n", ret1, ret2);

	fclose(fDic);
	fclose(fHead);
	fclose(fPost);

	free(tHead);
	free(tPost);
}

// 입력 파일로부터 토큰-문서 구조체를 생성한다.
tTokenDoc *get_tokens(char *filename, int *num_tokens){
	int numToken = 0;
	char str[50000];
	tTokenDoc* tokenDoc = (tTokenDoc*)malloc(sizeof(tTokenDoc) * 1000000);
	FILE* fp = fopen(filename, "rt");

	if(!fp){
		printf("cannot open file.\n");
		return NULL;
	}

	int num = 1;

	while(fgets(str, sizeof(str), fp)){
		char* ptr = strtok(str, " ");
		
		while(strcmp(ptr, "\n") != 0 && ptr){
			tokenDoc[numToken].token = strdup(ptr);
			tokenDoc[numToken].docid = num;
			numToken++;

			ptr = strtok(NULL, " ");
		}
		num++;
	}
		
	tokenDoc = (tTokenDoc*)realloc(tokenDoc, sizeof(tTokenDoc) * numToken); //메모리 사이즈 재할당

	fclose(fp);
	*num_tokens = numToken;
	return tokenDoc;
}

// qsort를 위한 비교함수 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
static int _compare(const void *n1, const void *n2){
	tTokenDoc token1 = *(tTokenDoc*)n1;
	tTokenDoc token2 = *(tTokenDoc*)n2;

	int comp = strcmp(token1.token,token2.token);

	if(comp == 0){
		if(token1.docid < token2.docid)
			return -1;
		else if(token1.docid > token2.docid)
			return 1;
		else
			return 0;
	}
	else if(comp < 0)
		return -1;
	else
		return 1;	
}

////////////////////////////////////////////////////////////////////////////////
static void print_tokens( tTokenDoc *tokens, int num_tokens)
{
	int i;
	
	for (i = 0; i < num_tokens; i++)
	{
		printf( "%s\t%d\n", tokens[i].token, tokens[i].docid);
	}
}

////////////////////////////////////////////////////////////////////////////////
void destroy_tokens( tTokenDoc *tokens, int num_tokens)
{
	int i;
	
	for (i = 0; i < num_tokens; i++)
	{
		free( tokens[i].token);
	}
	free( tokens);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	tTokenDoc *tokens;
	int num_tokens = 0;

	if (argc != 2)
	{
		printf( "Usage: %s FILE\n", argv[0]);
		return 2;
	}
	
	tokens = get_tokens( argv[1], &num_tokens);
	
	assert( tokens != NULL && num_tokens > 0);

#if DEBUG	
//	print_tokens( tokens, num_tokens);
#endif

	// 정렬 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
	qsort( tokens, num_tokens, sizeof( tTokenDoc), _compare);
	
	invertedIndex( tokens, num_tokens, "dic.txt", "header.idx", "posting.idx");

#if DEBUG	
	print_tokens( tokens, num_tokens);
#endif	

	destroy_tokens( tokens, num_tokens);
	
	return 0;
}

