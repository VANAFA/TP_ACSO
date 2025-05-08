#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DIRSIZ 14 // Maximum size of a directory entry name

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
        int dirinumber, struct direntv6 *dirEnt) {
    
    // Check input parameters
    if (fs == NULL || name == NULL || dirEnt == NULL || dirinumber < 1) {
        return -1;
    }
    
    // Get the inode for the directory
    struct inode dirInode;
    if (inode_iget(fs, dirinumber, &dirInode) != 0) {
        return -1;
    }
    
    // Verify it's a directory
    if ((dirInode.i_mode & IFMT) != IFDIR) {
        return -1; // Not a directory
    }
    
    // Get directory size
    int dirSize = inode_getsize(&dirInode);
    int numBlocks = (dirSize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    
    // Buffer for reading directory blocks
    char buffer[DISKIMG_SECTOR_SIZE];
    
    // Iterate through each block in the directory
    for (int blockNum = 0; blockNum < numBlocks; blockNum++) {
        // Read current directory block
        int bytesRead = file_getblock(fs, dirinumber, blockNum, buffer);
        if (bytesRead <= 0) {
            continue; // Skip this block if read error
        }
        
        // Calculate number of directory entries in this block
        int numEntries = bytesRead / sizeof(struct direntv6);
        struct direntv6 *entries = (struct direntv6 *)buffer;
        
        // Check each entry in the block
        for (int i = 0; i < numEntries; i++) {
            // Skip empty inodes (inode 0 means unused entry)
            if (entries[i].d_inumber == 0) {
                continue;
            }
            
            // Compare the name
            if (strncmp(entries[i].d_name, name, DIRSIZ) == 0) {
                // Found the entry! Copy it to the provided structure
                *dirEnt = entries[i];
                return 0;
            }
        }
    }
    
    // File not found in directory
    return -1;
}
