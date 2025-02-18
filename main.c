#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


// Função para ignorar espaços em branco
void skip_whitespace(const char **input) {
    while (isspace(**input)) {
        (*input)++;
    }
}

// Função para analisar um número inteiro
int parse_number(const char **input) {
    int value = 0;
    while (isdigit(**input)) {
        value = value * 10 + (**input - '0');
        (*input)++;
    }
    return value;
}

// Função principal para avaliar a expressão
int evaluate_expression(const char *input) {
    int result = 0;
    char operator = '+';

    while (*input) {
        skip_whitespace(&input);

        if (*input == '\0') {
            break;
        }

        // Parse o número
        int number = parse_number(&input);

        // Aplica a operação anterior
        switch (operator) {
            case '+': result += number; break;
            case '-': result -= number; break;
            case '*': result *= number; break;
            case '/':
                if (number == 0) {
                    fprintf(stderr, "Erro: Divisão por zero!\n");
                    exit(1);
                }
                result /= number;
                break;
            default:
                fprintf(stderr, "Erro: Operador inválido '%c'!\n", operator);
                exit(1);
        }

        skip_whitespace(&input);

        // Verifica o próximo operador
        if (*input == '\0') {
            break;
        }

        operator = *input;
        input++;
    }

    return result;
}

int main() {
    char input[256];

    printf("Digite uma expressão aritmética simples (+, -, *, /):\n");
    fgets(input, sizeof(input), stdin);

    int result = evaluate_expression(input);
    printf("Resultado: %d\n", result);

    return 0;
}
