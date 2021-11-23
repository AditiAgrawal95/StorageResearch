// DMG.c : This file contains the 'main' function. Program execution begins and ends there.
//
//   Date:18-09-2021 

#include <stdio.h>
#include <zlib.h>
#include "dmgParser.h"
#include "apfs.h"
#include "pList.h"

//Portable endianness conversion
#if defined(WIN32) || defined(__WIN32) ||defined(__WIN32__) || defined(__NT__) ||defined(_WIN64)    
    #include <Windows.h>
    #define be64toh(x) _byteswap_uint64(x)    
#elif __linux__
    #include <endian.h>
#elif __unix__ // all unixes not caught above
    #include <endian.h>
#endif

#define INFLATE_BUF_LEN 4096
#define ENTRY_TYPE_ZLIB	0x80000005

FILE* readImageFile(FILE* stream, char* dmg_path)
{
#if defined(WIN32) || defined(__WIN32) ||defined(__WIN32__) || defined(__NT__) ||defined(_WIN64)   
    size_t bufferSize;
    errno_t e=_dupenv_s(&dmg_path, &bufferSize,"bigandsmall");

    if (e || dmg_path == nullptr)
        printf("The path is not found.");     
#endif

    stream = fopen(dmg_path, "r");

    if (stream == 0)
    {
        printf("Failed to open the file '%s'\n", dmg_path);
    }

    return stream;
}

FILE* parseDMGTrailer(FILE* stream, UDIFResourceFile* dmgTrailer)
{
    int trailerSize = 512;

    if (fseek(stream, -trailerSize, SEEK_END)) {
        printf("Couldn't seek to the desired position in the file.\n");
    }
    else {
        int bytesRead = fread(dmgTrailer, trailerSize, 1, stream);
    }

    return stream;
}

FILE* readXMLOffset(FILE* stream, UDIFResourceFile* dmgTrailer, char** plist)
{
    fseek(stream, be64toh(dmgTrailer->XMLOffset), SEEK_SET);     // Seek to the offset of xml.   
    *plist = (char*)malloc(be64toh(dmgTrailer->XMLLength));      // Allocate memory to plist char array.
    fread(*plist, be64toh(dmgTrailer->XMLLength), 1, stream);    // Read the xml.

    return stream;
}

BLKXTable* decodeDataBlk(const char* data)
{
    size_t decode_size = strlen(data);
    unsigned char* decoded_data = base64_decode(data, decode_size, &decode_size);

    BLKXTable* dataBlk = NULL;
    dataBlk = (BLKXTable*)decoded_data;

    return dataBlk;
}

int decompress_to_file(char *compressed, unsigned long comp_size, char* filename)
{
    z_stream inflate_stream = {0};
    FILE *output_buf = NULL;
    char decompressed[INFLATE_BUF_LEN] = {0};
	unsigned long remaining = 0;
    int ret = 0;

    inflate_stream.zalloc = Z_NULL;
    inflate_stream.zfree  = Z_NULL;
    inflate_stream.opaque = Z_NULL;

    inflate_stream.next_in  = compressed;
    inflate_stream.avail_in = comp_size;

    if ( (ret = inflateInit(&inflate_stream)) != Z_OK ) {
            printf("Error Initializing Inflate! [Ecode - %d]\n", ret);
            return -1;
    }

    inflate_stream.next_out  = decompressed;
    inflate_stream.avail_out = INFLATE_BUF_LEN;

    if ((output_buf = fopen(filename, "a")) == NULL) {
        printf("Unable to create file decompressed!\n");
        ret = -1;
        goto inflate_end;
    }

    while ((ret = inflate(&inflate_stream,  Z_FINISH )) != Z_STREAM_END) {
        if ((ret == Z_OK || ret == Z_BUF_ERROR ) && inflate_stream.avail_out == 0) {
                if (fwrite(decompressed, 1, INFLATE_BUF_LEN, output_buf) == 0) {
                    printf("Error Writing to output file!\n");
                    goto close;
                }

                memset(decompressed, 0, sizeof(decompressed));
                inflate_stream.next_out  = decompressed;
                inflate_stream.avail_out = INFLATE_BUF_LEN;
        }
        else {
			printf("Error Inflating! [Code - %d]\n", ret);
			goto close;
		}
	}

	remaining = inflate_stream.total_out % INFLATE_BUF_LEN;
	if (remaining == 0 && inflate_stream.total_out > 0)
		remaining = INFLATE_BUF_LEN;
		
	if (fwrite(decompressed, 1, remaining, output_buf) == 0)
		printf("Error Writing to output file!\n");

close:
	fclose(output_buf);
inflate_end:
	inflateEnd (&inflate_stream);
	return ret;
}

    
// This function will be called by printdmgBlocks
void readDataBlks(BLKXTable* dataBlk, FILE* stream, char* filename)
{
	int sectorSize = 512;
	uint8_t* compressedBlk = NULL; 

    for (int noOfChunks = 0; noOfChunks < (be32toh(dataBlk->NumberOfBlockChunks)); noOfChunks++)
    {
	    fseek(stream, be64toh(dataBlk->chunk[noOfChunks].CompressedOffset), SEEK_SET);

	    compressedBlk = (uint8_t*)malloc(be64toh(dataBlk->chunk[noOfChunks].CompressedLength));
	    size_t result=fread(compressedBlk, 1, be64toh(dataBlk->chunk[noOfChunks].CompressedLength), stream);

	    //Only supports zlib compression
	    //To-Do: support other compression types
        if (be32toh(dataBlk->chunk[noOfChunks].EntryType) == ENTRY_TYPE_ZLIB) {
	        if (decompress_to_file(compressedBlk, result, filename) < 0) {
	            printf("Failed to decompress block\n");
	        }
        }
    }
}

