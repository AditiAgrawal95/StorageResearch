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
	printf("Parsed APFS BLK Header! Size = %u\n", size);

	printf("Checksum = %lu\n", block_header.checksum);
	printf("Block ID = %lu\n", block_header.block_id);
	printf("Version = %lu\n", block_header.version);
	printf("Block Type = %u\n", block_header.block_type);
	printf("Flags = %u\n", block_header.flags);
	printf("Padding = %u\n", block_header.padding);

	return 0;
}

void parse_APFS( int block_no )
{
	FILE *apfs = NULL;
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

	if ((size = fread(&super_blk, 1, sizeof(super_blk), apfs)) < 0) {
        	printf("Error reading from APFS File! [%d] size = %lu\n", size, sizeof(super_blk));
                return;
        }
	printf("Parsed Super Block Header! Size - %u\n", size);

}
