/*
 * NAME: Chungchhay Kuoch
 * EMAIL: chungchhay@ucla.edu
 * ID: 004843308
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "ext2_fs.h"

//extern struct image *img; //image
static int fd; //file descriptor for the image

//size of each data type
static const int BLOCK_SIZE = 1024;
static const int GROUP_DESC_SIZE = 32;
static const int INODE_SIZE = 128;
static const int DIRENT_SIZE = 8;
static const int INDIRECT_SIZE = 18;
static int block_offset = 0; //offset for each datatype

//data types for file system
enum datatypes {
    BYTE = 256, INT16, INT32,
    SBLOCK, GROUP, INODE, DIRENT, INDIRECT
};

//block img reader.h
struct ext2_indirect {
    __uint32_t inode;            //inode number of the file
    __uint16_t indirect_level;   //scanned block level of indirection
    __uint32_t block_offset;     // logical block offset
    __uint32_t indirect_block;   // indirect block number
    __uint32_t referenced_block; // referenced block number
};

//Variables used for output from the image
struct image {
    struct ext2_super_block sblock;
    struct ext2_group_desc *group;
    struct ext2_inode *inode;
    struct ext2_dir_entry *dirent;
    struct ext2_indirect *indirect;
    int *bfree;
    int *ifree;
    int num_groups;
    int num_bfree;
    int num_ifree;
    int num_inodes;
    int num_dirents;
    int num_indirects;
    int *inode_num;
    int *dirent_parent;
};

//check for the arguments. The user has to input 2 arguments.
int process_args(int, char**);

//print error
void print_error(int);

//These function are to print each data type
void print_csv(void);
static void print_superblock(void);
static void print_group(void);
static void print_bfree(void);
static void print_ifree(void);
static void print_inode(int);
static void print_dirent(int);
static void print_indirect(unsigned int);

struct image *img;    // entire structure of image

//offset for each data type
static void offset_function(int);

//These functions are to read image and each data type
static void image_read(void*, int, enum datatypes);
static void read_superblock(void);
static void read_group(void);
static int* read_free_list(int*, int);
static void read_bfree(void);
static void read_ifree(void);
static void read_inode(void);
static void read_dirent(void);
static void read_indirect(void);
static void read_indirect_recursive(int, int, int, int, int*);

void init_image(int); //initialize the image
void image_validation(void); //check if the image is valid
struct image* scan_image(void); //scan image
void close_image(void); //After done checking image, close it

int main(int argc, char *argv[]) {
  int image_file_descriptor = process_args(argc, argv); //create the file descriptor
  init_image(image_file_descriptor); //initializing the image
  image_validation(); //checking if the image is valid
  img = scan_image(); //scanning the image
  print_csv(); //print datatypes

  //free the memory
  close_image();
  exit(0);
}

int process_args(int argc, char* argv[]) {
  int image_file_descriptor;
  char error_usage[34] =
    " ERROR file or Directory\n";

  //check the arguments
  if (argc != 2) {
    fprintf(stderr, "Invalid number of argument(s).\n");
    fprintf(stderr, "%s", error_usage);
    exit(1);
  }
  
  //open the file image
  image_file_descriptor = open(argv[1], O_RDONLY);
    
  //check for the error
  if (image_file_descriptor < 0) {
    print_error(errno);
    fprintf(stderr, "%s", error_usage);
    exit(1);
  }
    
  return image_file_descriptor;
}

void print_error(int err) {
    fprintf(stderr, "Error %d: %s", err, strerror(err));
}

void print_csv(void) {
    //call each data type for printing
    //This function is the main function to print every other data types
    print_superblock();
    print_group();
    print_bfree();
    print_ifree();
    
    unsigned int i;
    for (i = 1; i <= img->sblock.s_inodes_count; i++) {
        print_inode(i);
        print_dirent(i);
        print_indirect(i);
    }
}


static void print_superblock(void) {
    //print the super block of the file image
    char buf[64];
    memset(buf, 0, 64);
    sprintf(buf, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
            img->sblock.s_blocks_count,
            img->sblock.s_inodes_count,
            1024 << img->sblock.s_log_block_size,
            img->sblock.s_inode_size,
            img->sblock.s_blocks_per_group,
            img->sblock.s_inodes_per_group,
            img->sblock.s_first_ino);
    
    fprintf(stdout, "%s", buf);
}


static void print_group(void) {
    //print the group of the file image
    char buf[64];
    memset(buf, 0, 64);
    int i;
    
    //get the count of the blocks and the inodes
    int block = img->sblock.s_blocks_count;
    int inode = img->sblock.s_inodes_count;
    
    //To see the current blocks and inodes we are at
    int current_blocks, current_inodes;
    
    //get the blocks and inodes per group
    int group_block = img->sblock.s_blocks_per_group;
    int group_inode = img->sblock.s_inodes_per_group;
    
    for (i = 0; i < img->num_groups; i++) {
        //check if the block is bigger or equal to the group block, store group block
        //in the current block. Otherwise, store the block in the current block.
        current_blocks = (block >= group_block) ? group_block : block;
        
        //check if the inode is bigger or equal to the group inode, store group inode
        //in the current inode. Otherwise, store the inode in the current inode.
        current_inodes = (inode >= group_inode)? group_inode : inode;
        block -= group_block;
        inode -= group_inode;
        
        //printing the blocks and inodes group
        sprintf(buf, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
                i,
                current_blocks,
                current_inodes,
                img->group[i].bg_free_blocks_count,
                img->group[i].bg_free_inodes_count,
                img->group[i].bg_block_bitmap,
                img->group[i].bg_inode_bitmap,
                img->group[i].bg_inode_table);
        
        fprintf(stdout, "%s", buf);
    }
}


static void print_bfree(void) {
    //print the bFree of the file image
    char buf[16];
    memset(buf, 0, 16);
    int i;
    for (i = 0; i < img->num_bfree; i++) {
        sprintf(buf, "BFREE,%d\n", img->bfree[i]);
        fprintf(stdout, "%s", buf);
    }
}


static void print_ifree(void) {
    //print the iFree of the file image
    char buf[16];
    memset(buf, 0, 16);
    int i;
    for (i = 0; i < img->num_ifree; i++) {
        sprintf(buf, "IFREE,%d\n", img->ifree[i]);
        fprintf(stdout, "%s", buf);
    }
}


static void print_inode(int inode) {
    //print the inode of the file image
    char buf[255];
    char *atime, *ctime, *mtime;
    memset(buf, 0, 255);
    long a, c, m;
    int format = 0;
    char option;
    int len;
    char print_option[84] = "INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d"; //create all the datatypes in one array
    
    atime = calloc(255, sizeof(char));
    mtime = calloc(255, sizeof(char));
    ctime = calloc(255, sizeof(char));
    
    int i, j;
    for (i = 0; i < img->num_inodes; i++) {
        if (img->inode_num[i] == inode) {
            format = img->inode[i].i_mode;
            format &= 0xF000;
            if (format == 0x4000) option = 'd'; // directory
            else if (format == 0x8000) option = 'f'; // file
            else if (format == 0xA000) option = 's'; // symbolic
            else option = '?';
            
            a = (long) img->inode[i].i_atime;
            m = (long) img->inode[i].i_mtime;
            c = (long) img->inode[i].i_ctime;
            
            strftime(ctime, 255, "%D %T", gmtime(&c));
            strftime(mtime, 255, "%D %T", gmtime(&m));
            strftime(atime, 32, "%D %T", gmtime(&a));
            
            len = sprintf(buf, print_option,
                          img->inode_num[i],
                          option,
                          img->inode[i].i_mode % 4096,
                          img->inode[i].i_uid,
                          img->inode[i].i_gid,
                          img->inode[i].i_links_count,
                          ctime,
                          mtime,
                          atime,
                          img->inode[i].i_size,
                          img->inode[i].i_blocks);
            
            //if symbolic, add the block of index 0 to len. Otherwise, add block of index j to len
            if (option == 's') {
                len += sprintf(buf+len, ",%d", img->inode[i].i_block[0]);
            }
            else {
                for (j = 0; j < 15; j++) {
                    len += sprintf(buf+len, ",%d", img->inode[i].i_block[j]);
                }
            }
            len += sprintf(buf+len, "\n");
            
            fprintf(stdout, "%s", buf);
        }
    }
    free(atime);
    free(mtime);
    free(ctime);
}


static void print_dirent(int inode) {
    //print direct of the file image
    char buf[300];
    char file[255];
    memset(buf, 0, 300);
    int offset = 0;
    
    int i;
    for (i = 0; i < img->num_dirents; i++) {
        if (img->dirent_parent[i] == inode) {
            memset(file, 0, 255);
            strncpy(file, img->dirent[i].name, img->dirent[i].name_len);
            sprintf(buf, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",
                    img->dirent_parent[i],
                    offset,
                    img->dirent[i].inode,
                    img->dirent[i].rec_len,
                    img->dirent[i].name_len,
                    file);
            offset += img->dirent[i].rec_len;
            
            fprintf(stdout, "%s", buf);
        }
    }
}


static void print_indirect(unsigned int inode) {
    //print indirect of the file image
    char buf[32];
    memset(buf, 0, 32);
    
    int i;
    for (i = 0; i < img->num_indirects; i++) {
        if (img->indirect[i].inode == inode) {
            sprintf(buf, "INDIRECT,%d,%d,%d,%d,%d\n",
                    img->indirect[i].inode,
                    img->indirect[i].indirect_level,
                    img->indirect[i].block_offset,
                    img->indirect[i].indirect_block,
                    img->indirect[i].referenced_block);
            
            fprintf(stdout, "%s", buf);
        }
    }
}

struct image* scan_image(void) {
    //just to scan each datatype
    read_superblock();
    read_group();
    read_bfree();
    read_ifree();
    read_inode();
    read_dirent();
    read_indirect();
    return img;
}


void init_image(int image_file_descriptor) {
    //request memory for the image
    fd = image_file_descriptor;
    img = calloc(1, sizeof(struct image));
}


void image_validation(void) {
    //Check the validation for the image
    struct ext2_super_block *s = &img->sblock;
    offset_function(BLOCK_SIZE);
    image_read(&s->s_magic, 56, INT16);
    if (s->s_magic != 0xEF53) {
        fprintf(stderr, "Invalid image structures\n");
        fprintf(stderr, "ERROR: %X detected.\n", s->s_magic);
        exit(2);
    }
}


void close_image(void) {
    //Free each data type that was created by the init image
    free(img->ifree);
    free(img->bfree);
    free(img->group);
    free(img->inode);
    free(img->dirent);
    free(img->indirect);
    free(img->inode_num);
    free(img->dirent_parent);
    free(img);
    close(fd);
}


static void offset_function(int offset) {
    //Offset of the block
    block_offset = offset;
}


static void image_read(void * dest, int offset, enum datatypes type) {
    int size;
    
    //This if else statement is based on the enum datatypes above
    if (type == BYTE) size = 1;
    else if (type == INT16) size = 2;
    else if (type == INT32) size = 4;
    else if (type == SBLOCK) size = BLOCK_SIZE;
    else if (type == GROUP) size = GROUP_DESC_SIZE;
    else if (type == INODE) size = INODE_SIZE;
    else if (type == DIRENT) size = DIRENT_SIZE;
    else if (type == INDIRECT) size = INDIRECT_SIZE;
    else size = type;
    
    void * buf = calloc(1, size);
    int bytes = pread(fd, buf, size, block_offset + offset);
    if (bytes < 0) {
        print_error(errno);
        free(buf);
        exit(2);
    }
    
    //Store buffer into the destination based on the datatype it receives
    if (type == BYTE)
        *(__uint8_t*)dest = *(__uint8_t*)buf;
    else if (type == INT16) 
        *(__uint16_t*)dest = *(__uint16_t*)buf;
    else if (type == INT32) 
        *(__uint32_t*)dest = *(__uint32_t*)buf;
    else if (type == SBLOCK)
        *(struct ext2_super_block*)dest = *(struct ext2_super_block*)buf;
    else if (type == GROUP)
        *(struct ext2_group_desc*)dest = *(struct ext2_group_desc*)buf;
    else if (type == INODE)
        *(struct ext2_inode*)dest = *(struct ext2_inode*)buf;
    else if (type == DIRENT)
        *(struct ext2_dir_entry*)dest = *(struct ext2_dir_entry*)buf;
    else if (type == INDIRECT)
        *(struct ext2_indirect*)dest = *(struct ext2_indirect*)buf;
    else
        strncpy(dest, buf, bytes);
    
    //free memory of the buffer
    free(buf);
}


static void read_superblock(void) {
    //read the data type from super block
    struct ext2_super_block *s = &img->sblock;
    offset_function(BLOCK_SIZE);
    image_read(s, 0, SBLOCK);
}


static void read_group(void) {
    //read the data type from group
    struct ext2_super_block *s = &img->sblock;
    img->num_groups = 1 + (s->s_blocks_count - 1) / s->s_blocks_per_group;
    img->group = calloc(img->num_groups, sizeof(struct ext2_group_desc));
    
    int i;
    for (i = 0; i < img->num_groups; i++) {
        offset_function(2*BLOCK_SIZE + i*GROUP_DESC_SIZE);
        image_read(&img->group[i], 0, GROUP);
    }
}


static void read_bfree(void) {
    //read the data type from bfree
    int size;
    img->bfree = read_free_list(&size, 1);
    img->num_bfree = size;
}


static void read_ifree(void) {
    //read the data type from ifree
    int size;
    img->ifree = read_free_list(&size, 0);
    img->num_ifree = size;
}


static int * read_free_list(int *size, int is_bfree) {
    //read the data type from free list
    int i, j, k, free_count = 0;
    int byte, mask;
    int bitmap, frees, per_group;
    
    //if the bfree is not 0, store block per group into a variable. Otherwise, store inode per group
    per_group = (is_bfree) ? img->sblock.s_blocks_per_group : img->sblock.s_inodes_per_group;
    
    int * free_list = calloc(0, sizeof(int));
    
    for (i = 0; i < img->num_groups; i++) {
        //if bfree is not 0, store block bitmap into a variable. Otherwise, store inode bitmap count
        bitmap = (is_bfree) ? img->group[i].bg_block_bitmap : img->group[i].bg_inode_bitmap;
        
        //if bfree is not 0, store free block count into a variable. Otherwise, store free inode count
        frees = (is_bfree) ? img->group[i].bg_free_blocks_count : img->group[i].bg_free_inodes_count;
        
        //set the offset
        offset_function(bitmap * BLOCK_SIZE);
        
        
        //the actual read from the image and store it into the free list
        for(j = 0; j < per_group; j++) {
            byte = 0;
            mask = 1;
            image_read(&byte, j, BYTE);
            for (k = 0; k < 8; k++) {
                if (!(byte & mask)) {
                    free_count++;
                    free_list = realloc(free_list, free_count*sizeof(int));
                    free_list[free_count-1] = i*per_group + j*8 + k + 1;
                }
                if (free_count >= frees) {
                    *size = free_count;
                    return free_list;
                }
                mask <<= 1;
            }
        }
    }
    
    *size = free_count;
    return free_list;
}


static void read_inode(void) {
    //read the data type from inode
    int i, j, count = 0;
    img->inode = calloc(0, sizeof(struct ext2_inode));
    img->inode_num = calloc(0, sizeof(int));
    
    int inodes_per_group, inode_count;
    
    //stores the inodes_per_group and inode_count into local variables
    inodes_per_group = img->sblock.s_inodes_per_group;
    inode_count = img->sblock.s_inodes_count;
    
    int table;
    int mode = 0, link_count = 0;
    
    for (i = 0; i < img->num_groups; i++) {
        for (j = 0; j < inodes_per_group; j++) {
            //Check the condition, if i*inodes_per_group+j < count, read the image
            if (i*inodes_per_group+j < inode_count) {
                table = img->group[i].bg_inode_table;
                offset_function(table*BLOCK_SIZE + j*INODE_SIZE);
                image_read(&mode, 0, INT16);
                image_read(&link_count, 26, INT16);
                //if mode > 0 && link_count > 0, read the inode image
                if (mode > 0 && link_count > 0) {
                    count++;
                    img->inode = realloc(img->inode, count*sizeof(struct ext2_inode));
                    img->inode_num = realloc(img->inode_num, count*sizeof(int));
                    image_read(&img->inode[count-1], 0, INODE);
                    img->inode_num[count-1] = j + 1;
                }
            }
        }
    }
    
    img->num_inodes = count;
}


static void read_dirent(void) {
    //read the data type from direct read
    //like any other read, except it has to check for some conditions
    //for example, it has to check if the mode is 0x4000, so it can proceed to read it
    int i, j;
    int count = 0;
    int offset;
    int temp;
    int mode = 0;
    int read_block = 0;
    img->dirent = calloc(0, sizeof(struct ext2_dir_entry));
    img->dirent_parent = calloc(0, sizeof(int));
    
    for (i = 0; i < img->num_inodes; i++) {
        mode = img->inode[i].i_mode & 0xF000;
        if (mode == 0x4000) { //directory
            for (j = 0; j < 12; j++) {
                read_block = img->inode[i].i_block[j];
                if (read_block > 0) {
                    offset = 0;
                    offset_function(read_block * BLOCK_SIZE);
                    image_read(&temp, 0, INT32);
                    while (temp != 0 && offset < BLOCK_SIZE) {
                        count++;
                        img->dirent = realloc(img->dirent,
                                              count*sizeof(struct ext2_dir_entry));
                        img->dirent_parent = realloc(img->dirent_parent,
                                                     count*sizeof(int));
                        image_read(&img->dirent[count-1], 0, DIRENT);
                        image_read(img->dirent[count-1].name, DIRENT_SIZE,
                                 img->dirent[count-1].name_len);
                        img->dirent_parent[count-1] = img->inode_num[i];
                        offset += img->dirent[count-1].rec_len;
                        image_read(&temp, img->dirent[count-1].rec_len, INT32);
                        offset_function(read_block * BLOCK_SIZE + offset);
                    }
                }
            }
        }
    }
    
    img->num_dirents = count;
}


static void read_indirect(void) {
    //read the data type from indirect read
    int i, j, count = 0;
    int read_block;
    img->indirect = calloc(0, sizeof(struct ext2_indirect));
    
    for (i = 0; i < img->num_inodes; i++) {
        for (j = 12; j < 15; j++) {
            //call the indirect recuresive function
            read_block = img->inode[i].i_block[j];
            read_indirect_recursive(read_block,
                               j - 11,
                               img->inode_num[i],
                               0,
                               &count);
        }
    }
    img->num_indirects = count;
}


static void read_indirect_recursive(int block, int level, int inode,
                               int logical, int *count) {
    //recursively to read the indirect read
    int root = block;
    int offset = 0;
    
    if (logical == 0) {
        if (level == 3)
            logical = 256*256 + 256;
        
        if (level == 2)
            logical = 256;
        
        logical += 12;
    }
    
    //Loop whenever offset is less than the block size
    while (offset < BLOCK_SIZE) {
        offset_function(root * BLOCK_SIZE);
        image_read(&block, offset, INT32);
        if (block > 0) {
            (*count)++;
            //store values into inode, indirect_level, block_offset, indirect_block, referenced_block
            img->indirect = realloc(img->indirect,
                                    *count*sizeof(struct ext2_indirect));
            img->indirect[*count-1].inode = inode;
            img->indirect[*count-1].indirect_level = level;
            img->indirect[*count-1].block_offset = logical;
            img->indirect[*count-1].indirect_block = root;
            img->indirect[*count-1].referenced_block = block;
            
            if (level > 1)
                read_indirect_recursive(block, level - 1, inode, logical, count);
        }
        offset += 4;
        logical++;
    }
}



