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

// Função para executar um bloco de código e gerar código Assembly equivalente
void execute_and_generate_assembly(const char **input, FILE *output_file)
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
                fprintf(output_file, "    # Print string\n");
                fprintf(output_file, "    mov $message, %%rdi\n");
                fprintf(output_file, "    call printf\n");
                fprintf(output_file, "message:\n");
                fprintf(output_file, "    .asciz \"");
                while (**input != '"' && **input != '\0')
                {
                    fputc(**input, output_file);
                    (*input)++;
                }
                fprintf(output_file, "\\n\"\n");
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
        (*input) += 5; // Avança "input"'
        skip_whitespace(input);
        if (**input == '(')
        {
            (*input)++;
            skip_whitespace(input);
            if (isalpha(**input))
            {
                char var = parse_variable(input);
                fprintf(output_file, "    # Input integer\n");
                fprintf(output_file, "    lea %c(%%rip), %%rsi\n", var);
                fprintf(output_file, "    mov $format_input, %%rdi\n");
                fprintf(output_file, "    xor %%rax, %%rax\n");
                fprintf(output_file, "    call scanf\n");
                fprintf(output_file, "format_input:\n");
                fprintf(output_file, "    .asciz \"%%d\"\n");
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
                fprintf(output_file, "    # Assign constant to variable\n");
                fprintf(output_file, "    mov $%d, %c(%%rip)\n", value, var);
            }
            else if (isalpha(**input))
            {
                char other_var = parse_variable(input);
                variables[var - 'a'] = variables[other_var - 'a'];
                fprintf(output_file, "    # Assign variable to variable\n");
                fprintf(output_file, "    mov %c(%%rip), %%rax\n", other_var);
                fprintf(output_file, "    mov %%rax, %c(%%rip)\n", var);
            }
        }
    }
}

// Função principal para interpretar o código e gerar código Assembly equivalente
void interpret_and_generate_assembly(const char *code, FILE *output_file)
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
                    execute_and_generate_assembly(&input, output_file);
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
            execute_and_generate_assembly(&input, output_file);
        }
        skip_whitespace(&input);
    }
}

// Função para compilar o código Assembly gerado
void assemble_and_link_code(const char *assembly_file)
{
    char assemble_command[512];
    char link_command[512];

    // Assemble the assembly code into an object file
    snprintf(assemble_command, sizeof(assemble_command), "as %s -o %s.o", assembly_file, assembly_file);
    printf("Assembling '%s'...\n", assembly_file);
    int assemble_result = system(assemble_command);

    if (assemble_result != 0)
    {
        fprintf(stderr, "Erro ao montar o arquivo '%s'.\n", assembly_file);
        return;
    }

    // Link the object file into an executable
    snprintf(link_command, sizeof(link_command), "gcc %s.o -o %s.out", assembly_file, assembly_file);
    printf("Linking '%s.o'...\n", assembly_file);
    int link_result = system(link_command);

    if (link_result == 0)
    {
        printf("Compilação bem-sucedida! Executável gerado: '%s.out'\n", assembly_file);
    }
    else
    {
        fprintf(stderr, "Erro ao vincular o arquivo '%s.o'.\n", assembly_file);
    }
}

int main(int argc, char *argv[])
{
    char code[MAX_INPUT_SIZE];

    // Cria um arquivo temporário para armazenar o código Assembly gerado
    const char *temp_file_name = "generated_code.s";
    FILE *output_file = fopen(temp_file_name, "w");
    if (!output_file)
    {
        perror("Erro ao criar arquivo temporário");
        return 1;
    }

    // Escreve o cabeçalho do arquivo Assembly
    fprintf(output_file, ".section .data\n");
    fprintf(output_file, ".section .text\n");
    fprintf(output_file, ".globl _start\n");
    fprintf(output_file, "_start:\n");

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
            // Interpreta a linha e gera código Assembly equivalente
            interpret_and_generate_assembly(code, output_file);
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
            interpret_and_generate_assembly(code, output_file);
        }
    }

    // Finaliza o arquivo Assembly
    fprintf(output_file, "    # Exit program\n");
    fprintf(output_file, "    mov $60, %%rax\n"); // syscall number for exit
    fprintf(output_file, "    xor %%rdi, %%rdi\n");
    fprintf(output_file, "    syscall\n");
    fclose(output_file);

    // Monta e vincula o código Assembly gerado
    assemble_and_link_code(temp_file_name);

    return 0;
}
