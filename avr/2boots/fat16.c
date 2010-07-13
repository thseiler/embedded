#include <avr/io.h>
#include "mmc_lib.h"
#include "fat16.h"

static uint16_t  RootDirRegionStartSec;
static uint32_t  DataRegionStartSec;
static uint16_t  RootDirRegionSize;
static uint8_t   SectorsPerCluster;
static uint16_t  FATRegionStartSec;

unsigned char fat16_init(void)
{
	mbr_t *mbr = (mbr_t*) buff;
	vbr_t *vbr = (vbr_t*) buff;
		
	if (mmc_init() != MMC_OK) return 1;
	
    mmc_start_read_block(0);

    // Try sector 0 as a bootsector
	if ((vbr->bsFileSysType[0] == 'F') && (vbr->bsFileSysType[4] == '6'))
	{
		FATRegionStartSec = 0;
	}
	else // Try sector 0 as a MBR	
	{     	 
		FATRegionStartSec = mbr->sector.partition[0].sectorOffset;
          
		mmc_start_read_block(mbr->sector.partition[0].sectorOffset);
	  
        if ((vbr->bsFileSysType[0] != 'F') || (vbr->bsFileSysType[4] != '6'))
		   return 2; // No FAT16 found
     }
    
	SectorsPerCluster  			= vbr->bsSecPerClus;		// 4
	
	// Calculation Algorithms
	FATRegionStartSec			+= vbr->bsRsvdSecCnt;						// 6
	RootDirRegionStartSec	 	= FATRegionStartSec + (vbr->bsNumFATs * vbr->bsNrSeProFAT16);		// 496	
//	RootDirRegionSize		 	= (vbr->bsRootEntCnt * 32) / 512;						// 32
	RootDirRegionSize		 	= (vbr->bsRootEntCnt / 16); 						// 32
	DataRegionStartSec 			= RootDirRegionStartSec + RootDirRegionSize;	// 528
	
	return 0;
}

uint16_t fat16_readRootDirEntry(uint16_t entry_num)
{
 uint8_t direntry_in_sector;
 
	direntry_t *dir; //Zeiger auf einen Verzeichniseintrag
	

//	if ((entry_num * sizeof(direntry_t) / 512) >= RootDirRegionSize)
	if ((entry_num / 16) >= RootDirRegionSize)
		return 0xFFFF; //End of root dir region reached!
	
//	uint32_t dirsector = RootDirRegionStartSec + entry_num * sizeof(direntry_t) / 512;
	uint32_t dirsector = RootDirRegionStartSec + entry_num / 16;

//	entry_num %= 512 / sizeof(direntry_t);
//        direntry_in_sector = (unsigned char) entry_num % (512 / sizeof(direntry_t));
        direntry_in_sector = (unsigned char) entry_num % 16;

	mmc_start_read_block(dirsector);
	
//	dir = (direntry_t *) buff + entry_num;
	dir = (direntry_t *) buff + direntry_in_sector;

	if ((dir->name[0] == 0) || (dir->name[0] == 0xE5) || (dir->fstclust == 0))
		return 0xFFFF;

	entry = dir;

	return dir->fstclust;
}


//void fat16_readfilesector(uint16_t startcluster, uint32_t filesector)
//filesector is not a real sectornumber. It's a sectoroffset in file !
void fat16_readfilesector(uint16_t cluster)
{
	uint16_t clusteroffset;
//	uint16_t currentfatsector;
	uint8_t currentfatsector;
	uint8_t temp, secoffset;
	uint32_t templong;
	
	fatsector_t *fatsector = (fatsector_t*) buff;

// SectorsPerCluster is always power of 2 !

// Einfach hierhin verschoben bringt 6 Bytes !
//	filesector %= SectorsPerCluster;
	secoffset = (uint8_t)sector & (SectorsPerCluster-1);
	
         clusteroffset = sector; // seit uint16_t filesector gehts nun doch ;)
//          clusteroffset = filesector / SectorsPerCluster;
           temp = SectorsPerCluster >> 1;
           while(temp)
            {
             clusteroffset >>= 1;
             temp >>= 1;
            }

// holger:
// cluster / 256 ist unsigned char !
// deshalb ist auch currentfatsector unsigned char !

/*
	currentfatsector = 0xFFFF;
	while (clusteroffset)
	{
		if (currentfatsector != (cluster / 256))
		{
			mmc_read_block(FATRegionStartSec + cluster / 256);
			currentfatsector = cluster / 256;
		}
		
		cluster = fatsector->fat_entry[cluster % 256];
		clusteroffset--;
	}
*/

	currentfatsector = 0xFF;
	while (clusteroffset)
	{
          temp = (unsigned char)((cluster & 0xFF00) >>8);
          
		if (currentfatsector != temp)
		{
			mmc_start_read_block(FATRegionStartSec + temp);

			currentfatsector = temp;
		}
		
		cluster = fatsector->fat_entry[cluster % 256];
		clusteroffset--;
	}

//	if (cluster < 2)
//		return; //Free/reserved Sector
		
//	if ((startcluster & 0xFFFFFFF0) == 0xFFFFFFF0)
//	if ((cluster & 0xFFF0) == 0xFFF0) //FAT16 hat nur 16Bit Clusternummern
//		return; //Reserved / bad / last cluster

// templong = (unsigned long) (startcluster - 2) * SectorsPerCluster;
        templong = cluster - 2;
          temp = SectorsPerCluster>>1;
          while(temp)
           {
            templong <<= 1;
            temp >>= 1;
           }
		
	mmc_start_read_block(templong + DataRegionStartSec + secoffset);
	sector++;
}
