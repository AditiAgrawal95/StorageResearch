#include <stdio.h>
#include "apfs.h"

int parse_blk_header(FILE *apfs)
{
	APFS_BH block_header = {0};
	unsigned int size = 0;

	if ((size = fread(&block_header, 1, sizeof(block_header), apfs)) < 0) {
		printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(block_header));
		return -1;
	}

	printf("\nParsed APFS BLK Header! Size = %u\n", size);
	printf("HEADER\n");
	printf("-------------------------------------\n");
	printf("Checksum = %lu\n", block_header.checksum);
	printf("Block ID = %lu\n", block_header.block_id);
	printf("Version = %lu\n", block_header.version);
	printf("Block Type = 0x%X\n", block_header.block_type);
	printf("Flags = 0x%X\n", block_header.flags);
	printf("Sub Type = 0x%X\n", block_header.sub_type);
	printf("Padding = 0x%X\n", block_header.padding);
	printf("-------------------------------------\n");

	return 0;
}

int parse_super_blk(FILE *apfs)
{
	APFS_SuperBlk super_blk = {0};
	unsigned int size = 0;

	if ((size = fread(&super_blk, 1, sizeof(super_blk), apfs)) < 0) {
		printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(super_blk));
		return -1;
	}

	printf("\nParsed Super Block Header! Size - %u\n", size);
	printf("SUPER BLOCK DATA\n");
	printf("-------------------------------------\n");
	printf("BlockSize - %u\n", super_blk.BlockSize);
	printf("BlocksCount - %lu\n", super_blk.BlocksCount);
	printf("Features - %lu\n", super_blk.Features);
	printf("ReadOnlyFeatures - %lu\n", super_blk.ReadOnlyFeatures);
	printf("IncompatibleFeatures - %lu\n", super_blk.IncompatibleFeatures);
	printf("NextIndent - %lu\n", super_blk.NextIdent);
	printf("NextTransaction- %lu\n", super_blk.NextTransaction);
	printf("Desc Blocks - %u\n", super_blk.DescriptorBlocks);
	printf("Data Blocks - %u\n", super_blk.DataBlocks);
	printf("DescriptorBase - %lu\n", super_blk.DescriptorBase);
	printf("DataBase - %lu\n", super_blk.DataBase);
	printf("DescriptorNext - %u\n", super_blk.DescriptorNext);
	printf("DataNext - %u\n", super_blk.DataNext);
	printf("DescriptorIndex - %u\n", super_blk.DescriptorIndex);
	printf("DescriptorLength - %u\n", super_blk.DescriptorLength);
	printf("DataIndex - %u\n", super_blk.DataIndex);
	printf("DataLength - %u\n", super_blk.DataLength);
	printf("SpaceManagerIdent - %lu\n", super_blk.SpaceManagerIdent);
	printf("ObjectsMapIdent - %lu\n", super_blk.ObjectsMapIdent);
	printf("ReaperIdent - %lu\n", super_blk.ReaperIdent);
	printf("MaximumVolumes - %u\n", super_blk.MaximumVolumes);
	printf("BlockRange Address - %lu\n", super_blk.BlockedOutOfRange.First);
	printf("BlockRange Count - %lu\n", super_blk.BlockedOutOfRange.Count);
	printf("MappingTreeIdent - %lu\n", super_blk.MappingTreeIdent);
	printf("OtherFlags - %lX\n", super_blk.OtherFlags);
	printf("JumpstartEFI Address - %lu\n", super_blk.JumpstartEFI);
	printf("KeyLocker Address - %lu\n", super_blk.KeyLocker.First);
	printf("KeyLocker Count - %lu\n", super_blk.KeyLocker.Count);
	printf("EphemeralInfo 1 - %lu\n", super_blk.EphemeralInfo[0]);
	printf("EphemeralInfo 2 - %lu\n", super_blk.EphemeralInfo[1]);
	printf("EphemeralInfo 3 - %lu\n", super_blk.EphemeralInfo[2]);
	printf("EphemeralInfo 4 - %lu\n", super_blk.EphemeralInfo[3]);
	printf("-------------------------------------\n");
}

void parse_APFS( int block_no )
{
	FILE *apfs = NULL;
	char filename[16] = {0};

	snprintf(filename, sizeof(filename), "decompressed%d", block_no);

	if (NULL == (apfs = fopen (filename, "r")) ) {
		printf("Unable to parse APFS: Error opening file %s!\n", filename);
		return;
	}
	
	if (parse_blk_header(apfs)) {
		printf("Unable to parse APFS: Failed to parse Block header\n");
		return;
	}
	
	if (parse_super_blk(apfs)) {
		printf("Unable to parse APFS: Failed to parse Suoper Block\n");
		return;
	}
}
