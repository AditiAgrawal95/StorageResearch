#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct {
    char     Signature[4];          // Magic ('koly')
    uint32_t Version;               // Current version is 4
    uint32_t HeaderSize;            // sizeof(this), always 512
    uint32_t Flags;                 // Flags
    uint64_t RunningDataForkOffset; //
    uint64_t DataForkOffset;        // Data fork offset (usually 0, beginning of file)
    uint64_t DataForkLength;        // Size of data fork (usually up to the XMLOffset, below)
    uint64_t RsrcForkOffset;        // Resource fork offset, if any
    uint64_t RsrcForkLength;        // Resource fork length, if any
    uint32_t SegmentNumber;         // Usually 1, may be 0
    uint32_t SegmentCount;          // Usually 1, may be 0
    uint8_t  SegmentID[16];         // 128-bit GUID identifier of segment (if SegmentNumber !=0)

    uint32_t DataChecksumType;      // Data fork 
    uint32_t DataChecksumSize;      //  Checksum Information
    uint32_t DataChecksum[32];      // Up to 128-bytes (32 x 4) of checksum

    uint64_t XMLOffset;             // Offset of property list in DMG, from beginning
    uint64_t XMLLength;             // Length of property list
    uint8_t  Reserved1[120];        // 120 reserved bytes - zeroed

    uint32_t ChecksumType;          // Master
    uint32_t ChecksumSize;          //  Checksum information
    uint32_t Checksum[32];          // Up to 128-bytes (32 x 4) of checksum

    uint32_t ImageVariant;          // Commonly 1
    uint64_t SectorCount;           // Size of DMG when expanded, in sectors

    uint32_t reserved2;             // 0
    uint32_t reserved3;             // 0 
    uint32_t reserved4;             // 0

}UDIFResourceFile;

typedef struct {
    uint32_t ChecksumType;          // Master
    uint32_t ChecksumSize;          //  Checksum information
    uint32_t Checksum[32];          //128 
}UDIFChecksum;


// Where each  BLXKRunEntry is defined as follows:

typedef struct {
    uint32_t EntryType;         // Compression type used or entry type (see next table)
    uint32_t Comment;           // "+beg" or "+end", if EntryType is comment (0x7FFFFFFE). Else reserved.
    uint64_t SectorNumber;      // Start sector of this chunk
    uint64_t SectorCount;       // Number of sectors in this chunk
    uint64_t CompressedOffset;  // Start of chunk in data fork
    uint64_t CompressedLength;  // Count of bytes of chunk, in data fork
}BLKXChunkEntry;

typedef struct {
    uint32_t Signature;  
   // uint32_t num;// Magic ('mish')
    uint32_t Version;            // Current version is 1
    uint64_t SectorNumber;       // Starting disk sector in this blkx descriptor
    uint64_t SectorCount;        // Number of disk sectors in this blkx descriptor

    uint64_t DataOffset;
    uint32_t BuffersNeeded;
    uint32_t BlockDescriptors;   // Number of descriptors

    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t reserved4;
    uint32_t reserved5;
    uint32_t reserved6;

    UDIFChecksum checksum;

    uint32_t NumberOfBlockChunks;
    BLKXChunkEntry chunk[0];
}BLKXTable;
#pragma pack()


FILE* readImageFile(FILE*);  // To open the file
FILE* parseDMGFile(FILE*, UDIFResourceFile*);
FILE* readXMLOffset(FILE*, UDIFResourceFile*, char**);
 void build_decoding_table();
 void base64_cleanup();
 char* base64_encode(const char*,size_t ,size_t* );
unsigned char* base64_decode(const char* ,size_t ,size_t* );
BLKXTable* decodeDataBlk(const char*);
readDataBlks(const char*,FILE*);


