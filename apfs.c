#include <stdio.h>
#include "apfs.h"

int parse_blk_header(FILE *apfs)
{
	APFS_BH block_header = {0};
	unsigned int size = 0;
     printf("Printing the ftell at the beginning of the block %d\n",ftell(apfs));
	if ((size = fread(&block_header, 1, sizeof(block_header), apfs)) < 0) {
        	printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(block_header));
                return -1;
        }
	printf("Parsed APFS BLK Header of Container at Block 0 and it's Size = %u\n", size);

	printf("Checksum = %lu\n", block_header.checksum);
	printf("Block ID = %lu\n", block_header.block_id);
	printf("Version = %lu\n", block_header.version);

	return 0;
}

APFS_SuperBlk findValidSuperBlock(FILE *apfs)
{
	APFS_SuperBlk containerSuperBlk;
	int size=fread(&containerSuperBlk,1,sizeof(containerSuperBlk),apfs);
	    printf("Read the body of Container Super Block at Block 0 and it;s size is %d\n",size);
		
			// Since the descriptor base addresss is virtual, after myltiplying 4096 to it,
            // we will go to that address to get the checkpoint mapping array.
			
			uint64_t checkpointMappingArray= containerSuperBlk.DescriptorBase* containerSuperBlk.BlockSize;
			//fseek to that address
			fseek(apfs,checkpointMappingArray,SEEK_SET);
			printf("The starting addresss of checkpoint mapping area is %d and in hex is: %x\n",ftell(apfs),ftell(apfs));
			
			//Now fread the checkpoint mapping array. It i a super block array of size 4096 bytes.
			//Need to read the header to get checksum and xid and body to get the Magic number.
			//Size of the array will be descriptor blocks from teh superblock at block 0.
			int recentValidSuperblock =0;
			int validSuperBlockAddress=0;
			for(int checkpointMappingItr = (containerSuperBlk.DescriptorBlocks) - 1;checkpointMappingItr >= 0;checkpointMappingItr--)
			{
				fseek(apfs,checkpointMappingItr*containerSuperBlk.BlockSize,SEEK_CUR);
				APFS_BH block_header_chkMap = {0};
	            
	            if ((size = fread(&block_header_chkMap, 1, sizeof(block_header_chkMap), apfs)) < 0) 
				{
        	    printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(block_header_chkMap));
                return containerSuperBlk;
                }
				printf("Parsed the Container SUperblock Header. Size = %u\n", size);
				
	            
				if(block_header_chkMap.block_type == 1)
				{
					if(block_header_chkMap.version > recentValidSuperblock)
					{
						recentValidSuperblock=block_header_chkMap.version;
						//Now check for checksum;
						//Check for magic Number
						char ch[4]={""};
						char ch2[4]={"NXSB"};
						fread(ch,4,1,apfs);
						if(strncmp(ch,ch2,sizeof(ch)) == 0)
							printf("It is a superblock\n");
						else
							continue; 
					    //Store this address in a variable to go to this valid superblock.
						validSuperBlockAddress = ftell(apfs) - 36;
						printf("The address of valid superblock is %d\n",validSuperBlockAddress);
						printf(" Valid Container Super Block's Checksum = %lu\n", block_header_chkMap.checksum);
	                    printf(" Valid Container Super Block's Block ID = %lu\n", block_header_chkMap.block_id);
	                    printf(" Valid Container Super Block's Version = %lu\n", block_header_chkMap.version);
						printf(" Valid Container Super Block's Block Type = %u\n", block_header_chkMap.block_type);
	                    printf(" Valid Container Super Block's Flags = %u\n", block_header_chkMap.flags);
	                    printf(" Valid Container Super Block's Padding = %u\n", block_header_chkMap.padding);
						break;
					}
				}
				else
				{
					printf("The subtype is not superblock conatiner, hence going to next block in array\n");
					fseek(apfs,checkpointMappingArray,SEEK_SET);
				    
				}
			}
	return containerSuperBlk;
}

