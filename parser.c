#include "utils.c"

// Função para avaliar uma condição
int evaluate_condition(const char **input) {
    skip_whitespace(input);

    // Lê a variável ou número à esquerda
    int left = 0;
    if (isalpha(**input)) {
        char var = parse_variable(input);
        left = get_variable(var);
    } else if (isdigit(**input)) {
        left = parse_number(input);
    }

    skip_whitespace(input);

    // Lê o operador de comparação
    char op = **input;
    if (op != '>' && op != '<' && op != '=') {
        fprintf(stderr, "Erro: Operador inválido '%c'!\n", op);
        exit(1);
    }
    (*input)++;

    skip_whitespace(input);

    // Lê a variável ou número à direita
    int right = 0;
    if (isalpha(**input)) {
        char var = parse_variable(input);
        right = get_variable(var);
    } else if (isdigit(**input)) {
        right = parse_number(input);
    }

    // Avalia a condição
    switch (op) {
        case '>': return left > right;
        case '<': return left < right;
        case '=': return left == right;
        default: return 0;
    }
}
