#!/usr/bin/python

#Name: Chungchhay Kuoch

import csv
import sys
import os

dirents = []
inodes = []
indirects = []
block_free_list = []
inode_free_list = []
superBlockVar = None
ptr = []
unallocate = []
allocate = []
block_reserve = -1
error_code = 0
dataBlock = ["BLOCK", "INDIRECT BLOCK", "DOUBLE INDIRECT BLOCK", "TRIPLE INDIRECT BLOCK"]
# the class for SuperBlock
class SuperBlock:
    def __init__(self, col1, col2, col3, col4, col5):
        self.num_blocks = col1
        self.num_inodes = col2
        self.block_size = col3
        self.inode_size = col4
        self.first_non_reserved_inode = col5
# the class for Inode
class Inode:
    def __init__(self, col1, col2, col3, col4, col5, col6, col7):
        self.number_inode = col1
        self.file_type_inode = col2.rstrip()
        self.group_inode = col3
        self.num_blocks = col4
        self.one_indirect_inode = col5
        self.double_indirect_inode = col6
        self.triple_indirect_inode = col7
# the class of Indirect
class Indirect:
    def __init__(self, col1, col2, col3, col4):
        self.number_inode_indirect = col1
        self.level_indirect = col2
        self.block_offset_indirect = col3
        self.reference_blocks_indirect = col4
# the class of Dirent
class Dirent:
    def __init__(self, col1, col2, col3):
        self.parent_inode_dirent = col1
        self.number_inode_dirent = col2
        self.name_dirent = col3.rstrip()

