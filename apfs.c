#include <stdio.h>
#include <time.h>
#include <endian.h>
#include "apfs.h"

int parse_blk_header(FILE *apfs)
{
	APFS_BH block_header = {0};
	unsigned int size = 0;
	
	if ((size = fread(&block_header, 1, sizeof(block_header), apfs)) < 0) {
		printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(block_header));
		return -1;
	}
	return 0;
}

/*
   Input Parameters: FILE* 
   Return Type:      btree_node_phys_t
   Description: Function reads the bTree Node.
*/
btree_node_phys_t readAndPrintBtree(FILE* apfs)
{
	btree_node_phys_t btree;
	unsigned int sizeofTree = 0;
	if ((sizeofTree = fread(&btree, 1, sizeof(btree), apfs)) < 0) {
		printf("Error reading from apfs File! [%d] size = %lu\n", sizeofTree, sizeof(btree));
		return btree;
	}
	return btree;
}

/*
   Input Parameters: APFS_BH, uint32_t 
   Return Type:      void
   Description: Function prints the Header of container superblock,
                Called to print the older versions of container superblock.
                
*/
void printContainerHeader(APFS_BH block_header,uint32_t containerAddress)
{
	printf("\n");
	printf("Container SuperBlock 	Address: %x		\n",containerAddress);
	printf("Checksum           			%lu\n",  block_header.checksum);
	printf("Block ID            			%lu\n",  block_header.block_id);
	printf("Version/TransactionID			%lu\n",  block_header.version);
	printf("Block Type          			%u\n",  block_header.block_type);
	printf("Flags               			%u\n", block_header.flags);
	printf("Padding             			%u\n", block_header.padding);
}

/*
   Input Parameters: obj_phys_t 
   Return Type:      void
   Description: Function prints the header of volume superblock,
                
*/
void print_blk_header(obj_phys_t obj)
{
	int i = 0;

	printf(" ##### BLOCK Header #####\n");
	printf("Checksum                        ");
	for (; i < MAX_CKSUM_SIZE; ++i)
		printf("%0x", obj.o_cksum[i]);
	printf("\n");

    printf("Block ID             			%lu\n", obj.o_oid);
    printf("Version              			%lu\n", obj.o_xid);
	printf("Block Type           			%x\n",  obj.o_type & OBJECT_TYPE_MASK);
	printf("Flags                 			%x\n", obj.o_type & OBJECT_TYPE_FLAGS_MASK);
	printf("Sub Type             			%x\n", obj.o_subtype);
	printf(" ##### END #####\n");	
}

/*
   Input Parameters: apfs_superblock_t, uint32_t , uint32_t 
   Return Type:      void
   Description: Function prints the structure of volume superblock,
                starting address and size.
                
*/
void printVolumeSuperBlock(apfs_superblock_t volumeSuperBlock, uint32_t volumeSuperBlkAddress, uint32_t sizeOfVolBlk)
{
    
	printf("\n");
	printf("Volume SuperBlock 	Address: %x		Size: %d\n",volumeSuperBlkAddress,sizeOfVolBlk);
	print_blk_header(volumeSuperBlock.apfs_o);
	printf("Magic Number        			%s\n", (char*) &volumeSuperBlock.apfs_magic);
	printf("FS INdex            			%u\n", volumeSuperBlock.apfs_fs_index);
	printf("Unmount time         			%lu\n", volumeSuperBlock.apfs_unmount_time);
	printf("Features             			%lu\n", volumeSuperBlock.apfs_features);
	printf("Reserved blks       			%lu\n", volumeSuperBlock.apfs_fs_reserve_block_count);
	printf("Quota blks          			%lu\n", volumeSuperBlock.apfs_fs_quota_block_count);
    printf("Alloc blks           			%lu\n", volumeSuperBlock.apfs_fs_alloc_count);
	printf("Root tree type       			%0x\n", volumeSuperBlock.apfs_root_tree_type);
    printf("Extent type         			%0x\n", volumeSuperBlock.apfs_extentref_tree_type);
    printf("Meta tree type        			%0x\n", volumeSuperBlock.apfs_snap_meta_tree_type);
    printf("OMAP OID             			%lu\n", volumeSuperBlock.apfs_omap_oid);
    printf("Root Tree OID        			%lu\n", volumeSuperBlock.apfs_root_tree_oid);
    printf("Extent tree OID      			%lu\n", volumeSuperBlock.apfs_extentref_tree_oid);
    printf("Snap meta tree oid    			%lu\n", volumeSuperBlock.apfs_snap_meta_tree_oid);
    printf("Next obj ID         			%lu\n", volumeSuperBlock.apfs_next_obj_id);
    printf("Num Files            			%lu\n", volumeSuperBlock.apfs_num_files);
    printf("Num Directories      			%lu\n", volumeSuperBlock.apfs_num_directories);
    printf("Symlinks             			%lu\n", volumeSuperBlock.apfs_num_symlinks);
    printf("other fs objects      			%lu\n", volumeSuperBlock.apfs_num_other_fsobjects);
    printf("Num Snapshots        			%lu\n", volumeSuperBlock.apfs_num_snapshots);
    printf("Total blks allocated   			%lu\n", volumeSuperBlock.apfs_total_blocks_alloced);
    printf("Total blks freed     			%lu\n", volumeSuperBlock.apfs_total_blocks_freed);
	printf("Last Modified Time     			%lu\n", volumeSuperBlock.apfs_last_mod_time);
    printf("FS Flags            			%lu\n", volumeSuperBlock.apfs_fs_flags);
	printf("Volume Name            			%s\n", (char*) &volumeSuperBlock.apfs_volname);
    printf("Next Doc ID            			%u\n", volumeSuperBlock.apfs_next_doc_id);
	printf("APFS Role           			%u\n", volumeSuperBlock.apfs_role);
	printf("Reserved            			%u\n", volumeSuperBlock.reserved);
    printf("APFS Root to Xid      			%lu\n", volumeSuperBlock.apfs_root_to_xid);
    printf("APFS er State oid          		%lu\n", volumeSuperBlock.apfs_er_state_oid);
    printf("APFS Cloneinfo Id Epoch        		%lu\n", volumeSuperBlock.apfs_cloneinfo_id_epoch);
    printf("APFS Cloneinfo Xid    			%lu\n", volumeSuperBlock.apfs_cloneinfo_xid);
    printf("APFS Snap meta Ext Oid   		%lu\n", volumeSuperBlock.apfs_snap_meta_ext_oid);

    printf("APFS Integrity meta Oid   		%lu\n", volumeSuperBlock.apfs_integrity_meta_oid);
    printf("APFS Fext Tree Oid      		%lu\n", volumeSuperBlock.apfs_fext_tree_oid);
	printf("APFS Fext Tree Type       		%u\n", volumeSuperBlock.apfs_fext_tree_type);
    printf("Reserved Type            		%u\n", volumeSuperBlock.reserved_type);
    printf("Reserved Oid            		%lu\n", volumeSuperBlock.reserved_oid);	
}

