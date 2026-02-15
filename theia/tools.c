#include "tools.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

String newstr(const char* data){
    size_t len = strlen(data);
    return (String){
        .data = strndup(data, len),
        .size = len,
        .cap = len + 1
    };
}
void strapp(String* str, const int c){
    if(str == NULL){
        printf("String are null\n");
        exit(EXIT_FAILURE);
    }
    if(str->size+1 >= str->cap){
        str->cap *= 2;
        char* tmp = (char*)realloc(str->data, str->cap);
        if(!tmp) {
            printf("String memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        str->data = tmp;
    }
    str->data[str->size++] = c;
    str->data[str->size] = '\0';
}
void strundo(String* str){
    if(!str->size) return;
    str->data[--str->size] = '\0';
}

void strclean(String* str){
    str->size = 0;
    str->data[0] = '\0';
}

void strdel(String* str){
    free(str->data);
    str->data = NULL;
    str->size = 0;
    str->cap = 0;
}