# this is to use the pointer to track the datablock
#it will list the datablock memebers if valid
# if not, set block
def block_consistency_audits():
    global ptr, error_code
    ptr = [-1] * superBlockVar.num_blocks
    
    #indirect blocks
    # check the indirect block.
    #pass if the indirect reference is equal to 0
    # if not, go through the following comparison and print out warning messages
    for temp in indirects:
        if temp.reference_blocks_indirect == 0:
            pass
        elif temp.reference_blocks_indirect > superBlockVar.num_blocks - 1 or temp.reference_blocks_indirect < 0:
            print("INVALID {} {} IN INODE {} AT OFFSET {}".format(dataBlock[temp.level_indirect], temp.reference_blocks_indirect, temp.number_inode_indirect, temp.block_offset_indirect))
            error_code = error_code + 1
        elif temp.reference_blocks_indirect < block_reserve:
            print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(dataBlock[temp.level_indirect], temp.reference_blocks_indirect, temp.number_inode_indirect, temp.block_offset_indirect))
            error_code = error_code + 1
        else:
            if ptr[temp.reference_blocks_indirect] == -1:
                ptr[temp.reference_blocks_indirect] = [[dataBlock[temp.level_indirect], temp.number_inode_indirect, temp.block_offset_indirect]]
            else:
                ptr[temp.reference_blocks_indirect].append([dataBlock[temp.level_indirect], temp.number_inode_indirect, temp.block_offset_indirect])

    #check direct blocks
    # check the validation of the indirect block with the number of block
    # pass if the indirect reference is equal to 0
    # if not, go through the following comparison and print out warning messages
    for temp in inodes:
        for block_num in temp.num_blocks:
            if block_num == 0:
                pass
            elif block_num > superBlockVar.num_blocks - 1 or block_num < 0:
                print("INVALID {} {} IN INODE {} AT OFFSET {}".format(dataBlock[0], block_num, temp.number_inode, 0))
                error_code = error_code + 1
            elif block_num < block_reserve:
                print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(dataBlock[0], block_num, temp.number_inode, 0))
                error_code = error_code + 1
            else:
                if ptr[block_num] == -1:
                    ptr[block_num] = [[dataBlock[0], temp.number_inode, 0]]
                else:
                    ptr[block_num].append([dataBlock[0], temp.number_inode, 0])
                        
        #sigle block
        # check if it is a single block
        # pass if the indirect innode pointer target is  0
        # if not, go through the following comparison
        # and print out warning messages
        if temp.one_indirect_inode == 0:
            pass
        elif temp.one_indirect_inode > superBlockVar.num_blocks - 1 or temp.one_indirect_inode < 0:
            print("INVALID {} {} IN INODE {} AT OFFSET {}".format(dataBlock[1], temp.one_indirect_inode, temp.number_inode, 12))
            error_code = error_code + 1
        elif temp.one_indirect_inode < block_reserve:
            print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(dataBlock[1], temp.one_indirect_inode, temp.number_inode, 12))
            error_code = error_code + 1
        else:
            if ptr[temp.one_indirect_inode] == -1:
                ptr[temp.one_indirect_inode] = [[dataBlock[1], temp.number_inode, 12]]
            else:
                ptr[temp.one_indirect_inode].append([dataBlock[1], temp.number_inode, 12])

        #double block
        # check for double block
        # pass if the indirect inode pointer target is  0
        # if not, go through the following comparison with the pointer target of the superblock or the inode
        # and print out warning message
        if temp.double_indirect_inode == 0:
            pass
        elif temp.double_indirect_inode > superBlockVar.num_blocks - 1 or temp.double_indirect_inode < 0:
            print("INVALID {} {} IN INODE {} AT OFFSET {}".format(dataBlock[2], temp.double_indirect_inode, temp.number_inode, 268))
            error_code = error_code + 1
        elif temp.double_indirect_inode < block_reserve:
            print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(dataBlock[2], temp.double_indirect_inode, temp.number_inode, 268))
            error_code = error_code + 1
        else:
            if ptr[temp.double_indirect_inode] == -1:
                ptr[temp.double_indirect_inode] = [[dataBlock[2], temp.number_inode, 268]]
            else:
                ptr[temp.double_indirect_inode].append([dataBlock[2], temp.number_inode, 268])

        #triple block
        # check for triple block
        # pass if the indirect inode pointer target is  0
        # if not, go through the following comparison with the pointer target of the superblock or the inode
        # and print out relevant message
        if temp.triple_indirect_inode == 0:
            pass
        elif temp.triple_indirect_inode > superBlockVar.num_blocks - 1 or temp.triple_indirect_inode < 0:
            print("INVALID {} {} IN INODE {} AT OFFSET {}".format(dataBlock[3], temp.triple_indirect_inode, temp.number_inode, 65804))
            error_code = error_code + 1
        elif temp.triple_indirect_inode < block_reserve:
            print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(dataBlock[3], temp.triple_indirect_inode, temp.number_inode, 65804))
            error_code = error_code + 1
        else:
            if ptr[temp.triple_indirect_inode] == -1:
                ptr[temp.triple_indirect_inode] = [[dataBlock[3], temp.number_inode, 65804]]
            else:
                ptr[temp.triple_indirect_inode].append([dataBlock[3], temp.number_inode, 65804])

    # in the list of the free blocks
    # where the temporary pointer points to is equal to -1
    # assign it to be 0
    # in general, this part is freeign blocks and inode, and
    # in below, we are check the unreferenced blocks and duplicate blocks
    # printing relevant message for it
    for temp in block_free_list:
        if(ptr[temp] == -1):
            ptr[temp] = 0
        else:
            print("ALLOCATED BLOCK {} ON FREELIST".format(temp))
            error_code = error_code + 1

    for number_block_temp , entry_block in enumerate(ptr):
        if(entry_block == -1 and number_block_temp < superBlockVar.num_inodes  + 1):
            pass
        elif(entry_block == 0):
            pass
        elif(entry_block == -1):
            print("UNREFERENCED BLOCK {}".format(number_block_temp))
            error_code = error_code + 1
        elif len(entry_block) > 1:
            for item in entry_block:
                print("DUPLICATE {} {} IN INODE {} AT OFFSET {}".format(item[0], ptr.index(ptr[number_block_temp]), item[1], item[2]))
                error_code = error_code + 1


# right here, we are appendinf the inodes to the allocated free list
# the program prints messages in 2 situations
# 1- the unallocated inode is not on the free list
# 2- the unallocated inode is on the free list
# then, double check if all inodes are being allocated or not
def inode_allocation_audits():
    global error_code, inodes, allocate, unallocate

    unallocate = inode_free_list

    for temp in inodes:
        if temp.file_type_inode == '0':
            if temp.number_inode not in inode_free_list:
                print("UNALLOCATED INODE {} NOT ON FREELIST".format(temp.number_inode))
                error_code = error_code + 1
                unallocate.append(temp.number_inode)
        else:
            if temp.number_inode in inode_free_list:
                print("ALLOCATED INODE {} ON FREELIST".format(temp.number_inode))
                error_code = error_code + 1
                unallocate.remove(temp.number_inode)

            allocate.append(temp)

    for temp in range(superBlockVar.first_non_reserved_inode, superBlockVar.num_inodes):
        used = True if len(list(filter(lambda x: x.number_inode == temp, inodes))) > 0 else False
        if temp not in inode_free_list and not used:
            print("UNALLOCATED INODE {} NOT ON FREELIST".format(temp))
            error_code = error_code + 1
            unallocate.append(temp)

