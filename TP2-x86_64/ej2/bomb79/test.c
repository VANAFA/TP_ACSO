#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read lines from a file (reimplementation of readlines)
char** readlines(int* line_count) {
    FILE* file = fopen("palabras.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open palabras.txt\n");
        exit(1);
    }
    
    // Allocate memory for lines (same size as in assembly: 0x17700)
    char** lines = malloc(0x17700);
    *line_count = 0;
    
    // Read lines from file
    char* line = NULL;
    size_t len = 0;
    size_t read;
    
    while ((read = getline(&line, &len, file)) != -1) {
        // Remove newline character
        if (read > 0 && line[read-1] == '\n') {
            line[read-1] = '\0';
        }
        
        // Skip comments and empty lines
        if (line[0] != '/' || line[1] != '/') {
            lines[*line_count] = strdup(line);
            (*line_count)++;
        }
    }
    
    fclose(file);
    if (line) free(line);
    
    return lines;
}

// Recreation of the cuenta function based on assembly analysis
int cuenta(char* input, char** lines, int start, int end) {
    if (start > end) {
        return 0;
    }
    
    // Calculate middle index using the same bit operations as in assembly
    int mid = ((start ^ end) >> 1) + (start & end);
    
    // Safety check for mid value
    if (mid < start || mid > end) {
        mid = (start + end) / 2;
    }
    
    // Get the middle word
    char* mid_word = lines[mid];
    
    // Get the first character of the middle word
    char first_char = mid_word[0];
    
    // Compare input with middle word
    int result = strcmp(input, mid_word);
    
    if (result == 0) {
        // Found the word, return ASCII value of first character
        return (int)first_char;
    } else if (result < 0) {
        // Input is less than mid_word, search left half
        if (mid == start) {
            // Can't narrow search further
            return first_char;
        }
        // Add first_char to result of left search
        return (int)first_char + cuenta(input, lines, start, mid - 1);
    } else {
        // Input is greater than mid_word, search right half
        if (mid == end) {
            // Can't narrow search further
            return first_char;
        }
        // Add first_char to result of right search
        return (int)first_char + cuenta(input, lines, mid + 1, end);
    }
}

int main() {
    int line_count;
    char** lines = readlines(&line_count);
    
    printf("Testing %d words from palabras.txt...\n", line_count);
    
    // Test each word to find which one gives exactly 799
    for (int i = 0; i < line_count; i++) {
        int result = cuenta(lines[i], lines, 0, line_count - 1);
        
        if (result == 799) {
            printf("Found word with result 799: '%s'\n", lines[i]); // pegar 999
        }
    }
    
    // Free memory
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    
    return 0;
}