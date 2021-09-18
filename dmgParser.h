#pragma once
#include <stdint.h>

int fileSize = 0;

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
#pragma pack()

// Declarations of functions
FILE* readImageFile(FILE*);  // To open the file
FILE* parseDMGFile(FILE*, UDIFResourceFile*);
FILE* readXMLOffset(FILE*, UDIFResourceFile*, char**);



