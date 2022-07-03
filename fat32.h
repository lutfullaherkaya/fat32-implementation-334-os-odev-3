#ifndef HW3_FAT32_H
#define HW3_FAT32_H

#include <stdint.h>
#include <vector>
#include <string>
#include "Bilimum.h"
#include <chrono>
#include <sys/time.h>
#include <ctime>

using namespace std;
using std::cout;
using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
// Bytes per sector is fixed at 512 in this homework.
#define BPS 512

#pragma pack(push, 1)
// Starting at offset 36 into the BIOS Parameter Block (BRB) for FAT32
struct BPB32_struct {
    uint32_t FATSize;              // Logical sectors per FAT. Size: 4 bytes
    uint16_t ExtFlags;             // Drive description/Mirroring flags. Size: 2 bytes
    uint16_t FSVersion;            // Version. Size: 2 bytes.
    uint32_t RootCluster;          // Cluster number of root directory start. Typically it is two. Size: 4 bytes.
    uint16_t FSInfo;               // Logical sector number of FS Information sector on FAT32. Size: 2 bytes
    uint16_t BkBootSec;            // First logical sector number of a copy of the three FAT32 boot sectors. It is typically 6. Size: 2 bytes
    uint8_t Reserved[12];          // Reserved bytes. (Previously used by MS-DOS utility FDISK). Size: 12 byte
    uint8_t BS_DriveNumber;        // Physical drive number. Size: 1 byte
    uint8_t BS_Reserved1;          // Reserved. Used for various purposes on FAT12/16 systems. Size: 1 byte
    uint8_t BS_BootSig;            // Boot signature. 0x26 for FAT12/16, 0x29 for FAT32. Size: 1 byte
    uint32_t BS_VolumeID;          // Volume ID. (Mostly for FAT12/16). Size: 4 bytes
    uint8_t BS_VolumeLabel[11];    // Volume Name. (Not really important). Size: 11 bytes.
    uint8_t BS_FileSystemType[8];  // File system type. Padded with spaces at the end. In our case it will be "FAT32   ". Size: 8 bytes
};

struct BPB_struct {

    uint8_t BS_JumpBoot[3];        // Jump Instruction. Size: 3 bytes
    uint8_t BS_OEMName[8];         // The system that formatted the disk. Size: 8 bytes
    uint16_t BytesPerSector;       // Bytes per logical sector (It is always will be 512 in our case). Size: 2 bytes
    uint8_t SectorsPerCluster;     // Logical sectors per cluster in the order of two. Size: 1 byte
    uint16_t ReservedSectorCount;  // Count of reserved logical sectors. Size: 2 bytes
    uint8_t NumFATs;               // Number of file allocation tables. Default value is two but can be higher. Size: 1 byte
    uint16_t RootEntryCount;       // Maximum number of FAT12 or FAT16 directory entries. It is 0 for FAT32. Size: 2 bytes
    uint16_t TotalSectors16;       // Total logical sectors. It is 0 for FAT32. Size: 2 bytes
    uint8_t Media;                 // Media descriptor. Size: 1 byte
    uint16_t FATSize16;            // Logical sectors per FAT for FAT12/FAT16. It is 0 for FAT32. Size: 2 bytes
    uint16_t SectorsPerTrack;      // Not relevant
    uint16_t NumberOfHeads;        // Not relevant
    uint32_t HiddenSectors;        // Not relevant
    uint32_t TotalSectors32;       // Total logical sectors including the hidden sectors
    BPB32_struct extended;         // Extended parameters for FAT32
};

struct FatFile83 {
    uint8_t filename[8];           // Filename for short filenames. First byte have special values.
    uint8_t extension[3];          // Remaining part used for file extension
    uint8_t attributes;            // Attributes
    uint8_t reserved;              // Reserved to mark extended attributes
    uint8_t creationTimeMs;        // Creation time down to ms precision
    uint16_t creationTime;         // Creation time with H:M:S format
    uint16_t creationDate;         // Creation date with Y:M:D format
    uint16_t lastAccessTime;       // Last access time
    uint16_t eaIndex;              // Used to store first two bytes of the first cluster
    uint16_t modifiedTime;         // Modification time with H:M:S format
    uint16_t modifiedDate;         // Modification date with Y:M:D format
    uint16_t firstCluster;         // Last two bytes of the first cluster
    uint32_t fileSize;             // Filesize in bytes

    uint32_t clusterID() const { return (eaIndex >> 16) | firstCluster; }

    static bool clusterVar(uint32_t clusterID) {
        if (clusterID >= 0x0FFFFFF8) {
            // cluster kalmadi
            return false;
        } else if (clusterID == 0x0FFFFFF7) {
            // bad sector
            return false;
        } else {
            //
            return true;
        }
    }

