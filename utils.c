int variables[26] = {0};

int get_variable(char var) {
    return variables[var - 'a'];
}

int* get_variable_ref(char var) {
    return &variables[var - 'a'];
}

void set_variable(char var, int value) {
    variables[var - 'a'] = value;
}
