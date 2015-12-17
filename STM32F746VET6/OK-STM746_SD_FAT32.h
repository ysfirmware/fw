/* ============================================================================ */
/*	  OK-STM746 V1.0 키트용(STM32F746VET6) SD 카드 FAT32 헤더 파일		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

// -------------------------------------------------------------------------------
//   이 헤더파일은 OK-STM746 키트 V1.0을 위한 것입니다. 이것은 아래의 조건에서만
//  올바른 동작을 보장합니다.
// -------------------------------------------------------------------------------
//	(1) 시스템 클록 : 216MHz
//	(2) C 컴파일러  : IAR EWARM V7.50.1
//	(3) 최적화 옵션 : High/Size
//	(4) CSTACK 크기 : 0x1000
//      (5) 인클루드    : 기본 헤더 파일 OK-STM746.h의 뒤에 인클루드할 것.
// -------------------------------------------------------------------------------

void Initialize_FAT32(void);			// initialize FAT32
unsigned char fatGetDirEntry(U32 cluster);	// get directory entry
unsigned char *fatDir(U32 cluster, U32 offset);	// get directory entry sector
unsigned int  fatNextCluster(U32 cluster);	// get next cluster
unsigned int  fatClustToSect(U32 cluster);	// convert cluster to sector

unsigned char Get_long_filename(U08 file_number); // check long file name
void TFT_short_filename(U08 xchar,U08 ychar, U16 colorfore,U16 colorback);
void TFT_long_filename(U08 xchar,U08 ychar, U16 colorfore,U16 colorback);
unsigned short Unicode_to_KS(U16 unicode);	// convert Unicode(유니코드) to KS(조합형)
void Filename_arrange(U16 new, U16 old, U08 *FileNameBuffer); // arrange file name

/* ---------------------------------------------------------------------------- */

#define MAX_FILE		200		// maximum file number

#define BYTES_PER_SECTOR	512		// bytes per sector
#define CLUST_FIRST		2		// first legal cluster number
#define CLUST_EOFE		0xFFFFFFFF	// end of eof cluster range
#define FAT16_MASK		0x0000FFFF	// mask for 16 bit cluster numbers
#define FAT32_MASK		0x0FFFFFFF	// mask for FAT32 cluster numbers

unsigned int   file_start_cluster[MAX_FILE];	// file start cluster
unsigned int   file_start_sector[MAX_FILE];	// file start sector number 
unsigned char  file_start_entry[MAX_FILE];	// file directory entry
unsigned int   file_size[MAX_FILE];		// file size

unsigned char  SectorBuffer[512];		// sector buffer
unsigned char  EntryBuffer[512];		// directory entry buffer

unsigned int   sector_cluster = 0;
unsigned int   extension = 0;			// filename extension string(3 character)

unsigned char  Fat32Enabled;			// indicate if is FAT32 or FAT16
unsigned int   FirstDataSector;			// first data sector address
unsigned short SectorsPerCluster;		// number of sectors per cluster
unsigned int   FirstFATSector;			// first FAT sector address
unsigned int   FirstFAT2Sector;			// first FAT2 sector address
unsigned int   FirstDirCluster;			// first directory(data) cluster address

/* ---------------------------------------------------------------------------- */

#pragma pack(1)					// byte(8 bit) align

struct MBR					/* MBR(Master Boot Record) */
{
  unsigned char	 mbrBootCode[512-64-2];		// boot code
  unsigned char	 mbrPartition[64];		// four partition records (64 = 16*4)
  unsigned char	 mbrSignature0;			// signature byte 0 (0x55)
  unsigned char	 mbrSignature1;			// signature byte 1 (0xAA)
};

struct partition				/* partition table(16 byte) of MBR */
{
  unsigned char	 partBootable;			// 0x80 indicates active bootable partition
  unsigned char	 partStartHead;			// starting head for partition
  unsigned short partStartCylSect;		// starting cylinder and sector
  unsigned char	 partPartType;			// partition type
  unsigned char	 partEndHead;			// ending head for this partition
  unsigned short partEndCylSect;		// ending cylinder and sector
  unsigned int	 partStartLBA;			// first LBA sector for this partition
  unsigned int	 partSize;			// size of this partition(bytes or sectors)
};

