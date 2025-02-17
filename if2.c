#include <stdio.h>
#include <string.h>
#include "interpreter.c"

#define MAX_INPUT_SIZE 256

int main() {
    char code[MAX_INPUT_SIZE];

    printf("Digite seu código (linha por linha, digite 'exit' para sair):\n");
    while (1) {
        printf("> ");
        fgets(code, sizeof(code), stdin);

        // Remove o caractere de nova linha
        code[strcspn(code, "\n")] = '\0';

        if (strcmp(code, "exit") == 0) {
            break;
        }

        interpret_code(code);
    }

    return 0;
}