/*
   Input Parameters: APFS_SuperBlk, uint32_t , uint32_t 
   Return Type:      void
   Description: Function prints the structure of container superblock,
                starting address and size.
                
*/

void printContainerSuperBlock(APFS_SuperBlk containerSuperBlk, uint32_t containerAddress, uint32_t sizeOfContainer,APFS_BH block_header)
{
	printf("\n");
	printf("Latest Container SuperBlock 	Address: %x		Size: %d\n",containerAddress,sizeOfContainer);
	printf(" ##### BLOCK Header #####\n");
	printf("Checksum           			%lu\n",  block_header.checksum);
	printf("Block ID            			%lu\n",  block_header.block_id);
	printf("Version/TransactionID			%lu\n",  block_header.version);
	printf("Block Type          			%u\n",  block_header.block_type);
	printf("Flags               			%u\n", block_header.flags);
	printf("Padding             			%u\n", block_header.padding);
	printf(" ##### END #####\n");
	
	printf("Magic Number        			%s\n", (char*) &containerSuperBlk.MagicNumber);
	printf("BlockSize           			%u\n", containerSuperBlk.BlockSize);
	printf("BlocksCount         			%lu\n", containerSuperBlk.BlocksCount);
	printf("Features             			%lu\n", containerSuperBlk.Features);
	printf("ReadOnlyFeatures     			%lu\n", containerSuperBlk.ReadOnlyFeatures);
	printf("IncompatibleFeatures 			%lu\n", containerSuperBlk.IncompatibleFeatures);

	printf("Uuid                   			");
	for (int i = 0; i < 16; ++i) {
		printf("%0x", containerSuperBlk.Uuid[i]);
	}
	printf("\n");

	printf("NextIdent            			%lu\n", containerSuperBlk.NextIdent);
	printf("NextTransaction        			%lu\n", containerSuperBlk.NextTransaction);
	printf("DescriptorBlocks     			%u\n", containerSuperBlk.DescriptorBlocks);
	printf("DataBlocks          			%u\n", containerSuperBlk.DataBlocks);
	printf("DescriptorBase       			%0x\n", containerSuperBlk.DescriptorBase);
	printf("DataBase            			%u\n", containerSuperBlk.DataBase);
	printf("DescriptorNext      			%u\n", containerSuperBlk.DescriptorNext);
	printf("DataNext             			%u\n", containerSuperBlk.DataNext);
	printf("DescriptorIndex       			%u\n", containerSuperBlk.DescriptorIndex);
	printf("DescriptorLength      			%u\n", containerSuperBlk.DescriptorLength);
	printf("DataIndex              			%u\n", containerSuperBlk.DataIndex);
	printf("DataLength             			%u\n", containerSuperBlk.DataLength);
	printf("SpaceManagerIdent   			%lu\n", containerSuperBlk.SpaceManagerIdent);
	printf("ObjectsMapIdent      			%lu\n", containerSuperBlk.ObjectsMapIdent);
	printf("ReaperIdent              		%lu\n", containerSuperBlk.ReaperIdent);
	printf("ReservedForTesting   			%u\n", containerSuperBlk.ReservedForTesting0);
	printf("MaximumVolumes       			%u\n", containerSuperBlk.MaximumVolumes);

	for (int i = 0; i < 100; ++i)
		if (containerSuperBlk.VolumesIdents[i])
			printf("VolumesIdent %d				%lu ", i, containerSuperBlk.VolumesIdents[i]);
	printf("\n");

	printf("MappingTreeIdent      			%lu\n", containerSuperBlk.MappingTreeIdent);
	printf("OtherFlags          			%lu\n", containerSuperBlk.OtherFlags);
	printf("JumpstartEFI        			%lu\n", containerSuperBlk.JumpstartEFI);
	printf("FusionUuid             			%u\n", containerSuperBlk.FusionUuid);
		
}

