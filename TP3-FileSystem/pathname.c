
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    // Check input parameters
    if (fs == NULL || pathname == NULL) {
        return -1;
    }
    
    // Special case: root directory
    if (strcmp(pathname, "/") == 0) {
        return ROOT_INUMBER;
    }
    
    // Start from root directory for absolute paths, or current dir for relative paths
    int currentInode = ROOT_INUMBER;
    
    // Make a copy of the pathname that we can modify
    char pathCopy[strlen(pathname) + 1];
    strcpy(pathCopy, pathname);
    
    // Skip leading '/' if present
    char *path = pathCopy;
    if (*path == '/') {
        path++;
    }
    
    // Tokenize the path and process each component
    char *component;
    char *saveptr;
    component = strtok_r(path, "/", &saveptr);
    
    while (component != NULL) {
        // Find this component in the current directory
        struct direntv6 dirEntry;
        if (directory_findname(fs, component, currentInode, &dirEntry) != 0) {
            return -1; // Component not found
        }
        
        // Move to the next inode
        currentInode = dirEntry.d_inumber;
        
        // Get the next component
        component = strtok_r(NULL, "/", &saveptr);
    }
    
    return currentInode;
}