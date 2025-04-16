#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** readlines(int* line_count) {
    FILE* file = fopen("palabras.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open palabras.txt\n");
        exit(1);
    }
    
    char** lines = malloc(0x17700);
    *line_count = 0;
    
    char* line = NULL;
    size_t len = 0;
    size_t read;
    
    while ((read = getline(&line, &len, file)) != -1) {
        if (read > 0 && line[read-1] == '\n') {
            line[read-1] = '\0';
        }
        
        if (line[0] != '/' || line[1] != '/') {
            lines[*line_count] = strdup(line);
            (*line_count)++;
        }
    }
    
    fclose(file);
    if (line) free(line);
    
    return lines;
}

int cuenta(char* input, char** lines, int start, int end) {
    if (start > end) {
        return 0;
    }
    
    int mid = ((start ^ end) >> 1) + (start & end);
    
    if (mid < start || mid > end) {
        mid = (start + end) / 2;
    }
    
    char* mid_word = lines[mid];
    
    char first_char = mid_word[0];
    
    int result = strcmp(input, mid_word);
    
    if (result == 0) {
        return (int)first_char;
    } else if (result < 0) {
        if (mid == start) {
            return first_char;
        }
        return (int)first_char + cuenta(input, lines, start, mid - 1);
    } else {
        if (mid == end) {
            return first_char;
        }
        return (int)first_char + cuenta(input, lines, mid + 1, end);
    }
}

int main() {
    int line_count;
    char** lines = readlines(&line_count);
    
    printf("Testing %d words from palabras.txt...\n", line_count);
    
    for (int i = 0; i < line_count; i++) {
        int result = cuenta(lines[i], lines, 0, line_count - 1);
        
        if (result == 799) {
            printf("Found word with result 799: '%s'\n", lines[i]);
        }
    }
    
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    
    return 0;
}
