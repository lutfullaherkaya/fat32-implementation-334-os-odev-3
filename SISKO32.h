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
#include "Bilimum.h"

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

    int clusterAzamiEntrySayisi;

    uint32_t yeniClusterAyir(uint32_t baglanacakClusterID);

    unsigned int getSISKO(uint32_t clusterID);

    uint32_t freeClusterBul();

    bool setSISKO(uint32_t baglanacakClusterID, uint32_t degerClusterID);

    FatFileEntry diskEntriOku(long adres);

    void diskEntriYaz(long adres, FatFileEntry *entriPtr);
};

class Dizin {
public:
    Dizin(SISKO32 *sisko32);

    void ustDizineCik();

    static string dizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti);

    static string uzunDizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti);

    static string kisaDizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti);

    static string filenamedenStrye(uint8_t *fname, int len);


    bool kokDizindir();

    bool in(string altDizin);

    void kokeCik();

    SISKO32 *sisko32;
    pair<vector<FatFileLFN>, FatFile83> dizin;
    vector<string> dizinKelimeleri;
    uint32_t dizinClusterID;
    long fatFile83Adresi;

    void fatFile83DiskeYaz();

    bool git(string &yol);
    bool git(vector<string> &gidilecekDizinKelimeleri);

    void ls(bool ayrintili);



    static bool dizinBosVeSonrasiYok(FatFile83 &dizin1);

    static bool dizinSilinmis(FatFile83 &dizin1);

    static bool klasordur(const pair<vector<FatFileLFN>, FatFile83>& dizin1);

    static bool noktaDizinidir(pair<vector<FatFileLFN>, FatFile83> dizin1);

    void dosyaOlustur(string &dosyaAdi, bool klasordur);

    vector<FatFileEntry> dizinEntrileriOlustur(string &dizinAdi, bool klasordur);

    void cat();

    bool dizinEntrileriEkle(vector<FatFileEntry> &entriler);
    bool dizinEntrileriEkle(pair<vector<FatFileLFN>, FatFile83>& dizinEntrileri);

    FatFile83 fatFile83olustur(string dizinAdi, bool klasordur, uint32_t firstClusterID);
    FatFile83 fatFile83olustur(string dizinAdi, bool klasordur);

    static vector<FatFileEntry> FatFileLFNlerOlustur(string &dizinAdi, const unsigned char *fatFile83filename);

    bool fatFileEntrySil(string altDizin);
};

#endif //INC_334_OS_ODEV_3_SISKO32_H