/*
   Input Parameters: FILE*, argv 
   Return Type:      APFS_SuperBlk
   Description: Function finds the latest container superblock.
                It loops through the checkpoint array in reverse, since the array is sorted,
				the latest superblock's detail is found in the beginning of the loop.
				Loop continues to fetch the older versions of container superblock.
                
*/
APFS_SuperBlk findValidSuperBlock(FILE *apfs,command_line_args argv)
{
	APFS_SuperBlk containerSuperBlk;
	APFS_BH block_header_chkMap = {0};
	APFS_SuperBlk validContainerSuperBlk;
	APFS_BH valid_block_header_chkMap = {0};
	uint32_t containerSize=0,recentValidSuperblock =0,validSuperBlockAddress =0;
	
	int size=fread(&containerSuperBlk,1,sizeof(containerSuperBlk),apfs);
	// Descriptor base addresss is virtual.
	uint64_t checkpointMappingArray= containerSuperBlk.DescriptorBase* containerSuperBlk.BlockSize;
	fseek(apfs,checkpointMappingArray,SEEK_SET);  // Checkpoint Mapping Array
	    
	//Now fread the checkpoint mapping array. It i a super block array of size 4096 bytes.
	//Need to read the header to get checksum and xid.
	
	for(int checkpointMappingItr = (containerSuperBlk.DescriptorBlocks) - 1;checkpointMappingItr >= 0;checkpointMappingItr--)
	{
		fseek(apfs,checkpointMappingItr*containerSuperBlk.BlockSize,SEEK_CUR);
		

		if ((size = fread(&block_header_chkMap, 1, sizeof(block_header_chkMap), apfs)) < 0) 
		{
			printf("Error reading block_header_chkMap! size = %lu\n",sizeof(block_header_chkMap));
			return containerSuperBlk;
		}

		if(block_header_chkMap.block_type == 1)
		{
			if(block_header_chkMap.version > recentValidSuperblock)
			{
				recentValidSuperblock=block_header_chkMap.version;
				//Now check for checksum;
				
				validSuperBlockAddress = ftell(apfs) - 32;
				valid_block_header_chkMap = block_header_chkMap;
	            containerSize=fread(&validContainerSuperBlk,1,sizeof(validContainerSuperBlk),apfs);
			}
			else{
				if(argv.container == 1)
				printContainerHeader(block_header_chkMap,ftell(apfs) - 32);
			}
		}
		else
		{
			fseek(apfs,checkpointMappingArray,SEEK_SET);

		}
	}
	
	if(argv.container == 1)
	printContainerSuperBlock(containerSuperBlk,validSuperBlockAddress,containerSize,valid_block_header_chkMap);
	return validContainerSuperBlk;
}

/*
   Input Parameters: FILE ,APFS_SuperBlk , int
   Return Type:      omap_phys_t
   Description: Function takes the container superblock and omap id as the parameter,
                and seeks to the omap structure's address, reads the structure and returns it, 
				to be used by findValidVolumeSuperBlock
                
*/
omap_phys_t parseValidContainerSuperBlock(FILE *apfs,APFS_SuperBlk containerSuperBlk, int ObjectsMapIdent )
{	
	omap_phys_t omapStructure;
	//fseek to the omap address.
	fseek(apfs,ObjectsMapIdent * containerSuperBlk.BlockSize,SEEK_SET);
	unsigned int sizeofOmapStructure = 0;
	if ((sizeofOmapStructure = fread(&omapStructure, 1, sizeof(omapStructure), apfs)) < 0) {
		printf("Error reading from apfs File! [%d] size = %lu\n", sizeofOmapStructure, sizeof(omapStructure));
		return omapStructure;
	}
	return omapStructure;
}

void seekNprint(uint64_t blk_num, uint64_t len, FILE *apfs, char *filename)
{
	FILE *op = NULL;
	uint64_t readb = 0;
	char buff[BLK_SIZE] = {0};
	int i = 0, read_size = (BLK_SIZE < len ) ? BLK_SIZE : len;

	if ((op = fopen(filename, "a")) == NULL) {
		printf("Unable to open %s\n", filename);
		return;
	}

	if (fseek(apfs, blk_num * BLK_SIZE, SEEK_SET)) {
		printf("Error Seeking to Extend Block!\n");
		goto end;
	}

	do {
		if (((readb += fread(buff, 1, read_size, apfs)) > 0)) {
			if (fwrite(buff, 1, read_size, op) == 0) {
				printf("Error writing to file!\n");
				break;
			}
			/* Read remaining if less than BLK SIZE */
			read_size = (BLK_SIZE < (len - readb)) ? BLK_SIZE : (len - readb);
		} else {
			printf("Error reading from Extend! Readb = %lu\n", readb);
		}
	} while (readb < len);

	printf("Added %lu Bytes to %s\n", readb, filename);
end:
	fclose(op);
	return;
}

