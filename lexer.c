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

// Função para analisar uma variável (letras de a-z)
char parse_variable(const char **input) {
    char var = **input;
    if (var >= 'a' && var <= 'z') {
        (*input)++;
        return var;
    }
    fprintf(stderr, "Erro: Variável inválida '%c'!\n", var);
    exit(1);
}
