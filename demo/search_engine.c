#include <stdio.h>
#include "search_engine.h"

int main(void) {
    // 변수 초기화
    total_indexed_words = 0; // 전체 문서에 있는 서로 다른 단어의 개수
    total_indexed_comparison = 0; // 색인 과정에서 수행한 스트링 비교 연산 횟수
    total_doc = 0; // 단어가 발견된 총 문서 수
    total_comparison = 0; // 검색 과정에서 수행한 스트링 비교 연산 횟수

    // 색인 시작
    build_dictionary();
    // 색인 결과 출력
    print_index_result();
    
    // 검색 시작
    // 검색할 keyword 입력
    printf("\n------------ Result ------------\n");
    char keyword[100];
    printf("Keyword : ");
    scanf("%s", keyword);
    // 대문자와 소문자는 같은 것으로 취급
    for (int i = 0; i < strlen(keyword); i++) keyword[i] = tolower(keyword[i]);

    // key가 들어있는 node를 찾음
    link_pointer tmp = hash_search(keyword);
    // 단어가 발견된 총 문서의 수 카운팅하면서 각 문서에 들어있는 단어 개수(빈도수) 카운팅해서 frequency 배열에 저장
    count_total(tmp);
    // 단어가 발견된 총 문서 수
    printf("Total documents : %d\n\n", total_doc);

    // keyword가 가장 많이 포함된 문서가 먼저 나오도록 정렬
    quick_sort(frequency, 0, total_doc-1);
    // 검색 결과 출력 
    print_hash_result(tmp, keyword);
    // 검색 과정에서 수행한 스트링 비교 연산 횟수
    printf("Total number of comparison : %d\n", total_comparison);
}

// 파일에서 단어들을 읽어 해시테이블 구성
void build_dictionary() {
    FILE* file;
    char originLine[3000]; // 원본 문장
    char line[3000]; // 특수 문자 치환 후 문장
    char fileName[15]; // 문서 이름

    for (int i = 1; i <= DOC_MAX; i++) {
        sprintf(fileName, "doc%03d.txt", i);
        file = fopen(fileName, "r"); // 파일 열기
        if (file == NULL) { // 파일이 없다면
            printf("NO FILE!!!\n"); // 오류 메시지 출력
            return;
        }
        // 한 줄씩 읽어오기
        while (fgets(line, sizeof(line), file) != NULL) {
            strcpy(originLine, line); // 원본 문장 저장
            sub(line); // 특수 문자 치환

            char* word = strtok(line, " "); // 공백을 기준으로 문장 쪼개기
            while (word != NULL) {
                hash_insert(word, originLine, i); // 단어를 하나씩 insert
                word = strtok(NULL, " ");
            }
        }
    }
    fclose(file);
}

// 특수 문자 치환
void sub(char* line) {
    // 문장에 있는 모든 문자 확인
    for (int i = 0; i < strlen(line); i++) {
        // ASCII code 이용하여 문자(대문자, 소문자)가 아니라면
        if ((line[i] < 65 || 90 < line[i]) && (line[i] < 97 || 122 < line[i])) {
            line[i] = 32; // 공백 처리
        }
        line[i] = tolower(line[i]); // 모두 소문자로 변경
    }
}
    
// 해시테이블에 (key, line, num_doc) 자료 삽입
void hash_insert(char* key, char* line, int num_doc) {
    // 이중 해싱
    int hash_value = hash1(key); 
    int step_size = hash2(key);

    // 버켓에 이미 어떤 키가 들어있다면
    while (strlen(hash_table[hash_value].key) != 0) {
        // 그 키가 바로 내 키라면 chaining
        if (strcmp(hash_table[hash_value].key, key) == 0) {
            // 해당 단어가 들어있는 해시테이블의 node를 찾아 linked list 이용
            link_pointer word = &(hash_table[hash_value]);
            
            link_pointer new = (link_pointer)malloc(sizeof(node));
            strcpy(new->key, key);
            strcpy(new->line, line);
            new->num_doc = num_doc;

            // circular linked list
            if (word->next == word) {
                word->next = new;
                new->prev = word;
                new->next = word;
                word->prev = new;
            }
            else {
                link_pointer tail = word->prev;
                tail->next = new;
                new->prev = tail;

                new->next = word;
                word->prev = new;
            }

            // 색인 한번 완료했으므로
            total_indexed_comparison++;
            return; // 종료
        }

        // 버켓에 있는 키가 내 키가 아니라면 다음 버켓 검사 (linear probing)
        hash_value = (hash_value + step_size) % TABLE_SIZE;

        // 제자리로 돌아왔다면 해시 테이블은 모두 차있는 상태
        if (hash_value == hash1(key)) {
            printf("Table is FULL!!!!\n");
            return; // 종료
        }
    }

    // 버켓이 비어있다면 insert
    strcpy(hash_table[hash_value].key, key);
    strcpy(hash_table[hash_value].line, line);
    hash_table[hash_value].num_doc = num_doc;
    hash_table[hash_value].next = &(hash_table[hash_value]);
    hash_table[hash_value].prev = &(hash_table[hash_value]);
    
    total_indexed_comparison++;
    total_indexed_words++; // 새로운 단어 색인 완료
}

// 해시 함수1 (folding에 key의 각 character 값을 TABLE_SIZE로 나눈 나머지를 더함)
int hash1(char* key) {
    int folding = 0;
	for (int i = 0; key[i] != '\0'; i++) {
		folding += key[i] % TABLE_SIZE;
	}
	return folding;
}