void parseFSObjects(uint8_t fileObjectType,FILE* apfs,uint16_t keyLength,int valueStartAddressforfstree
		, uint16_t valueOffset, uint16_t valueLength, struct fs_obj *ob1)
{	
	char *filename = NULL;
	uint64_t file_size = 0;
	int i = 0;

	if(APFS_TYPE_ANY == fileObjectType)
	{
		printf("The objtype is APFS_TYPE_ANY\n");
	}else if(APFS_TYPE_SNAP_METADATA == fileObjectType){
		printf("The objtype is APFS_TYPE_SNAP_METADATA\n");
	}else if(APFS_TYPE_EXTENT ==fileObjectType){
		printf("The objtype is APFS_TYPE_EXTENT\n");
	}else if(APFS_TYPE_INODE ==fileObjectType){
		printf("The objtype is APFS_TYPE_INODE\n");
		j_inode_val_t inode = {0};
		xf_blob_t blob = {0};
		if (fseek(apfs, valueStartAddressforfstree - valueOffset, SEEK_SET)) {
			printf("Failed to seek to INDOE val!\n");
			return;
		}

		if (fread(&inode, 1, sizeof(inode), apfs) < 0) {
			printf("Error reading INODE!\n");
			return;
		}

		printf(" Parent ID: %lu\n", inode.parent_id);
		printf(" Private ID: %lu\n", inode.private_id);
		printf(" Create time: %lu\n", inode.create_time);
		printf(" MOD Time: %lu\n", inode.mod_time);
		printf(" Change Time: %lu\n", inode.change_time);
		printf(" Access Time: %lu\n", inode.access_time);
		printf(" Internal Flags: %0x\n", inode.internal_flags);
		printf(" Number of Children/links: %d\n", inode.nchildren);
		printf(" Default Protection Class: %0x\n", inode.default_protection_class);
		printf(" BSD Flags: %0x\n", inode.bsd_flags);
		printf(" Owner: %u\n", inode.owner);
		printf(" Group: %u\n", inode.group);
		printf(" Mode: %u\n", inode.mode);
		printf(" Uncompressed size: %lu\n", inode.uncompressed_size);

		if (fread(&blob, 1, sizeof(blob), apfs) < 0) {
			printf("Error reading INODE XF BLOBS!\n");
			return;
		}

		printf("Num Extends %u\n", blob.xf_num_exts);
		printf("Used Data %u\n", blob.xf_used_data);

		x_field_t *xfields = calloc (sizeof(x_field_t),  blob.xf_num_exts);

		if (fread(xfields, sizeof(x_field_t), blob.xf_num_exts, apfs) < 0) {
			printf("Error reading INODE X FIELDS!\n");
			return;
		}

		for (i = 0; i < blob.xf_num_exts; ++i) {
			printf("[%d] Type %u\n", i, xfields[i].x_type);
			printf("[%d] TFlags %0x\n", i, xfields[i].x_flags);
			printf("[%d] TSize %u\n", i, xfields[i].x_size);

			if (INO_EXT_TYPE_NAME == xfields[i].x_type) {
				filename = malloc (xfields[i].x_size);
				if (fread(filename, 1, xfields[i].x_size, apfs) < 0) {
					printf("Error Reading File name from INODE!\n");
				} else {
					printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
					printf("File Name: %s\n", filename);
					printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");
				}
				ob1->filename = filename;

			} else if (INO_EXT_TYPE_DSTREAM == xfields[i].x_type) {
				j_dstream_t dstream = {0};

				if (fread(&dstream, sizeof(j_dstream_t), 1, apfs) < 0) {
					printf("Error reading dstream from INODE!\n");
				} else {
					printf("DATA STREAM:\n");
					file_size = le64toh(dstream.size);
					printf("Size %lu %lu\n", file_size, dstream.size);
					printf("Allocated Size %lu\n", dstream.alloced_size);
					printf("T Bytes Written %lu\n", dstream.total_bytes_written);
					printf("T Bytes Read %lu\n", dstream.total_bytes_read);
				}
			}
		}
		free(xfields);
		// TODO: create obj with obj_id, filename, filesize and in dstream_id check obj_id and read data into file in blks 	
	}else if(APFS_TYPE_XATTR ==fileObjectType){
		j_xattr_key_t fileObjectkey_xattr ={0};
		printf("The objtype is APFS_TYPE_XATTR\n");
	}else if(APFS_TYPE_SIBLING_LINK ==fileObjectType){
		printf("The objtype is APFS_TYPE_SIBLING_LINK\n");
	}else if(APFS_TYPE_DSTREAM_ID ==fileObjectType){
		printf("The objtype is APFS_TYPE_DSTREAM_ID\n");
	}else if(APFS_TYPE_CRYPTO_STATE ==fileObjectType){
		printf("The objtype is APFS_TYPE_CRYPTO_STATE\n");
	}else if(APFS_TYPE_FILE_EXTENT ==fileObjectType){
		printf("The objtype is APFS_TYPE_FILE_EXTENT\n");

		j_file_extent_val_t extend = {0};

		if (fseek(apfs, valueStartAddressforfstree - valueOffset, SEEK_SET)) {
			printf("Failed to seek to INDOE val!\n");
			return;
		}

		if (fread(&extend, 1, sizeof(extend), apfs) < 0) {
			printf("Error Reading Extend!\n");
			return;
		}

		printf("Length = %lu\n", extend.len_and_flags & J_FILE_EXTENT_LEN_MASK);
		printf("Flags = %lu\n", (extend.len_and_flags & J_FILE_EXTENT_FLAG_MASK) >> J_FILE_EXTENT_FLAG_SHIFT);
		printf("Phy Block Num = %0x\n", extend.phys_block_num);
		printf("Crypto ID = %0x\n", extend.crypto_id);
		seekNprint(extend.phys_block_num, extend.len_and_flags & J_FILE_EXTENT_LEN_MASK, apfs, ob1->filename);

	}else if(APFS_TYPE_DIR_REC ==fileObjectType){

		printf("The objtype is APFS_TYPE_DIR_REC %0x\n", ftell(apfs));

		j_drec_hashed_key_t fileObjectkey_dir={0};
		j_drec_val_t  fileObject_dir_value={0};
		char dirName[keyLength - 12];
		int result = fread(&fileObjectkey_dir,1,4,apfs);
		printf("The result of fread is %d",result);
		fread(&dirName,1,sizeof(dirName),apfs);
		dirName[keyLength - 12]='\0';
		// Print the contents of the directory key
		printf(" The directory length is :%d\n",fileObjectkey_dir.name_len_and_hash & J_DREC_LEN_MASK);
		printf("The directory name is:");
		for(int i=0;i<keyLength - 12;i++)
		{
			printf("%c",dirName[i]);
		}
		printf("\n");

		fseek(apfs,valueStartAddressforfstree - valueOffset,SEEK_SET);
		fread(&fileObject_dir_value,1,valueLength,apfs);
		printf("The file id is  %lu\n",fileObject_dir_value.file_id);
		printf("The date added is %lu\n",fileObject_dir_value.date_added);
		printf("The flags for the directory are %" PRIu16 "\n",fileObject_dir_value.flags);
	}else if(APFS_TYPE_DIR_STATS ==fileObjectType){
		printf("The objtype is APFS_TYPE_DIR_STATS\n");
		j_dir_stats_key_t fileObjectkey_dirStat={0};
	}else if(APFS_TYPE_SNAP_NAME ==fileObjectType){
		printf("The objtype is APFS_TYPE_SNAP_NAME\n");
	}else if(APFS_TYPE_SIBLING_MAP ==fileObjectType){
		printf("The objtype is APFS_TYPE_SIBLING_MAP\n");
	}else if(APFS_TYPE_FILE_INFO ==fileObjectType){
		printf("The objtype is APFS_TYPE_FILE_INFO\n");
	}else if(APFS_TYPE_MAX ==fileObjectType){
		printf("The objtype is APFS_TYPE_MAX\n");
	}
}

