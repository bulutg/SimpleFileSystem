#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "simplefs.h"

#define DIR_ENTRY_SIZE 128
#define DIR_BLOCK_COUNT 4
#define FCB_ENTRY_SIZE 128
#define FCB_BLOCK_COUNT 4

// Global Variables =======================================
int vdisk_fd; // Global virtual disk file descriptor. Global within the library.
              // Will be assigned with the vsfs_mount call.
              // Any function in this file can use this.
              // Applications will not use  this directly. 
// ========================================================


typedef struct rootdir {
	char name[128];
	int size;
	int blk;
	int isAvailable;
} rtdir;

typedef struct fcbEntry{
	int dir;
	int nextblk;
} fcb;

// read block k from disk (virtual disk) into buffer block.
// size of the block is BLOCKSIZE.
// space for block must be allocated outside of this function.
// block numbers start from 0 in the virtual disk. 
int read_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = read (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	printf ("read error\n");
	return -1;
    }
    return (0); 
}

// write block k into the virtual disk. 
int write_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = write (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	printf ("write error\n");
	return (-1);
    }
    return 0; 
}


/**********************************************************************
   The following functions are to be called by applications directly. 
***********************************************************************/

// this function is partially implemented.
int create_format_vdisk (char *vdiskname, unsigned int m)
{
    char command[1000];
    int size;
    int num = 1;
    int count;
    size  = num << m;
    count = size / BLOCKSIZE;
    //    printf ("%d %d", m, size);
    sprintf (command, "dd if=/dev/zero of=%s bs=%d count=%d",
             vdiskname, BLOCKSIZE, count);
    //printf ("executing command = %s\n", command);
    system (command);

    // now write the code to format the disk below.
    // .. your code...

    sfs_mount(vdiskname);

    if (vdisk_fd != 1){
    	printf("formatting...\n");
    	int size;
    	int pos;

    	void *rootdirptr;

    	rtdir *entryptr = (rtdir*) malloc(sizeof(rtdir));

    	rootdirptr = (void*) entryptr;

    	entryptr -> size = 0;
    	entryptr -> blk = -1;
    	entryptr -> isAvailable = -1;

    	size = BLOCKSIZE * DIR_BLOCK_COUNT / DIR_ENTRY_SIZE;
    	pos = BLOCKSIZE;

    	for (int i = 0; i < size; ++i)
    	{
    		lseek(vdisk_fd, (off_t) pos, SEEK_SET);
    		write(vdisk_fd, rootdirptr, sizeof(rtdir));
    		pos += DIR_ENTRY_SIZE;
    	}

    	fcb *fcbptr = (fcb*) malloc(sizeof(fcb));
    	fcbptr -> dir = -1;
    	fcbptr -> nextblk = -1;

    	void* voidfcb = (void*) fcbptr;

    	pos = BLOCKSIZE * (DIR_BLOCK_COUNT + 1);

    	for (int i = 0; i < FCB_ENTRY_SIZE; ++i)
    	{
    		lseek(vdisk_fd, (off_t) pos, SEEK_SET);
    		write(vdisk_fd, voidfcb, sizeof(fcb));
    		pos += FCB_ENTRY_SIZE;
    	}
    	free(fcbptr);
    	free(entryptr);

    }
    else {
    	printf("open disk error!\n");
    }
    
    return (0); 
}


// already implemented
int sfs_mount (char *vdiskname)
{
    // simply open the Linux file vdiskname and in this
    // way make it ready to be used for other operations.
    // vdisk_fd is global; hence other function can use it. 
    vdisk_fd = open(vdiskname, O_RDWR); 
    return(0);
}


// already implemented
int sfs_umount ()
{
    fsync (vdisk_fd); // copy everything in memory to disk
    close (vdisk_fd);
    return (0); 
}


int sfs_create(char *filename)
{
    return (0);
}


int sfs_open(char *file, int mode)
{
    return (0); 
}

int sfs_close(int fd){
    return (0); 
}

int sfs_getsize (int  fd)
{
    return (0); 
}

int sfs_read(int fd, void *buf, int n){
    return (0); 
}


int sfs_append(int fd, void *buf, int n)
{
    return (0); 
}

int sfs_delete(char *filename)
{
    return (0); 
}