#define PART_TYPE_FAT16		0x04		// partition type
#define PART_TYPE_FAT16BIG	0x06
#define PART_TYPE_FAT32		0x0B
#define PART_TYPE_FAT32LBA	0x0C
#define PART_TYPE_FAT16LBA	0x0E

struct bootrecord				/* PBR(Partition Boot Record) */
{
  unsigned char	brJumpBoot[3];			// jump instruction E9xxxx or EBxx90
  char		brOEMName[8];			// OEM name and version
  char		brBPB[53];			// volume ID(= BIOS parameter block)
  char		brExt[26];			// Bootsector Extension
  char		brBootCode[418];		// pad so structure is 512 bytes
  unsigned char	brSignature2;			// 2 & 3 are only defined for FAT32
  unsigned char	brSignature3;
  unsigned char	brSignature0;			// boot sector signature byte 0(0x55)
  unsigned char	brSignature1;			// boot sector signature byte 1(0xAA)
};

struct volumeID					/* BPB for FAT16 and FAT32 */
{
					// same at FAT16 and FAT32
  unsigned short bpbBytesPerSec;		// bytes per sector
  unsigned char	 bpbSecPerClust;		// sectors per cluster
  unsigned short bpbResSectors;			// number of reserved sectors
  unsigned char	 bpbFATs;			// number of FATs
  unsigned short bpbRootDirEnts;		// number of root directory entries(0 = FAT32)
  unsigned short bpbSectors;			// total number of sectors(0 = FAT32)
  unsigned char	 bpbMedia;			// media descriptor
  unsigned short bpbFATsecs;			// number of sectors per FAT16(0 = FAT32)
  unsigned short bpbSecPerTrack;		// sectors per track
  unsigned short bpbHeads;			// number of heads
  unsigned int	 bpbHiddenSecs;			// number of hidden sectors
  unsigned int	 bpbHugeSectors;		// number of sectors for FAT32
					// only at FAT32
  unsigned int	 bpbBigFATsecs;			// number of sectors per FAT32
  unsigned short bpbExtFlags;			// extended flags
  unsigned short bpbFSVers;			// file system version
  unsigned int	 bpbRootClust;			// start cluster for root directory
  unsigned short bpbFSInfo;			// file system info structure sector
  unsigned short bpbBackup;			// backup boot sector
  unsigned char	 bpbReserved[12];
					// same at FAT16 and FAT32
  unsigned char	 bpbDriveNum;			// INT 0x13 drive number
  unsigned char	 bpbReserved1;
  unsigned char	 bpbBootSig;			// extended boot signature(0x29)
  unsigned char	 bpbVolID[4];			// extended
  unsigned char	 bpbVolLabel[11];		// extended
  unsigned char	 bpbFileSystemType[8];		// extended
};

typedef struct Dir_entry			/* structure of DOS directory entry */
{
  unsigned char	 dirName[8];      		// filename, blank filled
  unsigned char	 dirExtension[3]; 		// extension, blank filled
  unsigned char	 dirAttributes;   		// file attributes
  unsigned char	 dirLowerCase;    		// NT VFAT lower case flags
  unsigned char	 dirCHundredth;			// hundredth of seconds in CTime
  unsigned char	 dirCTime[2];			// create time
  unsigned char	 dirCDate[2];			// create date
  unsigned char	 dirADate[2];			// access date
  unsigned short dirHighClust;			// high bytes of cluster number
  unsigned char	 dirMTime[2];			// last update time
  unsigned char	 dirMDate[2];			// last update date
  unsigned short dirStartCluster;		// starting cluster of file
  unsigned int	 dirFileSize;			// size of file in bytes
} Dir_entry;

#define 	SLOT_EMPTY      	0x00	// slot has never been used
#define 	SLOT_DELETED    	0xE5	// file in this slot deleted

#define 	ATTR_NORMAL     	0x00	// normal file
#define 	ATTR_READONLY   	0x01	// file is read-only
#define 	ATTR_HIDDEN     	0x02	// file is hidden
#define 	ATTR_SYSTEM     	0x04	// file is a system file
#define 	ATTR_VOLUME     	0x08	// entry is a volume label
#define 	ATTR_LONG_FILENAME	0x0F	// this is a long filename entry
#define 	ATTR_DIRECTORY  	0x10	// entry is a directory name
#define 	ATTR_ARCHIVE    	0x20	// file is new or modified

