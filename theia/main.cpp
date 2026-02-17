#include <iostream>
#include <vector>
#include <stdio.h>
#include "lexer.h"

int main() {
    Lexer lexer = lexer_init("../../Content/backup.th");
    tokenize(&lexer);
	printf("tokenized");
    size_t i = 0, line = 0;
	printf("%lu : ", line);
    while(lexer.vec.tokens[i].Class != END_OF_FILE){
        while(line < lexer.vec.tokens[i].line){
            printf("\n%lu : ", ++line);
        }
        //std::cout << lexer.vec.tokens[i].data << " ";
        std::cout << "[" << lexer.vec.tokens[i].Class << "-" << lexer.vec.tokens[i].type << "] ";
        i++;
    }
    printf("\nend of program\n");
    lexer_close(&lexer);
    return 0;
}