    static string tarihSaatYaziyaDonustur(uint16_t tarih, uint16_t zaman) {
        string sonuc;
        string aylar[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        sonuc += aylar[(tarih >> 5) & 0b1111] + " " + to_string(tarih & 0b11111) + " ";
        sonuc += saatBasina0Koy(to_string((zaman >> 11) & 0b11111)) + ":" +
                 saatBasina0Koy(to_string((zaman >> 5) & 0b111111));
        return sonuc;
    }

    static string saatBasina0Koy(string saat) {
        if (saat.size() == 1) {
            return "0" + saat;
        } else {
            return saat;
        }
    }

    void setAllDateTimesToNow() {
        // current date/time based on current system
        time_t simdi = time(0);
        tm *ltm = localtime(&simdi);

        auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        creationTimeMs = (millisec_since_epoch / 10) % 200;

        uint16_t zaman = simdikiZaman();
        creationTime = zaman;
        modifiedTime = zaman;
        uint16_t tarih = simdikiTarih();
        creationDate = tarih;
        lastAccessTime = tarih;
        modifiedDate = tarih;
    }

    static uint16_t simdikiZaman() {
        // current date/time based on current system
        time_t simdi = time(0);
        tm *ltm = localtime(&simdi);

        uint16_t zaman = 0;
        zaman |= (ltm->tm_sec / 2 % 30);
        zaman |= ((ltm->tm_min % 60) << 5);
        zaman |= ((ltm->tm_hour % 24) << 11);
        return zaman;
    }

    static uint16_t simdikiTarih() {
        // current date/time based on current system
        time_t simdi = time(0);
        tm *ltm = localtime(&simdi);
        uint16_t tarih = 0;
        tarih |= (ltm->tm_mday);
        tarih |= ((ltm->tm_mon) << 5);
        tarih |= ((ltm->tm_year - 80) << 9);
        return tarih;

    }
};

// The long filename information can be repeated as necessary before the original 8.3 filename entry
struct FatFileLFN {
    uint8_t sequence_number;
    uint16_t name1[5];      // 5 Chars of name (UTF-16 format)
    uint8_t attributes;     // Always 0x0f
    uint8_t reserved;       // Always 0x00
    uint8_t checksum;       // Checksum of DOS Filename. Can be calculated with a special formula.
    uint16_t name2[6];      // 6 More chars of name (UTF-16 format)
    uint16_t firstCluster;  // Always 0x0000
    uint16_t name3[2];      // 2 More chars of name (UTF-16 format)

    unsigned char lfn_checksum(const unsigned char *pFCBName) {
        int i;
        unsigned char sum = 0;

        for (i = 11; i; i--)
            sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;

        return sum;
    }

    void setName(string name) {
        for (int i = 0; i < 5; ++i) {
            if (i < name.size()) {
                name1[i] = name[i];
            } else if (i == name.size()) {
                name1[i] = 0;
            } else {
                name1[i] = 0xFFFF;
            }
        }
        for (int i = 0; i < 6; ++i) {
            if ((i + 5) < name.size()) {
                name2[i] = name[i + 5];
            } else if ((i + 5) == name.size()) {
                name2[i] = 0;
            } else {
                name2[i] = 0xFFFF;
            }
        }
        for (int i = 0; i < 2; ++i) {
            if ((i + 11) < name.size()) {
                name3[i] = name[i + 11];
            } else if ((i + 11) == name.size()) {
                name3[i] = 0;
            } else {
                name3[i] = 0xFFFF;
            }
        }
    }

    static vector<uint16_t> birlesikLFN(vector<FatFileLFN> &ayrikLFNler) {
        vector<uint16_t> sonuc;
        for (long i = ayrikLFNler.size() - 1; i >= 0; --i) {
            auto &lfn = ayrikLFNler[i];
            for (uint16_t &utf16char: lfn.name1) {
                sonuc.push_back(utf16char);
            }
            for (uint16_t &utf16char: lfn.name2) {
                sonuc.push_back(utf16char);
            }
            for (uint16_t &utf16char: lfn.name3) {
                sonuc.push_back(utf16char);
            }
        }
        return sonuc;
    }

    static string utf16DenAsciiYe(vector<uint16_t> &utf16string) {
        string sonuc;
        for (uint16_t &utf16char: utf16string) {
            if (utf16char == 0) {
                break;
            }
            sonuc.push_back(((char *) (&utf16char))[0]);
        }
        return sonuc;
    }
};

union FatFileEntry {
    FatFile83 msdos;
    FatFileLFN lfn;

    bool longFileNamedir() {
        uint8_t bayt11 = ((uint8_t *) this)[11];
        uint8_t ilkBayt = ((uint8_t *) this)[0];
        return (ilkBayt != 0) && (ilkBayt != 0xE5) && (ilkBayt != 0x05) && (bayt11 == 0x0F);
    }
};

#pragma pack(pop)


#endif //HW3_FAT32_H
