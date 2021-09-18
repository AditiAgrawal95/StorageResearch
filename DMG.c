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
 
    stream = fopen("bigandsmall.dmg", "r");

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

    fileSize = ftell(stream);        // calculating the size of the file
 

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

int main()
{
    FILE* stream = NULL;
    UDIFResourceFile dmgTrailer;
    char * plist;

    stream = readImageFile(stream);
    parseDMGTrailer(stream, &dmgTrailer);      // reference of dmgTrailer is passed.
    readXMLOffset(stream,&dmgTrailer,&plist);  //reference of dmgTrailer and plist is passed
    parseXML();
	return 0;
}


