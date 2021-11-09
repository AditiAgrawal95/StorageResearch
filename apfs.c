#include <stdio.h>
#include <time.h>
#include <endian.h>
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

btree_node_phys_t readAndPrintBtree(FILE* apfs)
{
	btree_node_phys_t btree;
	unsigned int sizeofTree = 0;
	if ((sizeofTree = fread(&btree, 1, sizeof(btree), apfs)) < 0) {
		printf("Error reading from apfs File! [%d] size = %lu\n", sizeofTree, sizeof(btree));
		return btree;
	}
	printf("Parsed Omap's B tree! Size = %u\n", sizeofTree);
	printf("We are at %d and in hex we are after reading the omap 56 bytes is %x\n",ftell(apfs),ftell(apfs));	

	printf("btn_flags = %lu\n", btree.btn_flags);
	printf(" count is :%" PRIu16 "\n",btree.btn_level);
	printf("btn_nkeys = %u\n", btree.btn_nkeys);
	printf("btn_table_space offset= %d\n", btree.btn_table_space.off);
	printf("btn_table_space length = %d\n", btree.btn_table_space.len);
	printf("btn_key_free_list offset= %d\n", btree.btn_key_free_list.off);
	printf("btn_key_free_list length = %d\n", btree.btn_key_free_list.len);
	printf("btn_val_free_list offset= %d\n", btree.btn_val_free_list.off);
	printf("btn_val_free_list length = %d\n", btree.btn_val_free_list.len);
	return btree;
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

apfs_superblock_t findValidVolumeSuperBlock(FILE *apfs,omap_phys_t omapStructure,APFS_SuperBlk containerSuperBlk)
{
	apfs_superblock_t volumeSuperBlock;
	// go to tree adress
	fseek(apfs,	omapStructure.om_tree_oid * containerSuperBlk.BlockSize,SEEK_SET);

	btree_node_phys_t omapBTree;
	int endOfOmapBtree = ftell(apfs) + 4096;
	printf(" EndOfBTree is: %d",endOfOmapBtree);
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
	//Add the loop for all volumes
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
	printf("APFS Omap ID %lu\n",volumeSuperBlock.apfs_omap_oid);


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


// Visit every node in the given Omap B-Tree
//
// apfs:			The file object of the disk image
// bTreeAddress:	The byte address of the target B-Tree in blocks
// blockSize:		The number of bytes in each block
void traverseOmapTree(FILE *apfs, int bTreeAddress, uint32_t blockSize)
{
	//Seek to the B-Tree node
	fseek(apfs, bTreeAddress, SEEK_SET);

	//Read the node struct
	btree_node_phys_t bTreeRoot;
	fread(&bTreeRoot, 1, sizeof(bTreeRoot), apfs);

	//Assert that the B-Tree has an Omap subtype
	if (bTreeRoot.btn_o.o_subtype != 11)
	{
		printf("The B-Tree at address 0x%X is not an Omap sybtype", bTreeAddress);
		return;
	}

	//Read node flags
	int rootNode = bTreeRoot.btn_flags & 1;
	int leafNode = (bTreeRoot.btn_flags >> 1) & 1;
	int fixedSize = (bTreeRoot.btn_flags >> 2) & 1;

	//Assert that the keys and values are a fixed size
	if (fixedSize == 0)
	{
		printf("The Omap B-Tree at address 0x%X does not have fixed size keys and values", bTreeAddress);
		return;
	}

	//The table of contents always starts 56 bytes into the structure
	//The node header is 32 bytes and the preceeding fields of the body are 24 bytes
	int tableStartAddr = bTreeAddress + 56;
	//Assuming every node occupies one block, the node info is at the end of the block
	int nodeInfoSize = sizeof(btree_info_t);
	int nodeInfoStartAddr = bTreeAddress + blockSize - nodeInfoSize;

	//Read the node info struct
	btree_info_t nodeInfo;
	fseek(apfs, nodeInfoStartAddr, SEEK_SET);
	fread(&nodeInfo, 1, nodeInfoSize, apfs);

	//The key area starts after the table of contents (and grows downward)
	int keyStartAddr = tableStartAddr + bTreeRoot.btn_table_space.len;
	//The value area starts starts the byte before the trailer (and grows upward)
	int valueStartAddr = nodeInfoStartAddr - 1;

	//Read the table of contents into an array
	toc_entry_t tableEntries[bTreeRoot.btn_nkeys];
	int tableSize = bTreeRoot.btn_nkeys * sizeof(toc_entry_t);
	fseek(apfs, tableStartAddr, SEEK_SET);
	fread(&tableEntries, 1, tableSize, apfs);

	//Iterate through the table
	for (int entry = 0; entry < bTreeRoot.btn_nkeys; entry++)
	{
		printf("Key Offset: %hu\t", tableEntries[entry].key_off);
		printf("Data Offset: %hu\n", tableEntries[entry].data_off);
	}
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

void parse_APFS( int block_no )
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
	fsTree=parseAPFSVolumeBlock(apfs,volumeSuperBlock,containerSuperBlk,&endOfOmapBtreeBtree);
	parseFSTree(apfs,fsTree,endOfOmapBtreeBtree);

	///////////////////////////
	//  B-Tree Parsing Test  //
	///////////////////////////

	//To-Do: Find the B-Tree address properly without hard coding
	//Address of root B-Tree node in Many Files.dmg
	int bTreeBlockAddr = 0x151B;
	int bTreeByteAddr = bTreeBlockAddr * containerSuperBlk.BlockSize;

	printf("\n\nB-TREE TRAVERSAL TEST\n");
	printf("---------------------\n");
	traverseOmapTree(apfs, bTreeByteAddr, containerSuperBlk.BlockSize);
	printf("---------------------\n\n");
}
