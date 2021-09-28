// DMG.c : This file contains the 'main' function. Program execution begins and ends there.
//
//   Date:18-09-2021 

#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "dmgParser.h"

#if defined(WIN32) || defined(__WIN32) ||defined(__WIN32__) || defined(__NT__) ||defined(_WIN64)    
           #	include <Windows.h>
           #define be64toh(x) _byteswap_uint64(x)    
#elif __linux__
      #	include <endian.h>
#elif __unix__ // all unixes not caught above
      #	include <endian.h>
#endif

FILE* readImageFile(FILE* stream, char* dmg_path)
{
 #if defined(WIN32) || defined(__WIN32) ||defined(__WIN32__) || defined(__NT__) ||defined(_WIN64)   
    size_t bufferSize;
    errno_t e=_dupenv_s(&dmg_path, &bufferSize,"bigandsmall");
     if (e || dmg_path == nullptr)
         printf("The path is not found.");     
#endif
    stream = fopen(dmg_path, "r");

    if (stream != 0)
    {
        printf("The file '%s' was opened\n", dmg_path);
    }
    else
    {
        printf("The file '%s' was not opened\n", dmg_path);
    }
    return stream;
}

FILE* parseDMGTrailer(FILE* stream, UDIFResourceFile* dmgTrailer)
{
    int trailerSize = 512, resultOfParse = 0;

    resultOfParse = fseek(stream, -trailerSize, SEEK_END);

    if (resultOfParse)
    {
        printf("Couldn't seek to the desired position in the file.\n");
    }
    else
    {
        // printf("Seeked to the beginning of Trailer.\n");

        int bytesRead = fread(dmgTrailer, trailerSize, 1, stream);
        // printf("The bytes read %d\n", bytesRead);
        
    }
    return stream;
}

FILE* readXMLOffset(FILE* stream, UDIFResourceFile* dmgTrailer, char** plist)
{
    fseek(stream, be64toh(dmgTrailer->XMLOffset), SEEK_SET);     // Seek to the offset of xml.   
    *plist = (char*)malloc(be64toh(dmgTrailer->XMLLength));      // Allocate memory to plist char array.
    fread(*plist, be64toh(dmgTrailer->XMLLength), 1, stream);    // Read the xml.

    //printf("The xml is : %s", *plist);
    return stream;
}


//Searches through the siblings of a node for the given type
xmlNode* findSiblingByType(xmlNode *node, char* type)
{
	const xmlChar *nodeType = (const xmlChar*)type;

	//Advance the pointer to its sibling
	node = node->next;

	while (node != NULL && !xmlStrEqual(node->name, nodeType))
		node = node->next;

	return node;
}

//Searches through the siblings of a node for the given content
xmlNode* findSiblingByContent(xmlDoc *doc, xmlNode *node, char* content)
{
	const xmlChar *nodeContent = (const xmlChar*)content;

	for(xmlNode* sibling = node->next; sibling != NULL; sibling = sibling->next)
	{
		if (sibling->type == XML_ELEMENT_NODE)
		{
			const xmlChar *nodeText = xmlNodeListGetString(doc, sibling->children, 1);
		
			if (xmlStrEqual(nodeText, nodeContent))
				return sibling;
		}

	}

	return NULL;
}

//A breadth-first search for an xmlNode with the given contents
xmlNode* findNodeByText(xmlDoc *doc, xmlNode *root, char *searchText)
{
	//Search all siblings
	xmlNode* sibling = findSiblingByContent(doc, root, searchText);

	if (sibling != NULL)
		return sibling;

	//Search all children
	for (xmlNode* sibling = root; sibling != NULL; sibling = sibling->next)
	{
		if (sibling->type == XML_ELEMENT_NODE)
		{
			xmlNode* node = findNodeByText(doc, sibling->children, searchText);

			if (node != NULL)
				return node;
		}
	}

	//Item not found in this branch
	return NULL;
}

char* removeWhiteSpace(char* string)
{
	char* copy;
	int length = strlen(string);
	int wsCount = 0;
	int newLength;

	int strIndex = 0;
	int copyIndex = 0;

	//Count the number of white spaces
	for (strIndex = 0; strIndex < length; strIndex++)
	{
		if (string[strIndex] == ' ' ||
			string[strIndex] == '\t')
			wsCount++;
	}

	//Allocate memory for a new string
	newLength = length - wsCount;
	copy = (char*) malloc(newLength * sizeof(char));

	//Copy all non-whitespace characters
	for (strIndex = 0; strIndex < length; strIndex++)
	{
		if (string[strIndex] != ' ' &&
			string[strIndex] != '\t')
		{
			copy[copyIndex] = string[strIndex];
			copyIndex++;
		}
	}

	return copy;
}

int printDmgBlocks(xmlDoc *doc, xmlNode *blkxNode)
{
	xmlNode *array = findSiblingByType(blkxNode, "array");

	if (array == NULL) {
		printf("plist format is incorrect");
		return 1;
	}

	xmlNode *block = findSiblingByType(array->children, "dict");

	if (block == NULL) {
		printf("plist format is incorrect");
		return 1;
	}

	//Print all blocks
	for (int i = 0; block != NULL; i++)
	{
		printf("\nBlock %i\n", i);

		//Find the CFName key
		xmlNode *node = findSiblingByContent(doc, block->children, "CFName");

		if (node == NULL) {
			printf("plist format is incorrect");
			return 1;
		}

		//Find the CFName content node
		node = findSiblingByType(node, "string");

		if (node == NULL) {
			printf("plist format is incorrect");
			return 1;
		}

		//Print the block CFName
		const xmlChar *name = xmlNodeListGetString(doc, node->children, 1);
		printf("CFName: %s\n", name);

		//Find the data node
		node = findSiblingByType(node, "data");

		if (node == NULL) {
			printf("plist format is incorrect");
			return 1;
		}

		//Print the block data
		char *data = xmlNodeListGetString(doc, node->children, 1);
		char *dataNoWs = removeWhiteSpace(data);
		printf("Data:\n %s\n", dataNoWs);
		free(data);
		free(dataNoWs);

		//Go to the next block
		block = findSiblingByType(block, "dict");
	}
}

//Adapted from the article:
//https://www.developer.com/database/libxml2-everything-you-need-in-an-xml-library/
void parseXML(char* xmlStr)
{
	xmlDoc *doc = NULL;
	xmlNode *root = NULL;

	//Parse the given string into an XML file
    doc = xmlParseDoc(xmlStr);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", xmlStr);
        return;
    }

    root = xmlDocGetRootElement(doc);

    //Get the blkx node
	xmlNode *blkx = findNodeByText(doc, root, "blkx");

	if (blkx == NULL) {
        printf("error: DMG plist is formatted incorrectly\n");
        return;
	}

	//Print the plist blocks
	printDmgBlocks(doc, blkx);

    //Free any libXML2 memory
    xmlFreeDoc(doc);
    xmlCleanupParser();
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
int main(int argc, char** argv)
{
    FILE* stream = NULL;
    UDIFResourceFile dmgTrailer;
    char *plist;

    if (argc < 2) {
    	printf("Not Enough Arguments\nCorrect Usage: DMG <dmg_path>");
    	return 1;
    }

    stream = readImageFile(stream, argv[1]);
    parseDMGTrailer(stream, &dmgTrailer);      // reference of dmgTrailer is passed.
    readXMLOffset(stream,&dmgTrailer,&plist);  //reference of dmgTrailer and plist is passed
    parseXML(plist);
	
       
    

    free(plist);
	return 0;
}


