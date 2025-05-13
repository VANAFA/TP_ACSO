#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    if (fs == NULL || buf == NULL || inumber < 1 || blockNum < 0) {
        return -1;
    }
    struct inode in;
    if (inode_iget(fs, inumber, &in) != 0) {
        return -1;
    }
    int fileSize = inode_getsize(&in);
    if (blockNum * DISKIMG_SECTOR_SIZE >= fileSize) {
        return 0;
    }
    int validBytes = DISKIMG_SECTOR_SIZE;
    if ((blockNum + 1) * DISKIMG_SECTOR_SIZE > fileSize) {
        validBytes = fileSize - blockNum * DISKIMG_SECTOR_SIZE;
    }
    int physicalBlockNum = inode_indexlookup(fs, &in, blockNum);
    if (physicalBlockNum <= 0) {
        if (physicalBlockNum == 0) {
            memset(buf, 0, validBytes);
            return validBytes;
        }
        return -1;
    }
    if (diskimg_readsector(fs->dfd, physicalBlockNum, buf) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }
    return validBytes;
}

