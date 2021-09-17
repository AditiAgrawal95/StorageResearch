// DMG.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <string.h>
#include "dmgParser.h"
FILE* stream;
FILE* readImageFile(FILE* stream)
{
 
    errno_t err;
    err = fopen_s(&stream, "bigandsmall.dmg", "r");

    if (err == 0)
    {
        printf("The file 'bigandsmall' was opened, %d\n", err);
    }
    else
    {
        printf("The file 'bigandsmall' was not opened\n");
    }
    return stream;
}

FILE* parseDMGTrailer(FILE* stream)
{

    fseek(stream, 0L, SEEK_END);
    // calculating the size of the file
    fileSize = ftell(stream);
    printf("The size of the file is %d\n", fileSize);

    int resultOfParse = 0;
    int trailerSize = 512;
    resultOfParse = fseek(stream, fileSize - trailerSize, SEEK_SET);
    if (resultOfParse)
    {
        printf("The The file didnt seek right to the trailer %d\n", resultOfParse);
    }
    else
    {
        printf("The file seeked to right position  %d and ftell is %d\n", resultOfParse,ftell(stream));
        uint8_t magic[4] = { "" };
        fread(magic, 1, 4, stream);
        printf("The magic signature is %s", magic);

        fseek(stream, 200, SEEK_CUR);
        printf("The ftell to get the xmlofffset is %d:", ftell(stream));
        uint64_t xmloffset;
        printf("The size of long is %d", sizeof(uint64_t));
        fread(&xmloffset, 8, 1, stream);
        printf("Printing the offset of xml %llu", xmloffset);
        
        fread(&xmllength, 8, 1, stream);
        printf("Printing the length of xml %lld", xmllength);

        //seeking to the xmloffset.
        fseek(stream, xmloffset, SEEK_SET);
    }
    return stream;
}
FILE* parseXMLFile(FILE* stream,uint64_t xmllen)
{
    char plistdata[5435576] = { " " };
    fread(plistdata, xmllen, 1, stream);
    // Call the parser
    return stream;
}

int main()
{

	printf("Hello world");
    FILE* stream = NULL;
    stream = readImageFile(stream);
    parseDMGTrailer(stream);
    parseXMLFile(stream,xmllength);
	return 0;
}