#define 	LCASE_BASE      	0x08	// filename base in lower case
#define 	LCASE_EXT       	0x10	// filename extension in lower case

typedef struct Long_dir_entry			/* structure of long directory entry */
{
  unsigned char  Longdir_Ord;			// 00(0x00)
  unsigned char	 Longdir_Name1[10];		// 01(0x01)
  unsigned char	 Longdir_Attr;			// 11(0x0B)
  unsigned char	 Longdir_Type;			// 12(0x0C)
  unsigned char	 Longdir_Chksum;		// 13(0x0D)
  unsigned char	 Longdir_Name2[12];		// 14(0x0E)
  unsigned short Longdir_FstClusLO;		// 26(0x1A)
  unsigned char	 Longdir_Name3[4];		// 28(0x1C)
} Long_dir_entry;

#pragma pack()					// word(32 bit) align

/* ---------------------------------------------------------------------------- */
/*			FAT32 포맷을 확인하고 초기화				*/
/* ---------------------------------------------------------------------------- */

void Initialize_FAT32(void)			/* initialize FAT32 */
{
  struct volumeID *BPB;
  struct partition PartInfo;

  TFT_string(0,21,White,Magenta,"            (2) FAT32 초기화            ");

  SD_read_sector(0, SectorBuffer);		// read partition table
  PartInfo = *((struct partition *) ((struct MBR *) SectorBuffer)->mbrPartition);

  SD_read_sector(PartInfo.partStartLBA, SectorBuffer);	// read start LBA sector
  BPB = (struct volumeID *) ((struct bootrecord *) SectorBuffer)->brBPB;
  FirstDataSector = PartInfo.partStartLBA;	// setup global disk constants

  if(BPB->bpbFATsecs)
    FirstDataSector += BPB->bpbResSectors + BPB->bpbFATs * BPB->bpbFATsecs;
  else
    FirstDataSector += BPB->bpbResSectors + BPB->bpbFATs * BPB->bpbBigFATsecs;

  SectorsPerCluster = BPB->bpbSecPerClust;
  FirstFATSector = BPB->bpbResSectors + PartInfo.partStartLBA;

  switch(PartInfo.partPartType)
    { case PART_TYPE_FAT32:			// FAT32
      case PART_TYPE_FAT32LBA:
			FirstDirCluster = BPB->bpbRootClust;
			Fat32Enabled = 1;
                        TFT_string(13,24,Green,Black,"초기화 완료 !");
			break;

      case PART_TYPE_FAT16:			// FAT16
      case PART_TYPE_FAT16BIG:
      case PART_TYPE_FAT16LBA:
			FirstDirCluster	= CLUST_FIRST;
			Fat32Enabled = 0;
			TFT_string(2,25,Red,Black,"본 FAT32 파일 시스템은 FAT16 포맷의");
			TFT_string(5,27,Red,Black,"SD 카드를 지원하지 않습니다 !");
			Beep_3times();
			while(1);
//			break;

      default:		TFT_string(9,25,Red,Black,"본 FAT32 파일 시스템은");
			TFT_string(0,27,Red,Black,"이 SD 카드의 포맷을 지원하지 않습니다 !");
			Beep_3times();
			while(1);
//			break;
    }

  if(Fat32Enabled == 1)
    FirstFAT2Sector = FirstFATSector + BPB->bpbBigFATsecs;
  else
    FirstFAT2Sector = FirstFATSector + BPB->bpbFATsecs;
}

/* ---------------------------------------------------------------------------- */
/*	각 디렉토리 엔트리에서 클러스터 넘버를 추출하고 총 파일수 파악		*/
/* ---------------------------------------------------------------------------- */