/*
 * Compares two arrays by byte
 *
 * Returns -1 if the first array is larger
 * Returns 0  if the arrays are identical
 * Returns 1  if the second array is larger
 */
int compArray(uint8_t* firstArray, int firstArrayLen, uint8_t* secondArray, int secondArrayLen)
{
	//Compare the array lengths
	if (firstArrayLen > secondArrayLen)
		return -1;
	else if (firstArrayLen < secondArrayLen)
		return 1;

	//Compare the bytes of the array
	for (int index = 0; index < firstArrayLen; index++)
	{
		if (firstArray[index] > secondArray[index])
			return -1;
		else if (firstArray[index] < secondArray[index])
			return 1;
	}

	return 0;
}

/*
 * Searches for the given key in the BTree
 *
 * Parameters
 * 
 * FILE apfsImage:		A file stream for the APFS disk image
 * uint32_t blockSize:	Typically 4096
 * uint64_t bNodeAddr:	The physical address of the B-Tree
 * uint8_t* searchKey:	A byte array containing the search key
 * uint searchKeyLen:	The length of the key byte array
 * uint8_t* returnVal:  An allocated byte array to store the result in
 * uint returnValueLen:	The expected length of the return value
 * uint64_t& nextBNode:	An address to the next layer in the BTree. May be a physical or virtual address.
 *
 * Return Value
 * 
 * -1:	Key not found or value length doesn't match.
 *  0:	Key found, value is stored in returnValue.
 *  1:	Node is internal and the key may be in a child node. Address to the next BNode is stored in nextBNode.
 *
 * Populates returnVal with:
 * NULL if the value is not found or valueLen does not match the actual value length.
 * The value if it is found.
 */
 
