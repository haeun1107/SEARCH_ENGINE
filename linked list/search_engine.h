#define DOC_MAX 100
#define	TABLE_SIZE 30000

// 전체 문서에 있는 서로 다른 단어의 개수
int total_indexed_words;          

// 색인 과정에서 수행한 스트링 비교 연산 횟수
int total_indexed_comparison; 

// 파일에서 단어들을 읽어 해시테이블 구성
void build_dictionary();

// 특수 문자 치환
void sub(char* line);

// Hash table
typedef struct node* link_pointer;
typedef struct node {
	char key[100];
	char line[3000];
    int num_doc;
    link_pointer link;
} node;
node hash_table[TABLE_SIZE];

// 해시 함수 (folding + division (TABLE_SIZE로 나눈 나머지))
int hash(char* key);

// folding (key의 각 character 값을 더함)
int transform(char* key);

// 해시테이블에 (key, line, num_doc) 자료 삽입
void hash_insert(char* key, char* line, int num_doc);

// 색인 결과 출력
void print_index_result();

int total_doc; // 단어가 발견된 총 문서 수
int total_comparison; // 비교 연산 횟수

// 해시테이블에서 key가 저장된 linked list를 찾아서 반환
link_pointer hash_search(char* key);

// 단어가 발견된 총 문서의 수 카운팅
void count_total_doc(node* word); // word는 해당 단어의 linked list의 head

// 문서에 있는 단어 개수
typedef struct frequency_key {
    int num_doc;
    int num_word;
} frequency_key;
frequency_key frequency[DOC_MAX];

// 각 문서에 들어있는 단어 개수 카운팅 후 frequency 배열에 저장
void save_list(node* word);

// keyword가 가장 많이 포함된 문서가 먼저 나오도록 정렬
void quick_sort(frequency_key list[], int left, int right);

// 배열 안에 있는 원소들의 위치 swap
void swap(frequency_key list[], int i, int j);

// 검색 결과 출력 
void print_hash_result(node* word, char* key);
