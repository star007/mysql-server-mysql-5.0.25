/* Copyright (C) 2003 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef BACKUP_FORMAT_HPP
#define BACKUP_FORMAT_HPP

#include <ndb_types.h>

static const char BACKUP_MAGIC[] = { 'N', 'D', 'B', 'B', 'C', 'K', 'U', 'P' };

struct BackupFormat {

  /**
   * Section types in file
   */
  enum SectionType {
    FILE_HEADER       = 1,
    FRAGMENT_HEADER   = 2,
    FRAGMENT_FOOTER   = 3,
    TABLE_LIST        = 4,
    TABLE_DESCRIPTION = 5,
    GCP_ENTRY         = 6,
    FRAGMENT_INFO     = 7
  };

  struct FileHeader {
    char Magic[8];
    Uint32 NdbVersion;

    Uint32 SectionType;
    Uint32 SectionLength;
    Uint32 FileType;
    Uint32 BackupId;
    Uint32 BackupKey_0;
    Uint32 BackupKey_1;
    Uint32 ByteOrder;
  };
  
  /**
   * File types
   */
  enum FileType {
    CTL_FILE = 1,
    LOG_FILE = 2,
    DATA_FILE = 3
  };
  
  /**
   * Data file formats
   */
  struct DataFile {

    struct FragmentHeader {
      Uint32 SectionType;
      Uint32 SectionLength;
      Uint32 TableId;
      Uint32 FragmentNo;
      Uint32 ChecksumType;
    };
    
    struct VariableData {
      Uint32 Sz;
      Uint32 Id;
      Uint32 Data[1];
    };
    
    struct Record {
      Uint32 Length;
      Uint32 NullBitmask[1];
      Uint32 DataFixedKeys[1];
      Uint32 DataFixedAttributes[1];
      VariableData DataVariableAttributes[1];
    };
    
    struct FragmentFooter {
      Uint32 SectionType;
      Uint32 SectionLength;
      Uint32 TableId;
      Uint32 FragmentNo;
      Uint32 NoOfRecords;
      Uint32 Checksum;
    };
  };

  /**
   * CTL file formats
   */
  struct CtlFile {
    
    /**
     * Table list
     */
    struct TableList {
      Uint32 SectionType;
      Uint32 SectionLength;
      Uint32 TableIds[1];      // Length = SectionLength - 2
    };

    /**
     * Table description(s)
     */
    struct TableDescription {
      Uint32 SectionType;
      Uint32 SectionLength;
      Uint32 DictTabInfo[1];   // Length = SectionLength - 2
    };

    /**
     * GCP Entry
     */
    struct GCPEntry {
      Uint32 SectionType;
      Uint32 SectionLength;
      Uint32 StartGCP;
      Uint32 StopGCP;
    };

    /**
     * Fragment Info
     */
    struct FragmentInfo {
      Uint32 SectionType;
      Uint32 SectionLength;
      Uint32 TableId;
      Uint32 FragmentNo;
      Uint32 NoOfRecordsLow;
      Uint32 NoOfRecordsHigh;
      Uint32 FilePosLow;
      Uint32 FilePosHigh;
    };
  };

  /**
   * LOG file format
   */
  struct LogFile {

    /**
     * Log Entry
     */
    struct LogEntry {
      Uint32 Length;
      Uint32 TableId;
      // If TriggerEvent & 0x10000 == true then GCI is right after data
      Uint32 TriggerEvent; 
      Uint32 Data[1]; // Len = Length - 2
    };
  };
};

#endif
