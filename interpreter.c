#include "lexer.c"
#include "parser.c"
#include "utils.c"

// Função para executar um bloco de código
void execute_block(const char **input);

// Função para executar um comando
void execute_command(const char **input) {
    skip_whitespace(input);

    if (strncmp(*input, "print", 5) == 0) {
        (*input) += 5; // Avança "print"
        skip_whitespace(input);

        if (**input == '"') { // String literal
            (*input)++; // Avança a aspa inicial
            while (**input != '"' && **input != '\0') {
                putchar(**input);
                (*input)++;
            }
            if (**input == '"') {
                (*input)++; // Avança a aspa final
            }
            putchar('\n');
        } else if (isalpha(**input)) { // Variável
            char var = parse_variable(input);
            printf("%d\n", get_variable(var));
        }
    } else if (strncmp(*input, "input", 5) == 0) {
        (*input) += 5; // Avança "input"
        skip_whitespace(input);

        char var = parse_variable(input);
        printf("Digite o valor para %c: ", var);
        scanf("%d", &get_variable_ref(var));
        getchar(); // Limpa o buffer do teclado
    } else if (isalpha(**input)) { // Atribuição de variável
        char var = parse_variable(input);
        skip_whitespace(input);

        if (*input == '=') {
            input++; // Avança '='
            skip_whitespace(input);

            // Lê o valor
            if (isdigit(*input)) {
                set_variable(var, parse_number(input));
            } else if (isalpha(*input)) {
                char other_var = parse_variable(input);
                set_variable(var, get_variable(other_var));
            } else if (**input == '+' || **input == '-' || **input == '*' || **input == '/') {
                char op = **input;
                (*input)++;
                skip_whitespace(input);

                int right = 0;
                if (isdigit(**input)) {
                    right = parse_number(input);
                } else if (isalpha(**input)) {
                    char other_var = parse_variable(input);
                    right = get_variable(other_var);
                }

                switch (op) {
                    case '+': set_variable(var, get_variable(var) + right); break;
                    case '-': set_variable(var, get_variable(var) - right); break;
                    case '*': set_variable(var, get_variable(var) * right); break;
                    case '/':
                        if (right == 0) {
                            fprintf(stderr, "Erro: Divisão por zero!\n");
                            exit(1);
                        }
                        set_variable(var, get_variable(var) / right);
                        break;
                }
            }
        }
    }
}

// Função para executar um bloco de código
void execute_block(const char **input) {
    skip_whitespace(input);

    while (**input != '\0' && strncmp(*input, "end", 3) != 0) {
        execute_command(input);
        skip_whitespace(input);
    }

    if (strncmp(*input, "end", 3) == 0) {
        (*input) += 3; // Avança "end"
    }
}

// Função principal para interpretar o código
void interpret_code(const char *code) {
    const char *input = code;

    while (*input) {
        skip_whitespace(&input);

        if (strncmp(input, "if", 2) == 0) {
            input += 2; // Avança "if"
            skip_whitespace(&input);

            // Avalia a condição
            int condition = evaluate_condition(&input);

            skip_whitespace(&input);

            if (strncmp(input, "then", 4) == 0) {
                input += 4; // Avança "then"
                skip_whitespace(&input);

                // Executa o bloco se a condição for verdadeira
                if (condition) {
                    execute_block(&input);
                } else {
                    // Ignora o bloco se a condição for falsa
                    while (*input != '\0' && strncmp(input, "end", 3) != 0) {
                        input++;
                    }
                    if (strncmp(input, "end", 3) == 0) {
                        input += 3; // Avança "end"
                    }
                }
            }
        } else if (strncmp(input, "while", 5) == 0) {
            input += 5; // Avança "while"
            skip_whitespace(&input);

            const char *loop_start = input;

            while (1) {
                // Avalia a condição
                const char *condition_start = loop_start;
                int condition = evaluate_condition(&condition_start);

                if (!condition) {
                    break;
                }

                // Executa o bloco
                const char *block_start = condition_start;
                execute_block(&block_start);

                // Volta ao início do loop
                input = loop_start;
            }

            // Avança até o "end" do loop
            while (**input != '\0' && strncmp(input, "end", 3) != 0) {
                input++;
            }
            if (strncmp(input, "end", 3) == 0) {
                input += 3; // Avança "end"
            }
        } else {
            execute_command(&input);
        }

        skip_whitespace(&input);
    }
}
