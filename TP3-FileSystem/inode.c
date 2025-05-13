#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"


int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1 || inp == NULL || fs == NULL) {
        return -1;
    }
    int INODES_PER_BLOCK = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int block = (inumber - 1) / INODES_PER_BLOCK + INODE_START_SECTOR;
    int offset = (inumber - 1) % INODES_PER_BLOCK;
    
    struct inode inodes[INODES_PER_BLOCK];
    
    if (diskimg_readsector(fs->dfd, block, inodes) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }
    *inp = inodes[offset];
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (inp == NULL || fs == NULL || blockNum < 0) {
        return -1;
    }
    int fileSize = inode_getsize(inp);
    int maxBlockNum = (fileSize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    if (blockNum >= maxBlockNum) {
        return -1;
    }
    if (inp->i_mode & ILARG) {   
        if (blockNum < 7 * 256) {
            int indirectBlockIndex = blockNum / 256;
            int indirectOffset = blockNum % 256;
            int indirectBlockNum = inp->i_addr[indirectBlockIndex];
            if (indirectBlockNum == 0) return 0;
            
            uint16_t indirectBlock[DISKIMG_SECTOR_SIZE/sizeof(uint16_t)];
            if (diskimg_readsector(fs->dfd, indirectBlockNum, indirectBlock) != DISKIMG_SECTOR_SIZE) {
                return -1;
            }
            return indirectBlock[indirectOffset];
        } else {
            int doubleBlockOffset = blockNum - (7 * 256);
            if (doubleBlockOffset >= 256 * 256) return -1;
            
            int doubleIndirectIndex = doubleBlockOffset / 256;
            int indirectOffset = doubleBlockOffset % 256;
            int doubleIndirectBlockNum = inp->i_addr[7];
            if (doubleIndirectBlockNum == 0) return 0;
            
            uint16_t doubleIndirectBlock[256];
            if (diskimg_readsector(fs->dfd, doubleIndirectBlockNum, doubleIndirectBlock) != DISKIMG_SECTOR_SIZE) {
                return -1;
            }
            int indirectBlockNum = doubleIndirectBlock[doubleIndirectIndex];
            if (indirectBlockNum == 0) return 0;
            
            uint16_t indirectBlock[256];
            if (diskimg_readsector(fs->dfd, indirectBlockNum, indirectBlock) != DISKIMG_SECTOR_SIZE) {
                return -1;
            }
            return indirectBlock[indirectOffset];
        }
    } else {
        if (blockNum > 7) return -1;
        return inp->i_addr[blockNum];
    }
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
