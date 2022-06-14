//
// Created by lutfullah on 13.06.2022.
//

#ifndef INC_334_OS_ODEV_3_SISKO32_H
#define INC_334_OS_ODEV_3_SISKO32_H
using namespace std;

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "fat32.h"

class Dizin;

class SISKO32 {
public:
    explicit SISKO32(char *imajDosyasi);

    BPB_struct fat_boot;
    string imajDosyasi;


    FILE *imajFP;

    uint32_t clusterSectorAdresi(uint32_t clusterID);

    uint32_t firstDataSector;
    uint32_t rootClusterID;

    long clusterBaytAdresi(uint32_t clusterID);
};

class Dizin {
public:
    Dizin(SISKO32 &sisko32);

    void ustDizineCik();

    string dizinAdi();

    pair<vector<FatFileLFN>, FatFile83> dizinOku();

    bool kokDizindir();

    bool in(string &altDizin);

    void kokeCik();

    SISKO32 &sisko32;
    pair<vector<FatFileLFN>, FatFile83> dizin;
    vector<string> dizinKelimeleri;
    uint32_t dizinClusterID;

    void dizinAtla();
};

#endif //INC_334_OS_ODEV_3_SISKO32_H
