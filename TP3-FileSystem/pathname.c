
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (fs == NULL || pathname == NULL) {
        return -1;
    }
    if (strcmp(pathname, "/") == 0) {
        return ROOT_INUMBER;
    }
    int currentInode = ROOT_INUMBER;
    char pathCopy[strlen(pathname) + 1];
    strcpy(pathCopy, pathname);
    char *path = pathCopy;
    if (*path == '/') {
        path++;
        if (*path == '\0') {
            return ROOT_INUMBER;
        }
    }
    char *component;
    char *saveptr;
    component = strtok_r(path, "/", &saveptr);

    while (component != NULL) {
        struct direntv6 dirEntry;
        if (directory_findname(fs, component, currentInode, &dirEntry) != 0) {
            return -1;
        }
        currentInode = dirEntry.d_inumber;    
        component = strtok_r(NULL, "/", &saveptr);
    }
    return currentInode;
}