int checkCommandLineArguments(char** argv, int argc)
{
	int result = 0;
	if (argc < 2) {
    	printf("Not Enough Arguments!\n");
    	return 1;
    }
    else if(argc > 5) {
		printf("Excess arguments entered!\n");
		result =1;
	}
	else if( argc == 3) {
		if(strcmp(argv[2],"-c") != 0 && strcmp(argv[2],"-C") != 0 && strcmp(argv[2],"-v") != 0 && strcmp(argv[2],"-V") != 0)
		{
			printf("Wrong parameter entered!\n");
			result =1;
		}
	}
	else if (argc == 4) {
		if( strcmp(argv[2],"-v") == 0 || strcmp(argv[2],"-V") == 0)
		{
			// Make sure that argument contains nothing but digits
            for (int i = 0; argv[3][i] != '\0'; i++)
            {
                if (!isdigit(argv[3][i]))
                {
                    printf("Volume id has to be an interger!\n");
					result =1;
                    break;
                }//if
            }//for
		}//if
	}
	else if(argc == 5 && (strcmp(argv[4],"-fs") != 0 && strcmp(argv[4],"-FS") != 0)) {
		printf("Wrong parameter entered for Volume's file structure! \n");
		result=1;
	}

	return result;
}

void printUsage()
{
	printf("Usage :              apfs_parser <DMG_FILE>               Prints the Disk Image Structure\n \
	                          -c                      Container Superblock Information\n \
                                 -v [ Volume ID ]        All Vol SuperBlock Information | Specified Volume's Information \n \
                                 -f <file_name>          Displays file content\n \
                                 -v <Volume_ID> -fs      Displays File system Structure\n");	
}

command_line_args fillCommandLineArguments(char **argv,int argc)
{
	command_line_args args={0};
	if( strlen(argv[2]) > 0 )
	{
		if(strcmp(argv[2],"-c") == 0 || strcmp(argv[2],"-C") == 0) {
		    args.container = 1;
		}
	    else if(strcmp(argv[2],"-v") == 0 || strcmp(argv[2],"-V") == 0) {
	        args.volume = 1;
			if(argc == 4)
			{
				args.volume_ID = atoi(argv[3]);
			}
		}
		else {
			args.file = 1;
		     if(argc == 4)
			{
				strncpy(args.file_name,argv[3],strlen(argv[3]));
			}
		}
	}

	if( argc == 5 )
		args.fs_structure = 1;
	
	return args;
}

int main(int argc, char** argv)
{
    FILE* stream = NULL;
    UDIFResourceFile dmgTrailer;
    char *plist;
    command_line_args args;

    // check command line arguments 
	int result = checkCommandLineArguments( argv , argc);

	if(result == 1)
	{
		printUsage();
		return 1;
	}
	else{
		args=fillCommandLineArguments(argv,argc);
	}

    stream = readImageFile(stream, argv[1]);
    parseDMGTrailer(stream, &dmgTrailer);      // reference of dmgTrailer is passed.
    readXMLOffset(stream,&dmgTrailer,&plist);  //reference of dmgTrailer and plist is passed
    char* apfsData = parsePlist(plist, stream);  //Parse the pList and find the APFS data block
	
	if (apfsData != NULL)
	{
		char *filename = "APFS Image Decompressed";

		printf("\nDecompressing DMG file...\n\n");

		//Decompress the APFS data blocks
		BLKXTable* dataBlk = NULL;
		dataBlk = decodeDataBlk(apfsData);   // decode the data block.
		readDataBlks(dataBlk, stream, filename);    // loop through the chunks to decompress each.

		//Parse the APFS image
		parse_APFS(args, filename);
	}
	else
	{
		printf("Failed to parse the DMG pList\n");
	}

	free(apfsData);
    free(plist);

    printf("\n");

    return 0;
}


