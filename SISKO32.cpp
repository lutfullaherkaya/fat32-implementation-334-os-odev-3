//
// Created by lutfullah on 13.06.2022.
//

#include "SISKO32.h"

SISKO32::SISKO32(char *imajDosyasi) : imajDosyasi(imajDosyasi) {
    imajFP = fopen(imajDosyasi, "r+");
    fread(&fat_boot, sizeof(BPB_struct), 1, imajFP);

    clusterAzamiEntrySayisi = fat_boot.BytesPerSector * fat_boot.SectorsPerCluster / 32;

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

// todo:
bool SISKO32::dizinEntrileriEkle(vector<FatFileEntry> &entriler) {
    return false;
}

Dizin::Dizin(SISKO32 *sisko32) : sisko32(sisko32) {
    kokeCik();
}

bool Dizin::kokDizindir() {
    return dizinKelimeleri.empty();
}

bool Dizin::in(string &altDizin) {
    vector<FatFileLFN> longFileNamesBuffer;
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sonrakiClusterID(suankiCid)) {

        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {
            FatFileEntry bilinmeyenDosya;
            fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
            if (bilinmeyenDosya.longFileNamedir()) {
                longFileNamesBuffer.push_back(bilinmeyenDosya.lfn);
            } else {
                pair<vector<FatFileLFN>, FatFile83> cocukDizin = {longFileNamesBuffer, bilinmeyenDosya.msdos};
                longFileNamesBuffer.clear();
                if (dizinBosVeSonrasiYok(cocukDizin)) {
                    break;
                } else if (dizinSilinmis(cocukDizin)) {
                    //
                } else {

                    string cocukDizinAdi = dizinAdi(cocukDizin);
                    if (cocukDizinAdi == altDizin) {
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
        }
    }
    return false;
}

void Dizin::kokeCik() {
    fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(sisko32->rootClusterID), SEEK_SET);
    dizin = dizinOku();
    dizinClusterID = sisko32->rootClusterID;
    dizinKelimeleri.clear();
}

pair<vector<FatFileLFN>, FatFile83> Dizin::dizinOku() {
    vector<FatFileLFN> longFileNames;
    FatFile83 file;
    while (true) {
        FatFileEntry bilinmeyenDosya;
        fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
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
    fseek(sisko32->imajFP, sizeof(FatFileEntry), SEEK_CUR);
}

string Dizin::dizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti) {
    // todo: uzun isimler denenecek. bir de kisa isimle uzun isim ayni mi?
    string uzun = uzunDizinAdi(dizinAdCifti);
    string kisa = kisaDizinAdi(dizinAdCifti);
    if (uzun == "") {
        return kisa;
    } else {
        return uzun;
    }
}

string Dizin::filenamedenStrye(uint8_t *fname, int len) {
    string sonuc;
    for (int i = 0; i < len; ++i) {
        if (fname[i] && fname[i] != ' ') {
            sonuc += (char) fname[i];
        }
    }
    return sonuc;
}

string Dizin::uzunDizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti) {
    string sonuc;
    if (!dizinAdCifti.first.empty()) {
        vector<uint16_t> utf16string = FatFileLFN::birlesikLFN(dizinAdCifti.first);
        sonuc += FatFileLFN::utf16DenAsciiYe(utf16string);
    }
    return sonuc;
}

string Dizin::kisaDizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti) {
    string sonuc;
    sonuc += filenamedenStrye(dizinAdCifti.second.filename, 8);
    string uzanti = filenamedenStrye(dizinAdCifti.second.extension, 3);
    if (uzanti != "") {
        sonuc += ".";
        sonuc += uzanti;
    }
    return sonuc;
}

bool Dizin::git(string &yol) {
    auto gidilecekDizinKelimeleri = ayir(yol, '/');
    return git(gidilecekDizinKelimeleri);
}

bool Dizin::git(vector<string> &gidilecekDizinKelimeleri) {
    Dizin orijinalDizin = *this;
    if (gidilecekDizinKelimeleri[0] == "") { // mesela /drives/etc
        kokeCik();
    }
    for (auto &kelime: gidilecekDizinKelimeleri) {
        if (kelime == "..") {
            if (kokDizindir()) {
                *this = orijinalDizin;
                return false;
            } else {
                ustDizineCik();
            }
        } else if (kelime == ".") { // mesela ./drives/./etc
            //
        } else if (kelime == "") { // mesela ./drives//etc
            //
        } else {
            if (!in(kelime)) {
                *this = orijinalDizin;
                return false;
            }
        }
    }
    return true;
}