# here is where checking for the block consistency audits
# see if every inodes or indirect blocks are valid
# the checking includes comparing the number of inode and total inodes, and
# we also check whether directory and inode linkcount matches
def directory_consistency_audits():
    global error_code
    total_inodes = superBlockVar.num_inodes
    inode_link_map = {}

    for temp in dirents:
        if temp.number_inode_dirent > total_inodes:
            print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(temp.parent_inode_dirent, temp.name_dirent, temp.number_inode_dirent))
            error_code = error_code + 1
        elif temp.number_inode_dirent in unallocate:
            print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(temp.parent_inode_dirent, temp.name_dirent, temp.number_inode_dirent))
            error_code = error_code + 1
        else:
            inode_link_map[temp.number_inode_dirent] = inode_link_map.get(temp.number_inode_dirent, 0) + 1

    for temp in allocate:
        if temp.number_inode in inode_link_map:
            if temp.group_inode != inode_link_map[temp.number_inode]:
                print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(temp.number_inode, inode_link_map[temp.number_inode], temp.group_inode))
                error_code = error_code + 1
        else:
            if temp.group_inode != 0:
                print("INODE {} HAS 0 LINKS BUT LINKCOUNT IS {}".format(temp.number_inode, temp.group_inode))
                error_code = error_code + 1

# every directory has with two links
# one to itself and one to its parent
# here is the part to check the linkage
# we put the pointer to go through the dirent,
# see if the name in the parent directory and its directory matches
# then, prints relevant messages
    inode_parent = {2:2}
    
    for temp in dirents:
        if temp.number_inode_dirent <= superBlockVar.num_inodes and temp.number_inode_dirent not in unallocate:
            if temp.name_dirent != "'..'" and temp.name_dirent != "'.'":
                inode_parent[temp.number_inode_dirent] = temp.parent_inode_dirent

    for temp in dirents:
        if temp.name_dirent == "'.'":
            if temp.number_inode_dirent != temp.parent_inode_dirent:
                print("DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(temp.parent_inode_dirent, temp.number_inode_dirent, temp.parent_inode_dirent))
                error_code = error_code + 1
        elif temp.name_dirent == "'..'":
            if temp.number_inode_dirent != inode_parent[temp.parent_inode_dirent]:
                print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(temp.parent_inode_dirent, temp.number_inode_dirent, inode_parent[temp.parent_inode_dirent]))
                error_code = error_code + 1

def main():
    # Here, we check if the sys arguments is not 2, it is invalid
    if(len(sys.argv)) != 2:
        sys.stderr.write("Invalid number of argument(s).\n")
        exit(1)
    
    # read the file and check for existence
    file = sys.argv[1]
    if not os.path.isfile(file):
        sys.stderr.write("Error in reading the file.\n")
        exit(1)
    
    #open the file
    global superBlockVar, block_reserve, block_free_list, inode_free_list, inodes, indirects
    
    fileOpen = open(file, 'r')
    
    if not fileOpen:
        sys.stderr.write("Error in opening file.\n")
        exit(1)
    
    if os.path.getsize(file) <= 0:
        sys.stderr.write("Error occures because of empty file.\n")
        exit(1)

    #reading the csv file
    reader = csv.reader(fileOpen)

    #check for each line to see which option belong to which so that we can initialize the value for that option
    for read in reader:
        if len(read) == 0:
            sys.stderr.write("Error because there is no lines.\n")
            exit(1)
        
        temp = read[0]
        
        if temp == 'SUPERBLOCK':
            superBlockVar = SuperBlock(int(read[1]), int (read[2]), int(read[3]), int(read[4]), int(read[7]))
        elif temp == 'GROUP':
            block_reserve = int(read[8])
        elif temp == 'BFREE':
            block_free_list.append(int(read[1]))
        elif temp == 'IFREE':
            inode_free_list.append(int(read[1]))
        elif temp == 'DIRENT':
            dirents.append(Dirent(int(read[1]), int(read[3]), read[6]))
        elif temp == 'INODE':
            inodes.append(Inode(int(read[1]), read[2], int(read[6]), list(map(int, read[12:24])), int(read[24]), int(read[25]), int(read[26])))
        elif temp == 'INDIRECT':
            indirects.append(Indirect(int(read[1]), int(read[2]), int(read[3]), int(read[5])))
        else:
            sys.stderr.write("Error not recognize the option.\n")
            exit(1)
    
    block_consistency_audits()
    inode_allocation_audits()
    directory_consistency_audits()

# exit code when
# 0-successful execution, no inconsistencies found.
# 1-unsuccessful execution, bad parameters or system call failure.
# 2-successful execution, inconsistencies found.
    if error_code == 0:
        exit(0)
    elif error_code == 1:
        exit(1)
    elif error_code == 2:
        exit(2)


if __name__ == '__main__':
    main()


