#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

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
			string[strIndex] == '\t'||
			string[strIndex] == '\n'||
			string[strIndex] == '\r')
			wsCount++;
	}

	//Allocate memory for a new string
	//Add an extra byte for the null terminator character
	newLength = length - wsCount + 1;
	copy = (char*) malloc(newLength * sizeof(char));

	//Copy all non-whitespace characters
	for (strIndex = 0; strIndex <= length; strIndex++)
	{
		if (string[strIndex] != ' ' &&
			string[strIndex] != '\t'&&
			string[strIndex] != '\n'&&
			string[strIndex] != '\r')
		{
			copy[copyIndex] = string[strIndex];
			copyIndex++;
		}
	}

	return copy;
}

//Finds the APFS data block in the parsed Plist
char* getApfsData(xmlDoc *doc, xmlNode *blkxNode, FILE* stream)
{
	xmlNode *array = findSiblingByType(blkxNode, "array");

	if (array == NULL)
		return NULL;

	xmlNode *block = findSiblingByType(array->children, "dict");

	if (block == NULL)
		return NULL;

	//Print all blocks
	for (int i = 0; block != NULL; i++)
	{
		//Find the CFName key
		xmlNode *node = findSiblingByContent(doc, block->children, "CFName");

		if (node == NULL)
			return NULL;

		//Find the CFName content node
		node = findSiblingByType(node, "string");

		if (node == NULL)
			return NULL;

		//Read the block CFName
		const xmlChar *name = xmlNodeListGetString(doc, node->children, 1);
		
		if (strstr(name, "Apple_APFS") != NULL)
		{
			//Find the data node
			node = findSiblingByType(node, "data");

			if (node == NULL)
				return NULL;

			//Print the block data
			char *data = xmlNodeListGetString(doc, node->children, 1);
			char *dataNoWs = removeWhiteSpace(data);
			free(data);

			return dataNoWs;
		}

		//Go to the next block
		block = findSiblingByType(block, "dict");
	}

	//If no APFS block was found, return NULL
	return NULL;
}

//Parses the given Plist and returns the APFS data block
//Adapted from the article:
//https://www.developer.com/database/libxml2-everything-you-need-in-an-xml-library/
char* parsePlist(char* xmlStr, FILE* stream)
{
	xmlDoc *doc = NULL;
	xmlNode *root = NULL;

	//Parse the given string into an XML file
	doc = xmlParseDoc(xmlStr);

	if (doc == NULL)
		return NULL;

	root = xmlDocGetRootElement(doc);

	//Get the blkx node
	xmlNode *blkx = findNodeByText(doc, root, "blkx");

	if (blkx == NULL)
		return NULL;

	//Find the APFS data block
	char* apfsData = getApfsData(doc, blkx,stream);

	//Free any libXML2 memory
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return apfsData;
}