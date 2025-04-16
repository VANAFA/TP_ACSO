#include <stdio.h>
#include <stdlib.h>

// This represents the array at address 0x4cde20 (labeled as array.0)
// We need to populate this with the correct values from the bomb
int array_0[16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

// This function simulates the path-following logic in phase_4
void find_solutions() {
    // We'll try every possible starting index
    for (int start_idx = 0; start_idx < 15; start_idx++) {
        int idx = start_idx;
        int steps = 0;
        int sum = 0;
        
        printf("Testing start index %d: ", start_idx);
        
        // Follow the path until we reach 15 or exceed 5 steps
        while (idx != 15 && steps < 6) {
            printf("%d→", idx);
            sum += idx;
            idx = array_0[idx]; // Jump to next position
            steps++;
        }
        
        // Check if we finished in exactly 5 steps and reached 15
        if (steps == 5 && idx == 15) {
            sum += idx; // Add the final position
            printf("%d (Final sum: %d)\n", idx, sum);
            printf("SOLUTION FOUND: %d %d\n", start_idx, sum);
        } else {
            printf(" (Invalid path: %d steps, ended at %d)\n", steps, idx);
        }
    }
}

int main() {
    printf("Phase 4 Solver - Finding inputs that will defuse the bomb\n");
    printf("=======================================================\n");
    
    // The array_0 values might not be correct - we'd need to extract them
    // from the running bomb or through further debugging.
    printf("Warning: Using placeholder array values - may not match actual bomb!\n\n");
    
    printf("Searching for valid paths...\n");
    find_solutions();
    
    return 0;
}