unsigned char fatGetDirEntry(U32 cluster)	/* get directory entry */
{
  unsigned char index, files = 0;
  unsigned int  cluster_number, offset = 0;

  Dir_entry      *pDirentry;
  Long_dir_entry *pLDirentry;

  for(offset = 0; ; offset++)
    { pDirentry = (Dir_entry *)fatDir(cluster, offset);

      if(pDirentry == 0)
	return files;
      for(index = 0; index < 16; index++)	// 16 direntries
	{ if(*pDirentry->dirName == SLOT_EMPTY)
	    break;
	  else
	    { if((*pDirentry->dirName != SLOT_DELETED) && (pDirentry->dirAttributes == ATTR_LONG_FILENAME))
		{ pLDirentry = (Long_dir_entry *)pDirentry;
		  if(((pLDirentry->Longdir_Ord & 0x40) == 0x40) || ((pLDirentry->Longdir_Ord & 0x50) == 0x50))
		    { file_start_sector[files] = sector_cluster;
		      file_start_entry[files] = index;
		    }
		}

	      if((*pDirentry->dirName != SLOT_DELETED) && (pDirentry->dirAttributes != ATTR_LONG_FILENAME) && (pDirentry->dirAttributes != ATTR_VOLUME) && (pDirentry->dirAttributes != ATTR_DIRECTORY))
		{ if(((pDirentry->dirLowerCase & 0x18) == 0x18) || ((pDirentry->dirLowerCase & 0x18) == 0x10))
		    { file_start_sector[files] = sector_cluster;
		      file_start_entry[files] = index;	
		    }

		  cluster_number = (unsigned int)(pDirentry->dirHighClust);
		  cluster_number <<= 16;
		  cluster_number |= (unsigned int)(pDirentry->dirStartCluster);

		  file_size[files] = pDirentry->dirFileSize;

		  file_start_cluster[files++] = cluster_number;

		  if(files > MAX_FILE)
		    { TFT_string(9,25,Red,Black,"본 FAT32 파일 시스템은");
		      TFT_string(3,27,Red,Black,"200개 이하의 파일만을 지원합니다 !");
		      Beep_3times();
		      while(1);
		    }
		}
	      pDirentry++;
  	    }
	}					// end of sector
    }						// end of cluster

//  return files;
}

/* ---------------------------------------------------------------------------- */
/*	클러스터에서 디렉토리 엔트리 정보와 오프셋을 가진 섹터를 추출		*/
/* ---------------------------------------------------------------------------- */

unsigned char *fatDir(U32 cluster, U32 offset)	/* get directory entry sector */
{
  unsigned int index;

  for(index = 0; index < offset/SectorsPerCluster; index++)
    cluster = fatNextCluster(cluster);

  if(cluster == 0)
    return 0;

  sector_cluster =  fatClustToSect(cluster) + offset % SectorsPerCluster;
  SD_read_sector(sector_cluster, SectorBuffer);

  return SectorBuffer;
}

/* ---------------------------------------------------------------------------- */
/*		FAT 체인으로 다음 클러스터를 찾음				*/
/* ---------------------------------------------------------------------------- */

unsigned int fatNextCluster(U32 cluster)	/* get next cluster */
{
  unsigned int   nextCluster, fatOffset, fatMask, sector;
  unsigned short offset;
  unsigned char  FAT_cache[512];

  if(Fat32Enabled == 1)
    { fatOffset = cluster << 2;			// four FAT bytes(32 bits) for every cluster
      fatMask = FAT32_MASK;
    }
  else
    { fatOffset = cluster << 1;			// two FAT bytes(16 bits) for every cluster
      fatMask = FAT16_MASK;
    }

  sector = FirstFATSector + (fatOffset/BYTES_PER_SECTOR); // calculate the FAT sector

  offset = fatOffset % BYTES_PER_SECTOR;	// calculate offset of entry in FAT sector

  SD_read_sector(sector,(unsigned char *)FAT_cache);

  nextCluster = (*((unsigned int *) &((char *)FAT_cache)[offset])) & fatMask;

  if(nextCluster == (CLUST_EOFE & fatMask))	// check the end of the chain
    nextCluster = 0;

  return nextCluster;
}

/* ---------------------------------------------------------------------------- */
/*		클러스터 번호를 섹터 번호로 변환				*/
/* ---------------------------------------------------------------------------- */

unsigned int fatClustToSect(U32 cluster)	/* convert cluster to sector */
{
  if(cluster == 0)
    cluster = 2;

  return ((cluster-2)*SectorsPerCluster) + FirstDataSector;
}

/* ---------------------------------------------------------------------------- */
/*		긴 파일명을 추출(13*15=195자까지 사용)				*/
/* ---------------------------------------------------------------------------- */

