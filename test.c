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

// Função para executar um bloco de código e gerar código C equivalente
void execute_and_generate_code(const char **input, FILE *output_file)
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
                fprintf(output_file, "printf(\"");
                while (**input != '"' && **input != '\0')
                {
                    fputc(**input, output_file);
                    (*input)++;
                }
                fprintf(output_file, "\\n\");\n");
                if (**input == '"')
                {
                    (*input)++; // Avança a aspa final
                }
                if (**input == ')')
                {
                    (*input)++;
                }
            }
        }
    }
    else if (strncmp(*input, "input", 5) == 0)
    {
        (*input) += 5; // Avança "input"
        skip_whitespace(input);
        if (**input == '(')
        {
            (*input)++;
            skip_whitespace(input);
            if (isalpha(**input))
            {
                char var = parse_variable(input);
                fprintf(output_file, "scanf(\"%%d\", &%c);\n", var);
                if (**input == ')')
                {
                    (*input)++;
                }
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
                int value = parse_number(input);
                variables[var - 'a'] = value;
                fprintf(output_file, "%c = %d;\n", var, value);
            }
            else if (isalpha(**input))
            {
                char other_var = parse_variable(input);
                variables[var - 'a'] = variables[other_var - 'a'];
                fprintf(output_file, "%c = %c;\n", var, other_var);
            }
        }
    }
}

// Função principal para interpretar o código e gerar código C equivalente
void interpret_and_generate_code(const char *code, FILE *output_file)
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
                    execute_and_generate_code(&input, output_file);
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
            execute_and_generate_code(&input, output_file);
        }
        skip_whitespace(&input);
    }
}

// Função para compilar o código gerado
void compile_generated_code(const char *source_file)
{
    char compile_command[512];
    snprintf(compile_command, sizeof(compile_command), "gcc %s -o %s.out", source_file, source_file);

    printf("Compilando '%s'...\n", source_file);
    int result = system(compile_command);

    if (result == 0)
    {
        printf("Compilação bem-sucedida! Executável gerado: '%s.out'\n", source_file);
    }
    else
    {
        fprintf(stderr, "Erro ao compilar o arquivo '%s'.\n", source_file);
    }
}

int main(int argc, char *argv[])
{
    char code[MAX_INPUT_SIZE];

    // Cria um arquivo temporário para armazenar o código C gerado
    const char *temp_file_name = "generated_code.c";
    FILE *output_file = fopen(temp_file_name, "w");
    if (!output_file)
    {
        perror("Erro ao criar arquivo temporário");
        return 1;
    }

    // Escreve o cabeçalho do arquivo C
    fprintf(output_file, "#include <stdio.h>\n\nint main() {\n");

    if (argc == 2)
    {
        // Abre o arquivo para leitura
        FILE *file = fopen(argv[1], "r");
        if (!file)
        {
            perror("Erro ao abrir o arquivo");
            fclose(output_file);
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
            // Interpreta a linha e gera código C equivalente
            interpret_and_generate_code(code, output_file);
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
            interpret_and_generate_code(code, output_file);
        }
    }

    // Finaliza o arquivo C
    fprintf(output_file, "return 0;\n}\n");
    fclose(output_file);

    // Compila o código gerado
    compile_generated_code(temp_file_name);

    return 0;
}
