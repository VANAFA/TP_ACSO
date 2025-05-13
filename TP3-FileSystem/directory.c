#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DIRSIZ sizeof(((struct direntv6 *)0)->d_name)

int directory_findname(struct unixfilesystem *fs, const char *name,
        int dirinumber, struct direntv6 *dirEnt) {
    if (fs == NULL || name == NULL || dirEnt == NULL || dirinumber < 1) {
        return -1;
    }
    struct inode dirInode;
    if (inode_iget(fs, dirinumber, &dirInode) != 0) {
        return -1;
    }
    if ((dirInode.i_mode & IFMT) != IFDIR) {
        return -1;
    }
    int dirSize = inode_getsize(&dirInode);
    int numBlocks = (dirSize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    char buffer[DISKIMG_SECTOR_SIZE];
    for (int blockNum = 0; blockNum < numBlocks; blockNum++) {
        int bytesRead = file_getblock(fs, dirinumber, blockNum, buffer);
        if (bytesRead <= 0) {
            continue;
        }
        int numEntries = bytesRead / sizeof(struct direntv6);
        struct direntv6 *entries = (struct direntv6 *)buffer;
        for (int i = 0; i < numEntries; i++) {
            if (entries[i].d_inumber == 0) {
                continue;
            }
            if (strncmp(entries[i].d_name, name, DIRSIZ) == 0) {
                *dirEnt = entries[i];
                return 0;
            }
        }
    }
    return -1;
}
