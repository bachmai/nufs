// based on cs3650 starter code

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <bsd/string.h>
#include <assert.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include "storage.h"

// implementation for: man 2 access
// Checks if a file exists.
int nufs_access(const char *path, int mask)
{
    int rv = 0;
    printf("access(%s, %04o) -> %d\n", path, mask, rv);
    return rv;
}

// implementation for: man 2 stat
// gets an object's attributes (type, permissions, size, etc)
int nufs_getattr(const char *path, struct stat *st)
{
    // Get the stat of the file based on the given path
    int rv = storage_stat(path, st);
    printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode, st->st_size);
    return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi)
{
    int rv = storage_readdir(path, buf, filler, offset);
    printf("readdir(%s) -> %d\n", path, rv);
    return rv;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
int nufs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    puts("IN MAKE NODE");
    int rv = storage_mknod(path, mode);
    printf("mknod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int nufs_mkdir(const char *path, mode_t mode)
{
    int rv = storage_mkdir(path, mode);
    printf("mkdir(%s) -> %d\n", path, rv);
    return rv;
}

// Removes the given filename from the filesystem
int nufs_unlink(const char *path)
{
    int rv = storage_unlink(path);
    printf("unlink(%s) -> %d\n", path, rv);
    return rv;
}

// New hard link to existing file
int nufs_link(const char *from, const char *to)
{
    int rv = storage_link(from, to);
    printf("link(%s => %s) -> %d\n", from, to, rv);
    return rv;
}

// Remove directory
int nufs_rmdir(const char *path)
{
    int rv = storage_rmdir(path);
    printf("rmdir(%s) -> %d\n", path, rv);
    return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int nufs_rename(const char *from, const char *to)
{
    int rv = storage_rename(from, to);
    printf("rename(%s => %s) -> %d\n", from, to, rv);
    return rv;
}

// Change access permissions of the file to the given one
int nufs_chmod(const char *path, mode_t mode)
{
    int rv = storage_chmod(path, mode);
    printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

// Trucate the given file to the fiven length
int nufs_truncate(const char *path, off_t size)
{
    int rv = storage_truncate(path, size);
    printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
    return rv;
}

// this is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
int nufs_open(const char *path, struct fuse_file_info *fi)
{
    // Can basically return 0
    // Used nufs_access for code consistency tho
    int rv = nufs_access(path, 0);
    printf("open(%s) -> %d\n", path, rv);
    return rv;
}

// Actually read data
int nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int rv = 0;
    const char *data = storage_data(path); // get data

    rv = strlen(data) + 1; // for an eof
    if (data == 0)
    {
        rv = 0;
    }
    // Actual data length is greater than given size
    if (size < rv)
    {
        rv = size;
    }
    printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
    // strlcpy for concat -- ?
    strncpy(buf, data, rv); // no more than rv
    return rv;
}

// Actually write data
int nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int rv = storage_write(path, buf, size, offset);
    printf("write(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
    return rv;
}

// Update the timestamps on a file or directory.
int nufs_utimens(const char *path, const struct timespec ts[2])
{
    int rv = storage_set_time(path, ts);
    printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n",
           path, ts[0].tv_sec, ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
    return rv;
}

// Extended operations
int nufs_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi,
               unsigned int flags, void *data)
{
    int rv = -1;
    printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
    return rv;
}

void nufs_init_ops(struct fuse_operations *ops)
{
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access = nufs_access;
    ops->getattr = nufs_getattr;
    ops->readdir = nufs_readdir;
    ops->mknod = nufs_mknod;
    ops->mkdir = nufs_mkdir;
    ops->link = nufs_link;
    ops->unlink = nufs_unlink;
    ops->rmdir = nufs_rmdir;
    ops->rename = nufs_rename;
    ops->chmod = nufs_chmod;
    ops->truncate = nufs_truncate;
    ops->open = nufs_open;
    ops->read = nufs_read;
    ops->write = nufs_write;
    ops->utimens = nufs_utimens;
    ops->ioctl = nufs_ioctl;
};

struct fuse_operations nufs_ops;

int main(int argc, char *argv[])
{
    assert(argc > 2 && argc < 6);
    storage_init(argv[--argc]);
    nufs_init_ops(&nufs_ops); // initialize operations
    return fuse_main(argc, argv, &nufs_ops, NULL);
}
