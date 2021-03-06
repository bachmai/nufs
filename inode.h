// based on cs3650 starter code

#ifndef INODE_H
#define INODE_H

#include <fcntl.h>
#include <sys/stat.h>

// #include "pages.h"

#define DIRECT_PTRS 8

// 4096 / 8 / 8 --> 64 for inode only
typedef struct inode
{
    int refs;              // reference count
    int mode;              // permission & type
    int size;              // bytes
    int ptrs[DIRECT_PTRS]; // direct pointers -->
    int iptr;              // single indirect pointer
    time_t ctime;          // changed time
    time_t mtime;          // modified time
} inode;

void print_inode(inode *node);
inode *get_inode(int inum);
int alloc_inode();
void free_inode();
int grow_inode(inode *node, int size);
int shrink_inode(inode *node, int size);
int inode_get_pnum(inode *node, int fpn);
void init_inode(inode *node, int mode);
int get_mt_db(inode *node);

#endif
