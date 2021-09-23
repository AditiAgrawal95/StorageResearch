// DMG.c : This file contains the 'main' function. Program execution begins and ends there.
//
//   Date:18-09-2021 

#include <stdio.h>
#include <string.h>
#include "dmgParser.h"

#if defined(WIN32) || defined(__WIN32) ||defined(__WIN32__) || defined(__NT__) ||defined(_WIN64)    
           #	include <Windows.h>
           #define be64toh(x) _byteswap_uint64(x)    
#elif __linux__
      #	include <endian.h>
#elif __unix__ // all unixes not caught above
      #	include <endian.h>
#endif

FILE* readImageFile(FILE* stream)
{
	char* dmg_path;
 #if defined(WIN32) || defined(__WIN32) ||defined(__WIN32__) || defined(__NT__) ||defined(_WIN64)   
    size_t bufferSize;
    errno_t e=_dupenv_s(&dmg_path, &bufferSize,"bigandsmall");
     if (e || dmg_path == nullptr)
         printf("The path is not found.");     
#endif
    stream = fopen(dmg_path, "r");

    if (stream != 0)
    {
        printf("The file 'bigandsmall' was opened\n");
    }
    else
    {
        printf("The file 'bigandsmall' was not opened\n");
    }
    return stream;
}

FILE* parseDMGTrailer(FILE* stream, UDIFResourceFile* dmgTrailer)
{
    fseek(stream, 0L, SEEK_END);

    int fileSize = ftell(stream);        // calculating the size of the file
 

    int resultOfParse = 0;
    int trailerSize = 512;
    resultOfParse = fseek(stream, fileSize - trailerSize, SEEK_SET);
    if (resultOfParse)
    {
        printf("Couldn't seek to the desired position in the file.\n");
    }
    else
    {
        printf("Seeked to the beginning of Trailer.\n");

        int bytesRead = fread(dmgTrailer, 512, 1, stream);
        printf("The bytes read %d\n", bytesRead);
        
    }
    return stream;
}
FILE* readXMLOffset(FILE* stream, UDIFResourceFile* dmgTrailer, char** plist)
{
    fseek(stream, be64toh(dmgTrailer->XMLOffset), SEEK_SET);     // Seek to the offset of xml.   
    *plist = (char*)malloc(be64toh(dmgTrailer->XMLLength));      // Allocate memory to plist char array.
    fread(*plist, be64toh(dmgTrailer->XMLLength), 1, stream);    // Read the xml.

    printf("The xml is : %s", *plist);
    return stream;
}

void parseXML()
{
    // ToDo By Alex
}


BLKXTable* decodeDataBlk(const char* data)
{
    size_t decode_size = strlen(data);
    printf("The size of data is : %i", decode_size);
    unsigned char* decoded_data = base64_decode(data, decode_size, &decode_size);
    printf("DEcode structure is %s\n", decoded_data);

    BLKXTable* dataBlk = NULL;
    dataBlk = (BLKXTable*)decoded_data;

    // we can remove these print statements later. Keeping them for reference.
    printf("Decoded Sector count  from dataBlk : %lu \n", be64toh(dataBlk->SectorCount));
    printf("The value of version is: %u\n", be32toh(dataBlk->Version));
    printf("The value of chunk is: %lu\n", be64toh(dataBlk->chunk[1].SectorNumber));

    return dataBlk;
}

// This function will be called by printdmgBlocks
void readDataBlks(const char* data,FILE* stream)
{
    BLKXTable* dataBlk = NULL;  
    dataBlk = decodeDataBlk(data);
	//fseek(stream,compressedOffset,SEEK_SET);
	
}
int main()
{
    FILE* stream = NULL;
    UDIFResourceFile dmgTrailer;
    char * plist;

    stream = readImageFile(stream);
    parseDMGTrailer(stream, &dmgTrailer);
    readXMLOffset(stream,&dmgTrailer,&plist);
    //parseXML();
	
       
    

    free(plist);
	return 0;
}


