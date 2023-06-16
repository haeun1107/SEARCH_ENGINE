#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DOC_MAX 100
#define	TABLE_SIZE 45000

typedef struct node* link_pointer;
typedef struct node {
	char key[100]; // 단어
	char line[2000]; // 단어가 포함된 문장
    int num_doc; // 단어가 출현한 문서 번호
    link_pointer next; // 다음 노드
    link_pointer prev; // 이전 노드
} node;
node hash_table[TABLE_SIZE]; // 해시 테이블

// 문서에 있는 단어 빈도수
typedef struct frequency_key {
    int num_doc; // 문서 번호
    int num_word; // 해당 문서에 출현한 단어 개수(빈도수)
} frequency_key;
frequency_key frequency[DOC_MAX];

// 전체 문서에 있는 서로 다른 단어의 개수
int total_indexed_words;          

// 색인 과정에서 수행한 스트링 비교 연산 횟수
int total_indexed_comparison; 

// 단어가 발견된 총 문서 수
int total_doc; 

// 검색 과정에서 수행한 스트링 비교 연산 횟수
int total_comparison; 

// 파일에서 단어들을 읽어 해시테이블 구성
void build_dictionary();

// 특수 문자 치환
void sub(char* line);

// 해시 함수1 (key의 각 character 값을 더하고 TABLE_SIZE로 나눈 나머지)
int hash1(char* key);

// 해시 함수2 (folding에 소수 31을 곱하고 key의 각 character 값을 더하여 TABLE_SIZE로 나눈 나머지를 더함)
int hash2(char* key);

// 해시테이블에 (key, line, num_doc) 자료 삽입
void hash_insert(char* key, char* line, int num_doc);

// 색인 결과 출력
void print_index_result();

// 해시테이블에서 key가 저장된 linked list를 찾아서 반환
link_pointer hash_search(char* key);

// 단어가 발견된 총 문서의 수 카운팅하면서 각 문서에 들어있는 단어 개수(빈도수) 카운팅해서 frequency 배열에 저장
void count_total(node* word); // word는 해당 단어의 linked list의 head

// keyword가 가장 많이 포함된 문서가 먼저 나오도록 정렬
void quick_sort(frequency_key list[], int left, int right);

// 배열 안에 있는 원소들의 위치 swap
void swap(frequency_key list[], int i, int j);

// 검색 결과 출력 
void print_hash_result(node* word, char* key);