int searchBTree(FILE *apfsImage, uint32_t blockSize, uint8_t *searchKey,uint searchKeyLen,uint returnValueLen, 
                uint64_t *nextBNode,btree_node_phys_t bNodeStruct,int endOfOmapBtree,uint64_t *leafValAddress)
{
	//Read node flags
	int rootNode = bNodeStruct.btn_flags & 1;
	int leafNode = (bNodeStruct.btn_flags >> 1) & 1;
	int fixedSize = (bNodeStruct.btn_flags >> 2) & 1;

	//The table of contents always starts 56 bytes into the structure
	//The node header is 32 bytes and the preceeding fields of the body are 24 bytes
	int tableStartAddr = ftell(apfsImage);
	//The key area starts after the table of contents (and grows downward)
	int keyStartAddr = tableStartAddr + bNodeStruct.btn_table_space.len;
    
	//The value area starts at the end of the node (and grows upward)
	int valueStartAddr =endOfOmapBtree;//bNodeAddr + blockSize;

	//If the node is the root, account for the info trailer 
	if (rootNode)
		valueStartAddr -= sizeof(btree_info_t);


	//Create pointers for both a fixed length and variable length table of contents
	toc_entry_fixed_t *tableEntriesFixed = NULL;
	toc_entry_varlen_t *tableEntriesVarLen = NULL;

	//Read the table of contents into an array
	if (fixedSize)
	{
		int tableSize = bNodeStruct.btn_nkeys * sizeof(toc_entry_fixed_t);
		tableEntriesFixed = malloc(tableSize);
		fread(tableEntriesFixed, 1, tableSize, apfsImage);
	}
	else
	{
		int tableSize = bNodeStruct.btn_nkeys * sizeof(toc_entry_varlen_t);
		tableEntriesVarLen = malloc(tableSize);
		fread(tableEntriesVarLen, 1, tableSize, apfsImage);
	}

	//Iterate through the table
	for (int entry_index = 0; entry_index < bNodeStruct.btn_nkeys; entry_index++)
	{
		uint16_t keyOff, dataOff, keyLen, dataLen;

		//Find the offset and length of the key and value
		if (fixedSize)
		{
			keyOff = tableEntriesFixed[entry_index].key_off;
			dataOff = tableEntriesFixed[entry_index].data_off;
			//Set the key length to the search key length in case the user
			//only wants to compare a portion of the key
			keyLen = searchKeyLen;
			dataLen = returnValueLen;
		}
		else
		{
			keyOff = tableEntriesVarLen[entry_index].key_off;
			dataOff = tableEntriesVarLen[entry_index].data_off;
			keyLen = tableEntriesVarLen[entry_index].key_len;
			dataLen = tableEntriesVarLen[entry_index].data_len;
		}

		//Read the key
		uint8_t key[keyLen];
		int keyAddr = keyStartAddr + keyOff;
		fseek(apfsImage, keyAddr, SEEK_SET);
		fread(&key, 1, keyLen, apfsImage);

		//Compare the two key arrays
		int keyComp = compArray(searchKey, searchKeyLen, key, keyLen);

		//If this node is not a leaf, get the address to the next layer
		if (!leafNode)
		{
			//If the key is smaller than this entry, go to the child of the previous entry
			if (keyComp == 1)
			{
				//If this is the first entry in the BNode, then the search key is not in the Tree
				if (entry_index == 0)
					return -1;
				//Otherwise, read and return the address to the next BNode
				else
				{
					//Find the data offset of the previous entry
					if (fixedSize)
						dataOff = tableEntriesFixed[entry_index-1].data_off;
					else
						dataOff = tableEntriesVarLen[entry_index-1].data_off;

					int valueAddr = valueStartAddr - dataOff;
					fseek(apfsImage, valueAddr, SEEK_SET);
					fread(nextBNode, 1, sizeof(uint64_t), apfsImage);

					return 1;
				}
			}
		}
		//If this node is a leaf, find the search key
		else
		{
			//If the keys match, return the value
			if(keyComp == 0)
			{
				//Return -1 if the value length does not match the expected length
				if (returnValueLen != dataLen)
					return -1;

				//Read the value
				int valueAddr = valueStartAddr - dataOff;
				tApFS_0B_ObjectsMap_Value_t value;
				fseek(apfsImage, valueAddr, SEEK_SET);
				fread(&value,1,16,apfsImage);
				*leafValAddress = value.Address;
				return 0;
			}
		}
	}

	//If no matches were found, the key is not in the B-Tree
	return -1;
}

/*
 * Searches an Omap for the physical address of the object corresponding to the given virtual address
 *
 * Parameters
 * 
 * FILE apfsImage:		A file stream for the APFS disk image
 * uint32_t blockSize:	Typically 4096
 * uint64_t omapAddr:	The physical address of the OMap
 * uint64_t oid:		The virtual address of the object to search for
 *
 * Return Value
 * 
 *  NULL if the OID is not found
 *  A pointer to the omap value struct if the OID is found
 */
 
uint8_t searchOmap(FILE *apfsImage, uint32_t blockSize, uint64_t oid,btree_node_phys_t omapBTree,int endOfOmapBtree,uint64_t *leafValAddress)
{
	//Omap keys are supposed to be 16 bytes (oid and xid)
	//We will ignore the xid and only pass the 8 byte oid
	uint keyLen = 8;
	//The return value is as 16 byte tApFS_0B_ObjectsMap_Value_t struct
	uint valLen = 16;
	uint64_t nextBNode;
	//Search the B-Tree for the key
	int result = searchBTree(apfsImage, blockSize,(uint8_t*)(&oid), keyLen, valLen, &nextBNode,omapBTree,endOfOmapBtree,leafValAddress);

	//Key not found
	if (result == -1)
		return -1;
	//Key found
	else if (result == 0)
		return 0;
	//Key in lower level of B-Tree
	else if (result == 1)
	{
		//Convert the virtual B-Node address to a physical one
		nextBNode *= blockSize;
		fseek(apfsImage,nextBNode,SEEK_SET);
		
	    btree_node_phys_t omapBTree;
	    uint64_t volumeSBAdress=0;
	    int endOfOmapBtree = ftell(apfsImage) + 4096;
	    omapBTree = readAndPrintBtree(apfsImage);  
		
		return searchOmap(apfsImage, blockSize,oid,omapBTree,endOfOmapBtree,&volumeSBAdress);
	}
}