void Dizin::ls(bool ayrintili) {
    vector<FatFileLFN> longFileNamesBuffer;
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sonrakiClusterID(suankiCid)) {

        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {
            FatFileEntry bilinmeyenDosya;
            fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
            if (bilinmeyenDosya.longFileNamedir()) {
                longFileNamesBuffer.push_back(bilinmeyenDosya.lfn);
            } else {
                pair<vector<FatFileLFN>, FatFile83> cocukDizin = {longFileNamesBuffer, bilinmeyenDosya.msdos};
                longFileNamesBuffer.clear();
                if (dizinBosVeSonrasiYok(cocukDizin)) {
                    break;
                } else if (dizinSilinmis(cocukDizin)) {
                    //
                } else {
                    if (!noktaDizinidir(cocukDizin)) {
                        string cocukDizinAdi = dizinAdi(cocukDizin);
                        if (ayrintili) {
                            if (klasordur(cocukDizin)) {
                                cout << "d";
                            } else {
                                cout << "-";
                            }
                            cout << "rwx------ root root ";
                            if (klasordur(cocukDizin)) {
                                cout << "0 ";
                            } else {
                                cout << cocukDizin.second.fileSize << " ";
                            }
                            string tarihVeSaat = FatFile83::tarihSaatYaziyaDonustur(
                                    cocukDizin.second.modifiedDate,
                                    cocukDizin.second.modifiedTime);
                            cout << tarihVeSaat;
                            cout << " " << cocukDizinAdi << endl;
                        } else {
                            cout << cocukDizinAdi << " ";
                        }
                    }
                }
            }
        }
    }
    if (!ayrintili) {
        cout << endl;
    }
}

unsigned int Dizin::sonrakiClusterID(uint32_t id) {
    auto first_fat_sector = sisko32->fat_boot.ReservedSectorCount;
    unsigned char FAT_table[sisko32->fat_boot.BytesPerSector];
    unsigned int fat_offset = id * 4;
    unsigned int fat_sector = first_fat_sector + (fat_offset / sisko32->fat_boot.BytesPerSector);
    unsigned int ent_offset = fat_offset % sisko32->fat_boot.BytesPerSector;

    fseek(sisko32->imajFP, fat_sector * sisko32->fat_boot.BytesPerSector, SEEK_SET);
    fread(&FAT_table, sizeof(FAT_table), 1, sisko32->imajFP);

    unsigned int table_value = *(unsigned int *) &FAT_table[ent_offset];
    table_value &= 0x0FFFFFFF;
    return table_value;

}

bool Dizin::dizinBosVeSonrasiYok(pair<vector<FatFileLFN>, FatFile83> dizin1) {
    uint8_t ilkBayt = ((uint8_t *) &dizin1.second)[0];
    return ilkBayt == 0;
}

bool Dizin::dizinSilinmis(pair<vector<FatFileLFN>, FatFile83> dizin1) {
    uint8_t ilkBayt = ((uint8_t *) &dizin1.second)[0];
    return ilkBayt == 0xE5 || ilkBayt == 0x05;
}

bool Dizin::klasordur(const pair<vector<FatFileLFN>, FatFile83> &dizin1) {
    return dizin1.second.attributes == 0x10;
}

bool Dizin::noktaDizinidir(pair<vector<FatFileLFN>, FatFile83> dizin1) {
    return ((uint8_t *) (&dizin1.second))[0] == 0x2E;
}

bool Dizin::mkdir(const string &dizinAdi) {
    vector<FatFileEntry> entriler = dizinEntrileriOlustur(dizinAdi, true);
    sisko32->dizinEntrileriEkle(entriler);
    return true;
}

bool Dizin::touch(const string &dosyaAdi) {
    vector<FatFileEntry> entriler = dizinEntrileriOlustur(dosyaAdi, false);
    sisko32->dizinEntrileriEkle(entriler);
    return true;
}

// todo:
vector<FatFileEntry> Dizin::dizinEntrileriOlustur(const string &dizinAdi, bool klasordur) {
    return vector<FatFileEntry>();
}

void Dizin::cat() {
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sonrakiClusterID(suankiCid)) {

        auto *sektor = new uint8_t[sisko32->fat_boot.BytesPerSector];
        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->fat_boot.SectorsPerCluster; ++i) {
            fread(sektor, sizeof(uint8_t) * sisko32->fat_boot.BytesPerSector, 1, sisko32->imajFP);
            if (sektor[0] != '\0') {
                printf("%s", sektor);
            }
        }
        delete[] sektor;
    }
}
