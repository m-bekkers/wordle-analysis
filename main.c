#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>

#define WORD_LEN 5
#define N_SLOTS (26 * 26 * 26 * 26 * 26)

struct Node {
    char *word;
    int count;
    struct Node *next;
};

struct Node* init_ll() {
    struct Node *head = malloc(sizeof *head);
    head->word = NULL;
    head->next = NULL;
    head->count = 0;
    return head;
}

struct CompareArgs {
    struct Node *current_word;
    struct Node *words;
};

void append_node(struct Node *head, char *data) {

    struct Node *ptr = head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }

    struct Node *new = init_ll();
    new->word = data;
    ptr->next = new;
}

void print_ll(struct Node *head) {

    struct Node *ptr = head;
    while (ptr->next != NULL) {
        printf("%s", ptr->word);
        ptr = ptr->next;
    }
}

int count(char *word, char target) {

    int c = 0;
    for (int i = 0; i < WORD_LEN; i++) {
        if (word[i] == target) {
            c++;
        }
    }
    return c;
}

bool in(char *word, char c) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] == c) {
            return true;
        }
    }
    return false;
}

int* process_word(char *start_word, char *word) {

    int *ret = malloc(sizeof(int) * WORD_LEN);
    char *seen = malloc(sizeof(char) * WORD_LEN);

    for (int i = 0; i < WORD_LEN; i++) {

        if (start_word[i] == word[i]) {
            ret[i] = 2;

        } else if (in(word, start_word[i])) {

            if (in(seen, start_word[i])) {

                if (count(seen, start_word[i]) < count(start_word, start_word[i])) {
                    ret[i] = 1;

                } else {
                    ret[i] = 0;
                }
            } else {
                ret[i] = 1;
            }
        } else {
            ret[i] = 0;
        }
        seen[i] = word[i];
    }
    free(seen);
    return ret;
}

int* compare_word(struct Node *current_word, struct Node *words) {
    struct Node *comparison_word = words;
    int *result;
    while (comparison_word->next != NULL) {
        result = process_word(current_word->word, comparison_word->word);
        comparison_word = comparison_word->next;
    }
    return result;
}

void* compare_word_thread(void *arg) {
    struct CompareArgs *args = arg;
    int *result = compare_word(args->current_word, args->words);
    for (int i = 0; i < WORD_LEN; i++) {
        printf("%d", result[i]);
    }
    printf("\n");
    free(result);
    return NULL;
}

void printb(unsigned int num) {

    int bits = sizeof(unsigned int) * 8;

    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
}

void printb_s(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        const char c = (unsigned char)str[i];
        for (int bit = 7; bit >=0; bit--) {
            printf("%d", (c >> bit) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

int main() {
    struct Node *words = init_ll();
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t n_words = 0;

    fp = fopen("./words.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    int h = 0;
    while ((read = getline(&line, &len, fp)) != -1) {

        if (read == 6) { // 6 includes null terminator - we can remove this
            read--;
        }

        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        n_words++;

        if (h) {
            append_node(words, strdup(line));
        } else {
            words->word = strdup(line);
            h = 1;
        }
    }
    fclose(fp);

    if (line) {
        free(line);
    }

    print_ll(words);

    char *w1 = "aefed";
    char *w2 = "acree";
    int res = *w1 & *w2;
    printb_s(w1);
    printb_s(w2);
    printb(res);

    int *result = process_word(w1, w2);
    for (int i = 0; i < WORD_LEN; i++) {
        printf("%d", result[i]);
    }

    printf("\n");

    pthread_t thread_ids[10];
    struct CompareArgs args[n_words];

    int i = 0;
    struct Node *start_word = words;
    while (start_word->next != NULL) {
        args[i] = (struct CompareArgs){start_word, words};
        printf("Start processing iteration %d...\n", i);
        if (pthread_create(&thread_ids[i%10], NULL, compare_word_thread, &args[i]) != 0) {
            exit(EXIT_FAILURE);
        }

        start_word = start_word->next;
        i++;
    }

    //for (int i = 0; i < n_words; i++) {
    //    printf("%s\n", words[i]);
    //}
    for (int j = 0; j < 10; j++) {
        pthread_join(thread_ids[j], NULL);
    }

    exit(EXIT_SUCCESS);

    return 0;

}