/*
   Input Parameters: FILE* , uint64_t ,APFS_SuperBlk ,command_line_args
   Return Type:      apfs_superblock_t
   Description: Function seeks to the physical address of volume superblock ,
                and prints the details of volume superblock.
				If -v option is given : Details of all Volumes are printed.
				If -v <Vol Id> is given :  ONly that particular Vol Id's details are printed.
                
*/
apfs_superblock_t readAndPrintVolumeSuperBlock(FILE* apfs, uint64_t volumeSBAdress,APFS_SuperBlk containerSuperBlk,command_line_args args)
{
	apfs_superblock_t volumeSuperBlock;
	// Go to the Address of Volume Super Block and print it.
		fseek(apfs,volumeSBAdress * containerSuperBlk.BlockSize,SEEK_SET);
	    unsigned int sizeofVolSuperBlock = 0;
		uint32_t volumeSuperBlkAddress =ftell(apfs);
	    if ((sizeofVolSuperBlock = fread(&volumeSuperBlock, 1, sizeof(volumeSuperBlock), apfs)) < 0) {
		    printf("Error reading from apfs File! [%d] size = %lu\n", sizeofVolSuperBlock, sizeof(volumeSuperBlock));
		    return volumeSuperBlock;
	    }
		if(args.volume == 1 && args.volume_ID == 0 )
		printVolumeSuperBlock(volumeSuperBlock,volumeSuperBlkAddress, sizeofVolSuperBlock);
	    else if (args.volume == 1 && args.volume_ID != 0 )
		{
		  if(args.volume_ID == volumeSuperBlock.apfs_o.o_oid)
			 printVolumeSuperBlock(volumeSuperBlock,volumeSuperBlkAddress, sizeofVolSuperBlock);
		     return volumeSuperBlock;
		}
		return volumeSuperBlock;
}

/*
   Input Parameters: FILE* , omap_phys_t ,APFS_SuperBlk ,command_line_args
   Return Type:      apfs_superblock_t
   Description: 
                
*/

apfs_superblock_t findValidVolumeSuperBlock(FILE *apfs,omap_phys_t omapStructure,APFS_SuperBlk containerSuperBlk,command_line_args args)
{
	apfs_superblock_t volumeSuperBlock;
	// go to tree adress
	fseek(apfs,	omapStructure.om_tree_oid * containerSuperBlk.BlockSize,SEEK_SET);
	btree_node_phys_t omapBTree;

	int startOfValueSection=0;
	int endOfOmapBtree = ftell(apfs) + 4096;
	omapBTree = readAndPrintBtree(apfs);
	uint64_t volumeSBAdress=0;
	uint64_t oid = 0;
	if(args.volume == 1 && args.volume_ID != 0 )
	{
		oid = args.volume_ID;
		uint8_t volSB = searchOmap(apfs, containerSuperBlk.BlockSize,oid,omapBTree,endOfOmapBtree,&volumeSBAdress);
		if( volSB == -1)
		{
			printf(" Volume ID %lu does not exist!!",oid);
			return volumeSuperBlock;
		}
	    volumeSuperBlock = readAndPrintVolumeSuperBlock(apfs,volumeSBAdress,containerSuperBlk,args);
	
	}else if(args.volume == 1 && args.volume_ID == 0 ){
		for(int noOfVolItr =0;noOfVolItr < NX_MAX_FILE_SYSTEMS;noOfVolItr++)
		{
			if(containerSuperBlk.VolumesIdents[noOfVolItr] == 0)
				break;
			else
			{
			oid = containerSuperBlk.VolumesIdents[noOfVolItr];
			uint8_t volSB = searchOmap(apfs, containerSuperBlk.BlockSize,oid,omapBTree,endOfOmapBtree,&volumeSBAdress);
	        readAndPrintVolumeSuperBlock(apfs,volumeSBAdress,containerSuperBlk,args);
	
			}
		}
	}
		
	return volumeSuperBlock;
}

btree_node_phys_t parseAPFSVolumeBlock(FILE *apfs, apfs_superblock_t volumeSuperBlock,APFS_SuperBlk containerSuperBlk,int *endOfOmapBtreeBtree)
{
	// Go to omap id
	fseek(apfs,volumeSuperBlock.apfs_omap_oid * containerSuperBlk.BlockSize,SEEK_SET);
	omap_phys_t omapStructureVol;			
	unsigned int sizeofOmapStructure = 0;
	if ((sizeofOmapStructure = fread(&omapStructureVol, 1, sizeof(omapStructureVol), apfs)) < 0) {
		printf("Error reading from apfs File! [%d] size = %lu\n", sizeofOmapStructure, sizeof(omapStructureVol));
	}
	printf("Parsed the OMAP Structure of Volume Super Block of Size = %u\n", sizeofOmapStructure);
	printf("Tree oid is: %lu\n",omapStructureVol.om_tree_oid);	

	// Go to B tree.
	fseek(apfs,	omapStructureVol.om_tree_oid * containerSuperBlk.BlockSize,SEEK_SET);

	btree_node_phys_t omapBTree;
	int endOfOmapBtree = ftell(apfs) + 4096;
	printf(" EndOfVolumeBTree is: %d",endOfOmapBtree);
	omapBTree = readAndPrintBtree(apfs);

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
	fseek(apfs,16,SEEK_CUR);
	int keyStartAddress = ftell(apfs);
	tApFS_0B_ObjectsMap_Key_t key;
	tApFS_0B_ObjectsMap_Value_t value;
	fread(&key,1,16,apfs);
	printf("The oid and xid are %lu and %lu\n",key.ObjectIdent,key.Transaction);

	// check if it is root node or leaf node
	// find the info address for root node.
	int startOfBTreeInfo = endOfOmapBtree - (40+32);
	printf(" startOfBTreeInfo is: %d\n",startOfBTreeInfo);
	fseek(apfs,startOfBTreeInfo,SEEK_SET);
	fread(&value,1,16,apfs);
	printf("The flag size and physicall address are %u, %u and %x and in decimal %d\n",value.Flags,value.Size,value.Address,value.Address);

	//Go to the Tree inside the VolOmap->Btree->Btree
	fseek(apfs,value.Address * containerSuperBlk.BlockSize,SEEK_SET);
	printf(" Vol Omap->Btree->Btree %d and in hex %x\n",ftell(apfs),ftell(apfs));
	// go to Btree adress

	btree_node_phys_t omapBTreeBtree;
	*endOfOmapBtreeBtree = ftell(apfs) + 4096;
	printf(" EndOfBTree is: %d",*endOfOmapBtreeBtree);
	omapBTreeBtree = readAndPrintBtree(apfs);

	return omapBTreeBtree;
}

