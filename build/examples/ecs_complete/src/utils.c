#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Utility functions for Pevi editor
char* load_file_content(const char* filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) return NULL;
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(length + 1);
    fread(content, 1, length, file);
    content[length] = '\0';
    
    fclose(file);
    return content;
}