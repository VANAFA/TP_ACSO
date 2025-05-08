#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"


/**
 * TODO complete the inode_iget function acording to unix v6 especifications
 * INODES_PER_BLOCK
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    // Validate input parameters
    if (inumber < 1 || inp == NULL || fs == NULL) {
        return -1;
    }
    int INODES_PER_BLOCK = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    
    // Calculate which block contains this inode
    int block = (inumber - 1) / INODES_PER_BLOCK + INODE_START_SECTOR;
    
    // Calculate the offset within the block
    int offset = (inumber - 1) % INODES_PER_BLOCK;
    
    // Buffer to hold the entire block
    struct inode inodes[INODES_PER_BLOCK];
    
    // Read the block from disk
    if (diskimg_readsector(fs->dfd, block, inodes) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }
    
    // Copy the relevant inode to the provided destination
    *inp = inodes[offset];
    
    return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    // Check if the inode is valid
    if (inp == NULL || fs == NULL || blockNum < 0) {
        return -1;
    }
    
    // Check if the block number is within the file's size
    int fileSize = inode_getsize(inp);
    int maxBlockNum = (fileSize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    if (blockNum >= maxBlockNum) {
        return -1; // Block number out of range
    }
    
    // Check if this is a large file
    if (inp->i_mode & ILARG) {
        // Large file format: 7 indirect and 1 double-indirect
        
        if (blockNum < 7 * 256) {
            // This is in the indirect blocks (each indirect block points to 256 data blocks)
            int indirectBlockIndex = blockNum / 256;
            int indirectOffset = blockNum % 256;
            
            // Get the indirect block number
            int indirectBlockNum = inp->i_addr[indirectBlockIndex];
            if (indirectBlockNum == 0) return 0; // Sparse file
            
            // Read the indirect block
            uint16_t indirectBlock[256];
            if (diskimg_readsector(fs->dfd, indirectBlockNum, indirectBlock) != DISKIMG_SECTOR_SIZE) {
                return -1;
            }
            
            return indirectBlock[indirectOffset];
        } else {
            // This is in the double-indirect block
            int doubleBlockOffset = blockNum - (7 * 256);
            if (doubleBlockOffset >= 256 * 256) return -1; // Beyond file size limit
            
            // Calculate indices for double indirection
            int doubleIndirectIndex = doubleBlockOffset / 256;
            int indirectOffset = doubleBlockOffset % 256;
            
            // Get the double-indirect block
            int doubleIndirectBlockNum = inp->i_addr[7];
            if (doubleIndirectBlockNum == 0) return 0; // Sparse file
            
            // Read the double-indirect block to get the indirect block number
            uint16_t doubleIndirectBlock[256];
            if (diskimg_readsector(fs->dfd, doubleIndirectBlockNum, doubleIndirectBlock) != DISKIMG_SECTOR_SIZE) {
                return -1;
            }
            
            int indirectBlockNum = doubleIndirectBlock[doubleIndirectIndex];
            if (indirectBlockNum == 0) return 0; // Sparse file
            
            // Read the indirect block
            uint16_t indirectBlock[256];
            if (diskimg_readsector(fs->dfd, indirectBlockNum, indirectBlock) != DISKIMG_SECTOR_SIZE) {
                return -1;
            }
            
            return indirectBlock[indirectOffset];
        }
    } else {
        // Small file format: 8 direct blocks
        if (blockNum > 7) return -1; // Beyond the maximum direct blocks
        
        return inp->i_addr[blockNum];
    }
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
