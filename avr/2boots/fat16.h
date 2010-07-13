
	
extern uint16_t sector;


#define PARTTYPE_FAT12	0x01
#define PARTTYPE_FAT16S	0x04
#define PARTTYPE_FAT16L	0x06
#define PARTTYPE_FAT32	0x0B

#define ATTRIBFLAG_READONLY	0x01
#define ATTRIBFLAG_HIDDEN	0x02
#define ATTRIBFLAG_SYSTEM	0x04
#define ATTRIBFLAG_VOLUMELABEL	0x08
#define ATTRIBFLAG_SUBDIR	0x10
#define ATTRIBFLAG_ARCHIVE	0x20


typedef struct
{
  unsigned char state;               // 0x80 if active
  unsigned char startHead;           // Starting head
  unsigned int  startCylinderSector; // Starting cyinder and sector
                                     // Format:
                                     // Bit 0..5  = Bit 0..5 of sector
                                     // Bit 6..7  = Bit 8..9 of cylinder
                                     // Bit 8..15 = Bit 0..7 of cylinder
  unsigned char typeId;              // Partition type
  unsigned char endHead;             // End head
  unsigned int  endCylinderSector;   // End cylinder and sector
                                     // Format see above
  unsigned long sectorOffset;        // Starting sector (counting from 0)
  unsigned long nSectors;            // Number of sectors in partition
} partition_t;

typedef union
{
   unsigned char buffer[512];

   struct
   {
      unsigned char fill[0x1BE];
      partition_t partition[4];
      unsigned short magic;
   } sector;
} mbr_t;

typedef struct
{
	uint8_t  		bsjmpBoot[3]; 		// 0-2   Jump to bootstrap (E.g. eb 3c 90; on i86: JMP 003E NOP. One finds either eb xx 90, or e9 xx xx. The position of the bootstrap varies.)
	char    		bsOEMName[8]; 		// 3-10  OEM name/version (E.g. "IBM  3.3", "IBM 20.0", "MSDOS5.0", "MSWIN4.0".
	uint16_t   	bsBytesPerSec;		// 11-12 Number of bytes per sector (512). Must be one of 512, 1024, 2048, 4096.
	uint8_t			bsSecPerClus;		// 13    Number of sectors per cluster (1). Must be one of 1, 2, 4, 8, 16, 32, 64, 128.
	uint16_t		bsRsvdSecCnt;   	// 14-15 Number of reserved sectors (1). FAT12 and FAT16 use 1. FAT32 uses 32.
	uint8_t			bsNumFATs;			// 16    Number of FAT copies (2)
	uint16_t		bsRootEntCnt; 		// 17-18 Number of root directory entries (224). 0 for FAT32. 512 is recommended for FAT16.
	uint16_t		bsTotSec16; 		// 19-20 Total number of sectors in the filesystem (2880). (in case the partition is not FAT32 and smaller than 32 MB)
	uint8_t			bsMedia;			// 21    Media descriptor type ->  For hard disks:  Value 0xF8  ,  DOS version 2.0   
	uint16_t		bsNrSeProFAT16;     	// 22-23 Number of sectors per FAT (9). 0 for FAT32.
	uint16_t		bsSecPerTrk;		// 24-25 Number of sectors per track (12)
	uint16_t		bsNumHeads; 		// 26-27 Number of heads (2, for a double-sided diskette)
	uint32_t		bsHiddSec;			// 28-31 Number of hidden sectors (0)
	uint32_t		bsTotSec32; 		// 32-35 Number of total sectors (in case the total was not given in bytes 19-20)
	union
	{
		struct
		{
			uint8_t			bsLogDrvNr;			// 36    Logical Drive Number (for use with INT 13, e.g. 0 or 0x80)
			uint8_t			bsReserved;			// 37    Reserved (Earlier: Current Head, the track containing the Boot Record) Used by Windows NT: bit 0: need disk check; bit 1: need surface scan
			uint8_t			bsExtSign;			// 38    Extended signature (0x29)  Indicates that the three following fields are present. Windows NT recognizes either 0x28 or 0x29.
			uint32_t		bsParSerNr; 		// 39-42 Serial number of partition
		};
		
		struct
		{
			uint32_t		SecPerFAT32;
			uint8_t			reserved[3];
		};
	};
	uint8_t			bsVolLbl[11]; 		// 43-53 Volume label or "NO NAME    "
	uint8_t			bsFileSysType[8]; 	// 54-61 Filesystem type (E.g. "FAT12   ", "FAT16   ", "FAT     ", or all zero.)
	uint8_t			bsBootstrap[448]; 	// Bootstrap
	uint16_t		bsSignature; 		// 510-511 Signature 55 aa
} vbr_t;

typedef struct 
{
	char		name[11];      //8 chars filename
	uint8_t	attr;         //file attributes RSHA, Longname, Drive Label, Directory
	uint8_t reserved;
	uint8_t fcrttime;			//Fine resolution creation time stamp, in tenths of a second
	uint32_t crttime;			//Time of Creation
	uint16_t lactime;			//Last Access Time
	uint16_t eaindex;			//EA-Index (used by OS/2 and NT) in FAT12 and FAT16, high 2 ytes of first clusternumber in FAT32
	uint32_t lmodtime;		//Last Modified Time
	uint16_t fstclust;		//First cluster in FAT12 and FAT16, low 2 bytes of first clusternumber in FAT32
	uint32_t filesize;		//File size
} direntry_t;

typedef struct
{
	uint16_t fat_entry[256]; //0: Cluster unused, 1 - Clustercount: Next clusternum, 0xFFFF0 - 0xFFFF6: Reserved Cluster, 0xFFF7 dead Cluster, 0xFFF8 - 0xFFFF: EOF
} fatsector_t;


extern uint8_t fat16_init(void);
/*
	DESCRIPTION:
		Initializes partition
	
	PARAMETER:
		None
		
	RETURN VALUE:
		0: Ok
		1: MMC_INIT failure
		2: Partition not found
		
*/

extern uint16_t fat16_readRootDirEntry(uint16_t entry_num);
/*
	DESCRIPTION:
		Gets a directory entry 
	
	PARAMETERS:
		entry_num:
			number of entry to look for
			
		size:
			Pointer to a ulong where filesize is returned
		
		dir_attrib:
			Pointer to a uchar where file attribute is returned
		
	RETURN VALUE:
		0: entry found, but has no startcluster (e.g. directory or volume label)
		0xFFFF: entry not found (e.g. directory end)
		other: Startcluster of file/directory
		
*/

//extern void fat16_readfilesector(uint16_t startcluster, uint32_t filesector);
extern void fat16_readfilesector(uint16_t cluster);
/*
	DESCRIPTION:
		Retrieves a sector of a file 
	
	PARAMETERS:
		startcluster:
			Startcluster of the file to be read, retrieved with fat16_readRootDirEntry()
		
		filesector:
			sectornumber of the file to be read, 0 - filesize/512
	
	RETURN VALUE:
		-
		
*/

extern direntry_t* entry;