unsigned char Get_long_filename(U08 file_number) /* check long file name */
{
  unsigned short i, n = 0;
  unsigned short dir_address, dir_number = 0;
  unsigned int	 dir_sector = 0;

  unsigned char ShortFileName[11];		// short file name buffer

  SD_read_sector(file_start_sector[file_number], SectorBuffer);	// read directory entry
  dir_address = file_start_entry[file_number]*32;

  if(SectorBuffer[dir_address+11] == 0x0F)	// *** long file name entry
    { dir_number = (SectorBuffer[dir_address] & 0x1F) + 1; // total number of directory entry

      if(dir_number > 16)			// if file name character > 13*15, return 2
        return 2;

      for( ; dir_address < 512; dir_address++)
	EntryBuffer[n++] = SectorBuffer[dir_address];

      if((dir_number*32) >= n)			// directory entry use 2 sectors
	{ dir_sector = file_start_sector[file_number];
	  SD_read_sector(++dir_sector, SectorBuffer);

	  i = 0;
	  for( ; n < 512; n++)
	    EntryBuffer[n] = SectorBuffer[i++];
	}

      dir_number = (EntryBuffer[0] & 0x1F)*32;	// last entry is short file name entry
      for(n = 0; n < 11; n++)
        ShortFileName[n] = EntryBuffer[dir_number++];

      extension = ShortFileName[8];		// get filename extension character
      extension <<= 8;
      extension += ShortFileName[9];
      extension <<= 8;
      extension += ShortFileName[10];
      return 1;					// if long file name, return 1
    }

  else if(SectorBuffer[dir_address+11] == 0x20)	// *** short file name entry
    { for( ; dir_address < 512; dir_address++)
	EntryBuffer[n++] = SectorBuffer[dir_address];

      for(n = 0; n < 11; n++)
        ShortFileName[n] = EntryBuffer[dir_number++];

      extension = ShortFileName[8];		// get filename extension character
      extension <<= 8;
      extension += ShortFileName[9];
      extension <<= 8;
      extension += ShortFileName[10];
      return 0;					// if short file name, return 0
    }

  else						// if file name error, return 3
    return 3;
}

/* ---------------------------------------------------------------------------- */
/*		짧은 파일명 출력						*/
/* ---------------------------------------------------------------------------- */

void TFT_short_filename(U08 xchar,U08 ychar, U16 colorfore,U16 colorback) /* display short filename */
{
  unsigned char  ch1, i;
  unsigned short ch2;

  Xcharacter = xchar;				// start position
  Ycharacter = ychar;

  foreground = colorfore;			// foreground color and background color
  background = colorback;

  for(i = 0; i < 11; i++ )			// convert upper case to lower case
    { if((EntryBuffer[i] >= 'A') && (EntryBuffer[i] <= 'Z'))
        EntryBuffer[i] += 0x20;	
    }

  for(i = 0; i < 11; i++)			// display 8.3 format
    { if(i == 8)
        TFT_English('.');

      ch1= EntryBuffer[i];
      if(ch1 < 0x80)				// English ASCII character
        {if(ch1 != 0x20)
	   TFT_English(ch1);
	}
      else					// Korean character
	{ ch2 = (ch1 << 8) + EntryBuffer[++i];
	  ch2 = KS_code_conversion(ch2);
	  TFT_Korean(ch2);
	}
    }
}

/* ---------------------------------------------------------------------------- */
/*		긴 파일명 출력(40자까지 표시)					*/
/* ---------------------------------------------------------------------------- */

void TFT_long_filename(U08 xchar,U08 ychar, U16 colorfore,U16 colorback) /* display long filename */
{
  unsigned char  ch1;
  unsigned short ch2, i, entrynumber, charnumber, newindex, oldindex;
  unsigned char  FileNameBuffer[512];

  Xcharacter = xchar;				// start position
  Ycharacter = ychar;

  foreground = colorfore;			// foreground color and background color
  background = colorback;

  entrynumber = EntryBuffer[0] & 0x1F;		// long entry number
  charnumber = (EntryBuffer[0] & 0x1F)*13;	// character number of file name
  oldindex = ((EntryBuffer[0] & 0x1F) - 0x01)*32;
  newindex = 0;

  for(i = 0; i < entrynumber; i++)		// arrange file name
    { Filename_arrange(newindex,oldindex,FileNameBuffer);
      newindex += 26;
      oldindex -= 32;
    }

  for(i = 0; i < charnumber*2; i++)		// display long file name format
    { if(i >= 40*2)				// limit length(40 character)
	{ TFT_English('~');
          TFT_English('1');
          return;
	}

      ch1 = FileNameBuffer[i];
      if(ch1 == 0xFF)
	return;

      if(ch1 < 0x80)				// English ASCII character
	TFT_English(FileNameBuffer[++i]);
      else					// Korean character
	{ ch2 = (ch1 << 8) + FileNameBuffer[++i];
	  ch2 = Unicode_to_KS(ch2);
	  TFT_Korean(ch2);
	}
    }
}

