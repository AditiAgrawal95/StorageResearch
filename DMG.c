// DMG.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <string.h>
#include "dmgParser.h"
FILE* stream;
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

FILE* parseDMGTrailer(FILE* stream)
{
    UDIFResourceFile trailer1;
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

        int bytesRead = fread(&trailer1, 512, 1, stream);
        printf("The bytes read %d\n", bytesRead);

        printf("The magic signature is %s\n", trailer1.Signature);
        printf("The xmloffset is %llu\n", trailer1.XMLOffset);
        printf("The xmllength is %llu\n", trailer1.XMLLength);
        printf("The version is %zu\n", trailer1.Version);
        printf("The HeaderSize is %zu\n", trailer1.HeaderSize);

        printf("%" PRIu32 "\n", trailer1.Version);

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