void parseFSTree(FILE* apfs,btree_node_phys_t fsBtree,int endOfOmapBtreeBtree)
{
	struct fs_obj ob1 = {0};
	uint64_t pid = 0;

	// Now for the table length loop to get the key value pairs.
	// Check if the kv flag is set to determine if it is kvoff or kloc
	kvloc_t keyValueStructure[fsBtree.btn_table_space.len/8] ;

	for(int i=0;i<fsBtree.btn_table_space.len/8; i++)
	{
		unsigned int sizeofkvloc_t = 0;
		sizeofkvloc_t=fread(&keyValueStructure[i],1,8,apfs);
		if(keyValueStructure[i].k.len !=0 || keyValueStructure[i].v.len != 0)
		{
			printf("Parsed sizeofkvoff_t of table of COntents! Size = %u\n", sizeofkvloc_t);
			printf("The first offset using array is %" PRIu16"\n",keyValueStructure[i].k.off);
			printf("The first offset using array is %" PRIu16"\n",keyValueStructure[i].k.len);		

			printf("btn_data_first_val offset= %d\n",  keyValueStructure[i].v.off);	
			printf("btn_data_first_val length= %d\n",  keyValueStructure[i].v.len);	
		}
	}

	int keyStartAddressBtreeBtree = ftell(apfs);
	int valueStartAddressforfstree = endOfOmapBtreeBtree - 40;
	printf("The size of keyvaluestruct is %d",sizeof(keyValueStructure));
	printf("The fsBtree.btn_table_space.len/8 is %d\n",fsBtree.btn_table_space.len/8);
	for(int h=0;h < (fsBtree.btn_table_space.len/8);h++)
	{

		uint64_t j_key_header=0;

		fseek(apfs,keyStartAddressBtreeBtree,SEEK_SET);	   
		fseek(apfs,keyValueStructure[h].k.off,SEEK_CUR);

		fread(&j_key_header,1,8,apfs);
		// Accessing the object typede
		uint8_t objType=0;
		objType=(j_key_header & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT;
		ob1.id = j_key_header & OBJ_ID_MASK;
		ob1.cflag = (ob1.id == pid);
		printf("##############################################################3\n");
		printf("The object Type is: %d & ID is %lu (at %0x)\n",objType, ob1.id, ftell(apfs));
		parseFSObjects(objType, apfs, keyValueStructure[h].k.len, valueStartAddressforfstree
				, keyValueStructure[h].v.off, keyValueStructure[h].v.len, &ob1);
		pid = ob1.id;
	}//for the table of contents
}

void parse_APFS( int block_no,command_line_args args)
{
	FILE *apfs = NULL;
	APFS_SuperBlk containerSuperBlk;
	omap_phys_t omapStructure;
	apfs_superblock_t volumeSuperBlock;
	btree_node_phys_t fsTree;
	int endOfOmapBtreeBtree=0;
	APFS_SuperBlk super_blk = {0};
	char filename[16] = {0};
	unsigned int size = 0;
	uint32_t blockSize =0;

	snprintf(filename, sizeof(filename), "decompressed%d", block_no);

	if (NULL == (apfs = fopen (filename, "r")) ) {
		printf("Unable to parse APFS: Error opening file %s!\n", filename);
		return;
	}

	if (parse_blk_header(apfs)) {
		printf("Unable to parse APFS: Failed to parse Block header\n");
		return;
	}

	containerSuperBlk=findValidSuperBlock(apfs,args);
	omapStructure = parseValidContainerSuperBlock(apfs,containerSuperBlk, containerSuperBlk.ObjectsMapIdent);
	volumeSuperBlock=findValidVolumeSuperBlock(apfs,omapStructure,containerSuperBlk,args);
	fsTree=parseAPFSVolumeBlock(apfs,volumeSuperBlock,containerSuperBlk,&endOfOmapBtreeBtree);
	parseFSTree(apfs,fsTree,endOfOmapBtreeBtree);
}
