#include <stdlib.h>
#include <stdio.h>
#include "apfs.h"

APFS_BH* parse_blk_header(FILE *apfs)
{
	APFS_BH *block_header = (APFS_BH*) malloc(sizeof(APFS_BH));
	unsigned int size = 0;

	if ((size = fread(block_header, 1, sizeof(*block_header), apfs)) < 0) {
		printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(*block_header));
		return NULL;
	}

	printf("\nParsed APFS BLK Header! Size = %u\n", size);
	printf("HEADER\n");
	printf("-------------------------------------\n");
	printf("Checksum = %lu\n", block_header->checksum);
	printf("Block ID = %lu\n", block_header->block_id);
	printf("Version = %lu\n", block_header->version);
	printf("Block Type = 0x%X\n", block_header->block_type);
	printf("Flags = 0x%X\n", block_header->flags);
	printf("Sub Type = 0x%X\n", block_header->sub_type);
	printf("Padding = 0x%X\n", block_header->padding);
	printf("-------------------------------------\n");

	return block_header;
}

APFS_SuperBlk* parse_super_blk(FILE *apfs)
{
	APFS_SuperBlk *super_blk = (APFS_SuperBlk*) malloc(sizeof(APFS_SuperBlk));
	unsigned int size = 0;

	if ((size = fread(super_blk, 1, sizeof(*super_blk), apfs)) < 0) {
		printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(*super_blk));
		return NULL;
	}

	printf("\nParsed Super Block Header! Size - %u\n", size);
	printf("SUPER BLOCK DATA\n");
	printf("-------------------------------------\n");
	printf("BlockSize - %u\n", super_blk->BlockSize);
	printf("BlocksCount - %lu\n", super_blk->BlocksCount);
	printf("Features - %lu\n", super_blk->Features);
	printf("ReadOnlyFeatures - %lu\n", super_blk->ReadOnlyFeatures);
	printf("IncompatibleFeatures - %lu\n", super_blk->IncompatibleFeatures);
	printf("NextIndent - %lu\n", super_blk->NextIdent);
	printf("NextTransaction- %lu\n", super_blk->NextTransaction);
	printf("Desc Blocks - %u\n", super_blk->DescriptorBlocks);
	printf("Data Blocks - %u\n", super_blk->DataBlocks);
	printf("DescriptorBase - %lu\n", super_blk->DescriptorBase);
	printf("DataBase - %lu\n", super_blk->DataBase);
	printf("DescriptorNext - %u\n", super_blk->DescriptorNext);
	printf("DataNext - %u\n", super_blk->DataNext);
	printf("DescriptorIndex - %u\n", super_blk->DescriptorIndex);
	printf("DescriptorLength - %u\n", super_blk->DescriptorLength);
	printf("DataIndex - %u\n", super_blk->DataIndex);
	printf("DataLength - %u\n", super_blk->DataLength);
	printf("SpaceManagerIdent - %lu\n", super_blk->SpaceManagerIdent);
	printf("ObjectsMapIdent - %lu\n", super_blk->ObjectsMapIdent);
	printf("ReaperIdent - %lu\n", super_blk->ReaperIdent);
	printf("MaximumVolumes - %u\n", super_blk->MaximumVolumes);
	printf("BlockRange Address - %lu\n", super_blk->BlockedOutOfRange.First);
	printf("BlockRange Count - %lu\n", super_blk->BlockedOutOfRange.Count);
	printf("MappingTreeIdent - %lu\n", super_blk->MappingTreeIdent);
	printf("OtherFlags - %lX\n", super_blk->OtherFlags);
	printf("JumpstartEFI Address - %lu\n", super_blk->JumpstartEFI);
	printf("KeyLocker Address - %lu\n", super_blk->KeyLocker.First);
	printf("KeyLocker Count - %lu\n", super_blk->KeyLocker.Count);
	printf("EphemeralInfo 1 - %lu\n", super_blk->EphemeralInfo[0]);
	printf("EphemeralInfo 2 - %lu\n", super_blk->EphemeralInfo[1]);
	printf("EphemeralInfo 3 - %lu\n", super_blk->EphemeralInfo[2]);
	printf("EphemeralInfo 4 - %lu\n", super_blk->EphemeralInfo[3]);
	printf("-------------------------------------\n");

	return super_blk;
}

void parse_APFS( int block_no )
{
	FILE *apfs = NULL;
	char filename[16] = {0};

	APFS_BH *super_block_header;
	APFS_SuperBlk *super_blk;
	APFS_BH *omap_header;

	snprintf(filename, sizeof(filename), "decompressed%d", block_no);

	if (NULL == (apfs = fopen (filename, "r")) ) {
		printf("Unable to parse APFS: Error opening file %s!\n", filename);
		return;
	}
	
	if (NULL == (super_block_header = parse_blk_header(apfs))) {
		printf("Unable to parse APFS: Failed to parse Block header\n");
		return;
	}
	
	if (NULL == (super_blk = parse_super_blk(apfs))) {
		printf("Unable to parse APFS: Failed to parse Suoper Block\n");
		return;
	}


	uint32_t block_size = super_blk->BlockSize;
	//Seek to the location of the Objects Map
	uint64_t omap_indent = super_blk->ObjectsMapIdent;
	uint64_t byte_addr = omap_indent * block_size;
	
	if (fseek(apfs, byte_addr, SEEK_SET)) {
		printf("Couldn't seek to the desired position in the file.\n");
	}
	
	if (NULL == (omap_header = parse_blk_header(apfs))) {
		printf("Unable to parse APFS: Failed to parse Block header\n");
		return;
	}
	
}