/* ---------------------------------------------------------------------------- */
/*		파일명 문자들을 순서대로 정리하여 재배열			*/
/* ---------------------------------------------------------------------------- */

void Filename_arrange(U16 newx, U16 oldx, U08 *FileNameBuffer) /* arrange file name */
{
  FileNameBuffer[newx+0]  = EntryBuffer[oldx+2];  // 1
  FileNameBuffer[newx+1]  = EntryBuffer[oldx+1];
  FileNameBuffer[newx+2]  = EntryBuffer[oldx+4];  // 2
  FileNameBuffer[newx+3]  = EntryBuffer[oldx+3];
  FileNameBuffer[newx+4]  = EntryBuffer[oldx+6];  // 3
  FileNameBuffer[newx+5]  = EntryBuffer[oldx+5];
  FileNameBuffer[newx+6]  = EntryBuffer[oldx+8];  // 4
  FileNameBuffer[newx+7]  = EntryBuffer[oldx+7];
  FileNameBuffer[newx+8]  = EntryBuffer[oldx+10]; // 5
  FileNameBuffer[newx+9]  = EntryBuffer[oldx+9];
  FileNameBuffer[newx+10] = EntryBuffer[oldx+15]; // 6
  FileNameBuffer[newx+11] = EntryBuffer[oldx+14];
  FileNameBuffer[newx+12] = EntryBuffer[oldx+17]; // 7
  FileNameBuffer[newx+13] = EntryBuffer[oldx+16];
  FileNameBuffer[newx+14] = EntryBuffer[oldx+19]; // 8
  FileNameBuffer[newx+15] = EntryBuffer[oldx+18];
  FileNameBuffer[newx+16] = EntryBuffer[oldx+21]; // 9
  FileNameBuffer[newx+17] = EntryBuffer[oldx+20];
  FileNameBuffer[newx+18] = EntryBuffer[oldx+23]; // 10
  FileNameBuffer[newx+19] = EntryBuffer[oldx+22];
  FileNameBuffer[newx+20] = EntryBuffer[oldx+25]; // 11
  FileNameBuffer[newx+21] = EntryBuffer[oldx+24];
  FileNameBuffer[newx+22] = EntryBuffer[oldx+29]; // 12
  FileNameBuffer[newx+23] = EntryBuffer[oldx+28];
  FileNameBuffer[newx+24] = EntryBuffer[oldx+31]; // 13
  FileNameBuffer[newx+25] = EntryBuffer[oldx+30];
}

/* ---------------------------------------------------------------------------- */
/*		한글 유니코드를 KS 조합형 코드로 변환				*/
/* ---------------------------------------------------------------------------- */

unsigned short Unicode_to_KS(U16 unicode)	/* convert Unicode(유니코드) to KS(조합형) */
{
  unsigned char  cho = 0, joong = 0, jong = 0;
  unsigned short value;

  value = unicode - 0xAC00;                	// 유니코드에서 '가'에 해당하는 값을 뺀다.

  jong  = value % 28;				// 유니코드를 초성, 중성, 종성으로 분리
  joong = ((value - jong) / 28 ) % 21;
  cho   = ((value - jong) / 28 ) / 21;

  cho += 2;                           	 	// 초성 + 오프셋

  if(joong < 5)       joong += 3;		// 중성 + 오프셋
  else if(joong < 11) joong += 5;
  else if(joong < 17) joong += 7;
  else                joong += 9;

  if(jong < 17) jong++;				// 종성 + 오프셋
  else          jong += 2;

  return 0x8000 | (cho << 10) | ( joong << 5) | jong; // 조합형 코드
}
