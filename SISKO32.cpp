//
// Created by lutfullah on 13.06.2022.
//

#include "SISKO32.h"

SISKO32::SISKO32(char *imajDosyasi) : imajDosyasi(imajDosyasi) {
    imajFP = fopen(imajDosyasi, "r+");
    fread(&fat_boot, sizeof(BPB_struct), 1, imajFP);

    rootClusterID = fat_boot.extended.RootCluster;
    firstDataSector = fat_boot.ReservedSectorCount + (fat_boot.NumFATs * fat_boot.extended.FATSize);

    uint32_t first_sector_of_cluster = clusterSectorAdresi(rootClusterID);

}


uint32_t SISKO32::clusterSectorAdresi(uint32_t clusterID) {
    return ((clusterID - 2) * fat_boot.SectorsPerCluster) + firstDataSector;

}

long SISKO32::clusterBaytAdresi(uint32_t clusterID) {
    return clusterSectorAdresi(clusterID) * fat_boot.BytesPerSector;
}

Dizin::Dizin(SISKO32 &sisko32) : sisko32(sisko32) {
    kokeCik();
}

bool Dizin::kokDizindir() {
    return dizinKelimeleri.empty();
}

bool Dizin::in(string &altDizin) {
    fseek(sisko32.imajFP, sisko32.clusterBaytAdresi(dizinClusterID), SEEK_SET);
    while (true) {
        // todo: zincirin diger halkalarina da bakilacak bu zincir biterse.
        // todo: extensiona da bakmiyorum hic.
        pair<vector<FatFileLFN>, FatFile83> cocukDizin = dizinOku();
        uint8_t ilkBayt = ((uint8_t *) &cocukDizin.second)[0];
        if (ilkBayt == 0) {
            // If the first byte of the entry is equal to 0 then there are no more files/directories in this directory,
            break;
        } else if (ilkBayt == 0xE5) {
            // If the first byte of the entry is equal to 0xE5 then the entry is unused.
        } else {
            string cocukDizinAdi;
            if (!cocukDizin.first.empty()) {
                vector<uint16_t> utf16string = FatFileLFN::birlesikLFN(cocukDizin.first);
                cocukDizinAdi = FatFileLFN::utf16DenAsciiYe(utf16string);
            }

            if (cocukDizinAdi == altDizin ||
                (altDizin == ".." && cocukDizin.second.filename[0] == '.' && cocukDizin.second.filename[1] == '.')) {
                dizin = cocukDizin;
                dizinClusterID = cocukDizin.second.clusterID();
                if (dizinClusterID == 0) {
                    dizinClusterID = 2; // .. olunca rootu 0 olarak gosteriyor.
                }
                dizinKelimeleri.push_back(altDizin);
                return true;
            }
        }
    }
    return false;
}

void Dizin::kokeCik() {
    fseek(sisko32.imajFP, sisko32.clusterBaytAdresi(sisko32.rootClusterID), SEEK_SET);
    dizin = dizinOku();
    dizinClusterID = sisko32.rootClusterID;
    dizinKelimeleri.clear();
}

pair<vector<FatFileLFN>, FatFile83> Dizin::dizinOku() {
    vector<FatFileLFN> longFileNames;
    FatFile83 file;
    while (true) {
        FatFileEntry bilinmeyenDosya;
        fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32.imajFP);
        uint8_t bayt11 = ((uint8_t *) &bilinmeyenDosya)[11];
        if (bayt11 == 0x0F) {
            longFileNames.push_back(bilinmeyenDosya.lfn);
        } else {
            // dizin okundu, uzun dosya adi varsa alindi.
            return {longFileNames, bilinmeyenDosya.msdos};
        }
    }
}

void Dizin::ustDizineCik() {
    string ustDizin = "..";
    in(ustDizin);
    dizinKelimeleri.pop_back();
    dizinKelimeleri.pop_back();
}

void Dizin::dizinAtla() {
    fseek(sisko32.imajFP, sizeof(FatFileEntry), SEEK_CUR);
}


