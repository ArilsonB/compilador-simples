#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

// Tamanho máximo da entrada
#define MAX_INPUT_SIZE 256

// Variáveis globais para armazenar o estado do programa
int variables[26]; // Armazena valores das variáveis (a-z)

// Função para ignorar espaços em branco
void skip_whitespace(const char **input)
{
    while (isspace(**input))
    {
        (*input)++;
    }
}

// Função para analisar um número inteiro
int parse_number(const char **input)
{
    int value = 0;
    while (isdigit(**input))
    {
        value = value * 10 + (**input - '0');
        (*input)++;
    }
    return value;
}

// Função para analisar uma variável (letras de a-z)
char parse_variable(const char **input)
{
    char var = **input;
    if (var >= 'a' && var <= 'z')
    {
        (*input)++;
        return var;
    }
    fprintf(stderr, "Erro: Variável inválida '%c'!\n", var);
    exit(1);
}

// Função para avaliar uma condição
int evaluate_condition(const char **input)
{
    skip_whitespace(input);

    // Lê a variável ou número à esquerda
    int left = 0;
    if (isalpha(**input))
    {
        char var = parse_variable(input);
        left = variables[var - 'a'];
    }
    else if (isdigit(**input))
    {
        left = parse_number(input);
    }

    skip_whitespace(input);

    // Lê o operador de comparação
    char op = **input;
    if (op != '>' && op != '<' && op != '=')
    {
        fprintf(stderr, "Erro: Operador inválido '%c'!\n", op);
        exit(1);
    }
    (*input)++;

    skip_whitespace(input);

    // Lê a variável ou número à direita
    int right = 0;
    if (isalpha(**input))
    {
        char var = parse_variable(input);
        right = variables[var - 'a'];
    }
    else if (isdigit(**input))
    {
        right = parse_number(input);
    }

    // Avalia a condição
    switch (op)
    {
    case '>':
        return left > right;
    case '<':
        return left < right;
    case '=':
        return left == right;
    default:
        return 0;
    }
}

// Função para executar um bloco de código
void execute_command(const char **input)
{
    skip_whitespace(input);

    if (strncmp(*input, "print", 5) == 0)
    {
        (*input) += 5; // Avança "print"
        skip_whitespace(input);

        if (**input == '(')
        {             // String literal
            (*input)++; // Avança '('
            if (**input == '"')
            {
                (*input)++;
                while (**input != '"' && **input != '\0')
                {
                    putchar(**input);
                    (*input)++;
                }
                if (**input == '"')
                {
                    (*input)++; // Avança a aspa final
                }
                if (**input == ')')
                {
                    (*input)++;
                }
                putchar('\n');
            }
        }
    }else if(strncmp(*input, "input", 5) == 0){
        (*input) += 5; // Avança "input"
        skip_whitespace(input);
        if(**input == '('){
            (*input)++;
            skip_whitespace(input);
            if(isalpha(**input)){
                char var = parse_variable(input);
                skip_whitespace(input);
                printf("Digite um valor para %c: ", var);
                scanf("%d", &variables[var - 'a']);
                if(**input == ')'){
                    (*input)++;
                }
                putchar('\n');
            }
        }
    }
    else if (isalpha(**input))
    {
        char var = parse_variable(input);
        skip_whitespace(input);
        if (**input == '=')
        {
            (*input)++;
            skip_whitespace(input);
            if (isdigit(**input))
            {
                variables[var - 'a'] = parse_number(input);
            }
            else if (isalpha(**input))
            {
                char other_var = parse_variable(input);
                variables[var - 'a'] = variables[other_var - 'a'];
            }
        }
    }
}

// Função principal para interpretar o código
void interpret_code(const char *code)
{
    const char *input = code;

    while (*input)
    {
        skip_whitespace(&input);

        if (strncmp(input, "if", 2) == 0)
        {
            input += 2; // Avança "if"
            skip_whitespace(&input);

            // Avalia a condição
            int condition = evaluate_condition(&input);
            skip_whitespace(&input);

            if (strncmp(input, ":", 1) == 0)
            {
                input += 1; // Avança ":"
                skip_whitespace(&input);

                // Executa o bloco se a condição for verdadeira
                if (condition)
                {
                    execute_command(&input);
                }
                else
                {
                    // Ignora o bloco se a condição for falsa
                    while (*input != '\0' && strncmp(input, "end", 3) != 0)
                    {
                        input++;
                    }
                    if (strncmp(input, "end", 3) == 0)
                    {
                        input += 3; // Avança "end"
                    }
                }
            }
        }
        else
        {
            execute_command(&input);
        }

        skip_whitespace(&input);
    }
}

int main(int argc, char *argv[])
{
    char code[MAX_INPUT_SIZE];

    if (argc == 2)
    {
        // Abre o arquivo para leitura
        FILE *file = fopen(argv[1], "r");
        if (!file)
        {
            perror("Erro ao abrir o arquivo");
            return 1;
        }

        // Lê o arquivo linha por linha
        while (fgets(code, sizeof(code), file))
        {
            // Remove o caractere de nova linha
            code[strcspn(code, "\n")] = '\0';
            if (strcmp(code, "exit") == 0)
            {
                break;
            }
            // Interpreta a linha
            interpret_code(code);
        }

        // Fecha o arquivo
        fclose(file);
    }
    else
    {
        printf("Digite seu código (linha por linha, digite 'exit' para sair):\n");
        while (1)
        {
            printf("> ");
            fgets(code, sizeof(code), stdin);

            // Remove o caractere de nova linha
            code[strcspn(code, "\n")] = '\0';
            if (strcmp(code, "exit") == 0)
            {
                break;
            }
            interpret_code(code);
        }
    }

    return 0;
}