// 해시 함수2 (folding에 소수 31을 곱하고 key의 각 character 값을 더하여 TABLE_SIZE로 나눈 나머지를 더함)
int hash2(char* key) {
    int folding = 0;
    int prime = 31;
    for (int i = 0; key[i] != '\0'; i++) {
        folding = (key[i] + folding * prime) % TABLE_SIZE;
    }
    return folding;
}

// 색인 결과 출력
void print_index_result() {
    printf("Total number of documents: %d\n", DOC_MAX);
    printf("Total number of indexed wordes: %d\n", total_indexed_words);
    printf("Total number of comparison: %d\n", total_indexed_comparison);
}

// 해시테이블에서 key가 저장된 linked list를 찾아서 반환
link_pointer hash_search(char* key) {
    // 이중 해싱
    int hash_value = hash1(key); 
    int step_size = hash2(key);

    // 버켓에 어떤 키가 들어있다면
    while (strlen(hash_table[hash_value].key) != 0) {
        total_comparison++;
        // 그 키가 바로 내 키라면 검색 성공
        if (strcmp(hash_table[hash_value].key, key) == 0) {
            // 해당 단어가 들어있는 해시테이블의 node를 찾아 linked list 이용
            link_pointer word = hash_table + hash_value;
            return word;
        }

        // 버켓에 있는 키가 내 키가 아니라면 다음 버켓 검사 (linear probing)
        hash_value = (hash_value + step_size) % TABLE_SIZE;

        // 제자리로 돌아왔다면 key가 존재하지 않으므로 검색 실패
        if (hash_value == hash1(key)) {
            printf("NO KEY!!!\n");
            exit(1); // 프로그램 종료
            return NULL;
        }
    }

    // 해시테이블에 key가 존재하지 않으면 검색 실패
    printf("NO KEY!!!\n");
    exit(1); // 프로그램 종료
    return NULL;
}

// 단어가 발견된 총 문서의 수 카운팅하면서 각 문서에 들어있는 단어 개수(빈도수) 카운팅해서 frequency 배열에 저장
void count_total(node* word) {
    // 단어가 발견된 총 문서의 수 카운팅
    link_pointer tmp = word; // 단어 정보가 담긴 node
    
    int now = 0; // 현재 문서 번호
    int i = -1;
    do {
        if (tmp->num_doc != now) { // 탐색 중인 문서 번호와 현재 문서 번호가 다르면
            frequency[++i].num_doc = tmp->num_doc; // frequency 배열에 문서 번호 저장
            frequency[i].num_word = 0; // frequency 배열의 단어 개수 초기화
            total_doc++; // 새로운 문서에서 단어가 발견되었으므로 총 문서 수 1 증가
            now = tmp->num_doc; // 현재 문서 번호 갱신
        }
        frequency[i].num_word++; // 단어 개수 1 증가
        tmp = tmp->next;
    } while (tmp != word);
}

// keyword가 가장 많이 포함된 문서가 먼저 나오도록 quick sort
void quick_sort(frequency_key list[], int left, int right) {
    int i, j, pivot;
    if (left < right) {
        i = left;
        j = right + 1;
        pivot = list[left].num_word;

        while (i < j) {
            // pivot 보다 작은 값을 찾음
            while (list[++i].num_word > pivot);
            // pivot 보다 큰 값을 찾음
            while (list[--j].num_word < pivot);
            
            if (i < j) swap(list, i, j);
        }
        
        // pivot을 중간(j)에 놓음
        swap(list, left, j);

        quick_sort(list, left, j-1);
        quick_sort(list, j+1, right);
    }
}

// 배열 안에 있는 원소들의 위치 swap
void swap(frequency_key list[], int i, int j) {
    frequency_key tmp;
    tmp = list[i];
    list[i] = list[j];
    list[j] = tmp;
}

// 검색 결과 출력 
void print_hash_result(node* word, char* key) {
    link_pointer tmp;
    char* key_start;
    int word_cnt = 0;
    // 단어가 발견된 총 문서 수만큼 반복
    for (int i = 0; i < total_doc; i++) {
        tmp = word;
        // frequency에 저장된 문서를 순서대로 가져옴
        printf("<doc%03d.txt> (%s : %d)\n", frequency[i].num_doc, key, frequency[i].num_word);
        word_cnt += frequency[i].num_word;
        // 해당 문서에 있는 단어가 나올 때까지 반복
        do {
            if (tmp->num_doc == frequency[i].num_doc) break;
            tmp = tmp->next; 
        } while (tmp != word);

        do {
            // line의 특수 문장 제거
            char subline[3000]; 
            strcpy(subline, tmp->line);
            sub(subline);

            char change_key[100] = " ";
            strcpy(change_key, tmp->key);
            strcat(change_key, " "); // 단어는 공백을 기준으로 구분되어 있으므로
            // line에서 key를 찾아서 주소값 반환
            key_start = strstr(subline, change_key);
            // key에 도달하면 글자색 변경 후 출력
            for (int k = 0; k < strlen(subline); k++) { 
                if ((k >= key_start - subline) && k < (key_start - subline + strlen(tmp->key))) {
                    printf("\033[1;35m%c\033[0m", tmp->line[k]);
                }
                else printf("%c", tmp->line[k]);
            }
            tmp = tmp->next;
        } while (tmp != word && tmp->num_doc == frequency[i].num_doc);
        printf("\n\n");
    }
    printf("Total number of word : %d \n", word_cnt);
}