omap_phys_t parseValidContainerSuperBlock(FILE *apfs,APFS_SuperBlk containerSuperBlk)
{
	//get the omapid and get the volume ids.
			//Getting the volume id.
			int numberOfVolumes = -1;
			for(int fsidItr=0;fsidItr<100;fsidItr++)
			{
				if(containerSuperBlk.VolumesIdents[fsidItr] !=0)
				{
					numberOfVolumes++;
				}
				else
					break;
			}
			
			omap_phys_t omapStructure;
			//fseek to the omap address.
			fseek(apfs,containerSuperBlk.ObjectsMapIdent * containerSuperBlk.BlockSize,SEEK_SET);
			unsigned int sizeofOmapStructure = 0;
	        if ((sizeofOmapStructure = fread(&omapStructure, 1, sizeof(omapStructure), apfs)) < 0) {
        	            printf("Error reading from apfs File! [%d] size = %lu\n", sizeofOmapStructure, sizeof(omapStructure));
                return omapStructure;
            }
	        printf("Parsed the OMAP Structure of Container Super Block of Size = %u\n", sizeofOmapStructure);
			printf("Tree oid is: %lu\n",omapStructure.om_tree_oid);	
			return omapStructure;
}


apfs_superblock_t findValidVolumeSuperBlock(FILE *apfs,omap_phys_t omapStructure,APFS_SuperBlk containerSuperBlk)
{
	apfs_superblock_t volumeSuperBlock;
	// go to tree adress
            fseek(apfs,	omapStructure.om_tree_oid * containerSuperBlk.BlockSize,SEEK_SET);
			
            btree_node_phys_t omapBTree;
			int endOfOmapBtree = ftell(apfs) + 4096;
				printf(" EndOfBTree is: %d",endOfOmapBtree);

            unsigned int sizeofOmapTree = 0;
	        if ((sizeofOmapTree = fread(&omapBTree, 1, sizeof(omapBTree), apfs)) < 0) {
        	            printf("Error reading from apfs File! [%d] size = %lu\n", sizeofOmapTree, sizeof(omapBTree));
                return volumeSuperBlock;
            }
	        printf("Parsed Omap's B tree! Size = %u\n", sizeofOmapTree);
            printf("We are at %d and in hex we are after reading the omap 56 bytes is %x\n",ftell(apfs),ftell(apfs));	

    printf("btn_flags = %lu\n", omapBTree.btn_flags);
	printf(" count is :%" PRIu16 "\n",omapBTree.btn_level);
	printf("btn_nkeys = %u\n", omapBTree.btn_nkeys);
	printf("btn_table_space offset= %d\n", omapBTree.btn_table_space.off);
	printf("btn_table_space length = %d\n", omapBTree.btn_table_space.len);
	printf("btn_key_free_list offset= %d\n", omapBTree.btn_key_free_list.off);
	printf("btn_key_free_list length = %d\n", omapBTree.btn_key_free_list.len);
    printf("btn_val_free_list offset= %d\n", omapBTree.btn_val_free_list.off);
	printf("btn_val_free_list length = %d\n", omapBTree.btn_val_free_list.len);
	
	// Now for the table length loop to get the key value pairs.
	// Check if the kv flag is set to determine if it is kvoff or kloc
	kvoff_t keyValueStruct[omapBTree.btn_table_space.len/4] ;
	for(int i=0;i<omapBTree.btn_table_space.len;i=i+4)
	{
		unsigned int sizeofkvoff_t = 0;
		sizeofkvoff_t=fread(&keyValueStruct,1,4,apfs);
		if(keyValueStruct->k !=0 || keyValueStruct->v != 0)
		{
	    printf("Parsed sizeofkvoff_t of table of COntents! Size = %u\n", sizeofkvoff_t);
		printf("btn_data_first_key = %" PRIu16"\n",keyValueStruct->k);	
	    printf("btn_data_first_val = %d\n", keyValueStruct->v);	
		}


	}
    int keyStartAddress = ftell(apfs);
	tApFS_0B_ObjectsMap_Key_t key;
	tApFS_0B_ObjectsMap_Value_t value;
	fread(&key,1,16,apfs);
	printf("The oid and xid are %lu and %lu\n",key.ObjectIdent,key.Transaction);
	
	// check if it is root node or leaf node
	// find the info address for root node.
	int startOfBTreeInfo = endOfOmapBtree - 56;
	printf(" startOfBTreeInfo is: %d\n",startOfBTreeInfo);
	fseek(apfs,startOfBTreeInfo,SEEK_SET);
	fread(&value,1,16,apfs);
	printf("The flag size and physicall address are %u, %u and %x and in decimal %d\n",value.Flags,value.Size,value.Address,value.Address);
	
	//Go to the Apfs Block
	fseek(apfs,value.Address * containerSuperBlk.BlockSize,SEEK_SET);
	
	unsigned int sizeofVolSuperBlock = 0;
            printf("Printing the ftell at the beginning of the block %d\n",ftell(apfs));
	        if ((sizeofVolSuperBlock = fread(&volumeSuperBlock, 1, sizeof(volumeSuperBlock), apfs)) < 0) {
        	            printf("Error reading from apfs File! [%d] size = %lu\n", sizeofVolSuperBlock, sizeof(volumeSuperBlock));
                return volumeSuperBlock;
            }
	        printf("Parsed APFS Size = %u\n", sizeofVolSuperBlock);
            printf("Printing the ftell after the super block %d and in hex it is %x \n",ftell(apfs),ftell(apfs));
			printf("Checksum =");
			for(int i=0;i<8;i++)
			{
				printf("%x",volumeSuperBlock.apfs_o.o_cksum[i]);
			}
			printf("\n");
	
	printf("OID = %lu\n", volumeSuperBlock.apfs_o.o_oid);
	printf("XID = %lu\n", volumeSuperBlock.apfs_o.o_xid);
	
	char magicApfs[4]={""};
	magicApfs[0] = volumeSuperBlock.apfs_magic >> 24;
    magicApfs[1] = volumeSuperBlock.apfs_magic >> 16;
    magicApfs[2] = volumeSuperBlock.apfs_magic >> 8;
    magicApfs[3] = volumeSuperBlock.apfs_magic;
    printf("Volume Magic Numer is:");
		
	for(int i=3;i>=0;i--)
	{
		printf("%c",magicApfs[i]);
	}
	printf("\n");
	printf("APFS Index %u=\n",volumeSuperBlock.apfs_fs_index);
    printf("APFS Features %lu=\n",volumeSuperBlock.apfs_features);
    printf("APFS Compatible Features %lu=\n",volumeSuperBlock.apfs_readonly_compatible_features);
    printf("APFS Omap ID %lu=\n",volumeSuperBlock.apfs_omap_oid);

    	
    return volumeSuperBlock;
}

void parse_APFS( int block_no )
{
	FILE *apfs = NULL;
	APFS_SuperBlk containerSuperBlk;
	omap_phys_t omapStructure;
	apfs_superblock_t volumeSuperBlock;
	APFS_SuperBlk super_blk = {0};
        char filename[16] = {0};
	unsigned int size = 0;

        snprintf(filename, sizeof(filename), "decompressed%d", block_no);

        if (NULL == (apfs = fopen (filename, "r")) ) {
                printf("Unable to parse APFS: Error opening file %s!\n", filename);
                return;
        }
	
	if (parse_blk_header(apfs)) {
		printf("Unable to parse APFS: Failed to parse Block header\n");
		return;
	}
	
	containerSuperBlk=findValidSuperBlock(apfs);
    omapStructure = parseValidContainerSuperBlock(apfs,containerSuperBlk);
    volumeSuperBlock = findValidVolumeSuperBlock(apfs,omapStructure,containerSuperBlk);
}
