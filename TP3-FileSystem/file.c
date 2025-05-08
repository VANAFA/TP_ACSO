#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    // Validate input parameters
    if (fs == NULL || buf == NULL || inumber < 1 || blockNum < 0) {
        return -1;
    }
    
    // Get the inode for this file
    struct inode in;
    if (inode_iget(fs, inumber, &in) != 0) {
        return -1;
    }
    
    // Get the file size to determine valid bytes in the block
    int fileSize = inode_getsize(&in);
    
    // If the requested block is beyond the file size, return 0
    if (blockNum * DISKIMG_SECTOR_SIZE >= fileSize) {
        return 0;
    }
    
    // Calculate valid bytes in this block
    int validBytes = DISKIMG_SECTOR_SIZE;
    if ((blockNum + 1) * DISKIMG_SECTOR_SIZE > fileSize) {
        validBytes = fileSize - blockNum * DISKIMG_SECTOR_SIZE;
    }
    
    // Look up the physical block number
    int physicalBlockNum = inode_indexlookup(fs, &in, blockNum);
    if (physicalBlockNum <= 0) {
        // Handle sparse files (block with value 0)
        if (physicalBlockNum == 0) {
            // Zero out the buffer for sparse blocks
            memset(buf, 0, validBytes);
            return validBytes;
        }
        return -1;
    }
    
    // Read the block from disk
    if (diskimg_readsector(fs->dfd, physicalBlockNum, buf) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }
    
    return validBytes;
}

