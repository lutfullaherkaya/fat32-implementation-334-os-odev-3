//
// Created by lutfullah on 13.06.2022.
//

#include <cstring>
#include "SISKO32.h"
#include "Bilimum.h"

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

uint32_t SISKO32::yeniClusterAyir(uint32_t baglanacakClusterID = 0) {
    uint32_t yeniClusterID = freeClusterBul();
    if (baglanacakClusterID) {
        setSISKO(baglanacakClusterID, yeniClusterID);
    }
    setSISKO(yeniClusterID, 0x0FFFFFF8);

    return yeniClusterID;

}

unsigned int SISKO32::getSISKO(uint32_t clusterID) {
    auto first_fat_sector = fat_boot.ReservedSectorCount;
    unsigned char FAT_table[fat_boot.BytesPerSector];
    unsigned int fat_offset = clusterID * 4;
    unsigned int fat_sector = first_fat_sector + (fat_offset / fat_boot.BytesPerSector);
    unsigned int ent_offset = fat_offset % fat_boot.BytesPerSector;

    fseek(imajFP, fat_sector * fat_boot.BytesPerSector, SEEK_SET);
    fread(&FAT_table, sizeof(FAT_table), 1, imajFP);

    unsigned int table_value = *(unsigned int *) &FAT_table[ent_offset];
    table_value &= 0x0FFFFFFF;
    return table_value;
}

uint32_t SISKO32::freeClusterBul() {
    // tamamen verimsiz bir fonkksiyon ama onemli degil
    unsigned long clusterSayisi = fat_boot.TotalSectors32 / fat_boot.SectorsPerCluster;
    for (unsigned long id = 2; id < clusterSayisi; ++id) {
        auto clusterID = getSISKO(id);
        if (clusterID == 0) {
            return id;
        }
    }
    return 0;
}

bool SISKO32::setSISKO(uint32_t baglanacakClusterID, uint32_t degerClusterID) {
    degerClusterID &= 0x0FFFFFFF;

    for (int i = 0; i < fat_boot.NumFATs; ++i) {
        auto first_fat_sector = fat_boot.ReservedSectorCount + i * fat_boot.extended.FATSize;
        unsigned int fat_offset = baglanacakClusterID * 4;
        unsigned int fat_sector = first_fat_sector + (fat_offset / fat_boot.BytesPerSector);
        unsigned int ent_offset = fat_offset % fat_boot.BytesPerSector;

        fseek(imajFP, fat_sector * fat_boot.BytesPerSector + ent_offset, SEEK_SET);
        fwrite(&degerClusterID, sizeof(degerClusterID), 1, imajFP);
    }

    return true;
}


Dizin::Dizin(SISKO32 *sisko32) : sisko32(sisko32) {
    kokeCik();
}

bool Dizin::kokDizindir() {
    return dizinKelimeleri.empty();
}

bool Dizin::in(string altDizin) {
    vector<FatFileLFN> longFileNamesBuffer;
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sisko32->getSISKO(suankiCid)) {

        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {
            FatFileEntry bilinmeyenDosya;
            fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
            if (bilinmeyenDosya.longFileNamedir()) {
                longFileNamesBuffer.push_back(bilinmeyenDosya.lfn);
            } else {
                pair<vector<FatFileLFN>, FatFile83> cocukDizin = {longFileNamesBuffer, bilinmeyenDosya.msdos};
                longFileNamesBuffer.clear();
                if (dizinBosVeSonrasiYok(cocukDizin.second)) {
                    break;
                } else if (dizinSilinmis(cocukDizin.second)) {
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
    if (!kokDizindir()) {
        in("..");
        dizinKelimeleri.pop_back();
        dizinKelimeleri.pop_back();
    }
}

void Dizin::dizinAtla() {
    fseek(sisko32->imajFP, sizeof(FatFileEntry), SEEK_CUR);
}

string Dizin::dizinAdi(pair<vector<FatFileLFN>, FatFile83> &dizinAdCifti) {
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
            ustDizineCik();
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
    bool dizinBos = true;
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sisko32->getSISKO(suankiCid)) {

        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {
            FatFileEntry bilinmeyenDosya;
            fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
            if (bilinmeyenDosya.longFileNamedir()) {
                longFileNamesBuffer.push_back(bilinmeyenDosya.lfn);
            } else {
                pair<vector<FatFileLFN>, FatFile83> cocukDizin = {longFileNamesBuffer, bilinmeyenDosya.msdos};
                longFileNamesBuffer.clear();
                if (dizinBosVeSonrasiYok(cocukDizin.second)) {
                    break;
                } else if (dizinSilinmis(cocukDizin.second)) {
                    //
                } else {
                    if (!noktaDizinidir(cocukDizin)) {
                        dizinBos = false;
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
    if (!ayrintili && !dizinBos) {
        cout << endl;
    }
}


bool Dizin::dizinBosVeSonrasiYok(FatFile83 &dizin1) {
    uint8_t ilkBayt = ((uint8_t *) &dizin1)[0];
    return ilkBayt == 0;
}

bool Dizin::dizinSilinmis(FatFile83 &dizin1) {
    uint8_t ilkBayt = ((uint8_t *) &dizin1)[0];
    return ilkBayt == 0xE5 || ilkBayt == 0x05;
}

bool Dizin::klasordur(const pair<vector<FatFileLFN>, FatFile83> &dizin1) {
    return dizin1.second.attributes == 0x10;
}

bool Dizin::noktaDizinidir(pair<vector<FatFileLFN>, FatFile83> dizin1) {
    return ((uint8_t *) (&dizin1.second))[0] == 0x2E;
}

bool Dizin::dosyaOlustur(string &dosyaAdi, bool klasordur) {
    vector<FatFileEntry> entriler = dizinEntrileriOlustur(dosyaAdi, klasordur);
    return dizinEntrileriEkle(entriler);
}

vector<FatFileEntry> Dizin::dizinEntrileriOlustur(string &dizinAdi, bool klasordur) {
    FatFileEntry dosya;
    dosya.msdos = fatFile83olustur(dizinAdi, klasordur);

    vector<FatFileEntry> dizinEntrileri = FatFileLFNlerOlustur(dizinAdi, dosya.msdos.filename);

    dizinEntrileri.push_back(dosya);

    return dizinEntrileri;
}

void Dizin::cat() {
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sisko32->getSISKO(suankiCid)) {

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

bool Dizin::dizinEntrileriEkle(vector<FatFileEntry> &entriler) {
    if (entriler.empty()) {
        return true;
    }
    FatFileEntry nullEntri{};
    entriler.push_back(nullEntri);

    unsigned long yazilacakEntriIndeks = 0;
    bool yaziliyor = false;
    bool yazmaBitti = false;
    uint32_t sonErisilenCid = dizinClusterID;
    for (auto suankiCid = dizinClusterID;
         !yazmaBitti && FatFile83::clusterVar(suankiCid);
         suankiCid = sisko32->getSISKO(suankiCid)) {
        sonErisilenCid = suankiCid;

        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {

            if (!yaziliyor) {
                FatFileEntry bilinmeyenDosya;
                fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
                if (!bilinmeyenDosya.longFileNamedir()) {
                    if (dizinBosVeSonrasiYok(bilinmeyenDosya.msdos)) {
                        yaziliyor = true;
                        fseek(sisko32->imajFP, -((long) sizeof(bilinmeyenDosya)), SEEK_CUR);
                        i--;
                    } else if (dizinSilinmis(bilinmeyenDosya.msdos)) {
                        // bosluklar doldurulabilir ama gerek yok.
                    }
                }
            } else {
                if (yazilacakEntriIndeks < entriler.size()) {
                    fwrite(&entriler[yazilacakEntriIndeks], sizeof(FatFileEntry), 1, sisko32->imajFP);
                    yazilacakEntriIndeks++;

                }
                if (yazilacakEntriIndeks >= entriler.size()) {
                    yazmaBitti = true;
                    yaziliyor = false;
                    break;
                }

            }
        }
    }
    while (!yazmaBitti) {
        sonErisilenCid = sisko32->yeniClusterAyir(sonErisilenCid);
        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(sonErisilenCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {
            if (yazilacakEntriIndeks < entriler.size()) {
                fwrite(&entriler[yazilacakEntriIndeks], sizeof(FatFileEntry), 1, sisko32->imajFP);
                yazilacakEntriIndeks++;
            }
            if (yazilacakEntriIndeks >= entriler.size()) {
                yazmaBitti = true;
                yaziliyor = false;
                break;
            }
        }
    }
    // null entry entrilerden silinir.
    entriler.pop_back();
}

FatFile83 Dizin::fatFile83olustur(string dizinAdi, bool klasordur) {
    auto yeniClusterID = sisko32->yeniClusterAyir();
    FatFile83 fatFile83 = fatFile83olustur(dizinAdi, klasordur, yeniClusterID);
    if (klasordur) {
        uint32_t anneClusterID = dizinClusterID;
        if (anneClusterID == sisko32->fat_boot.extended.RootCluster) {
            anneClusterID = 0;
        }
        FatFile83 nokta = fatFile83olustur(".", true, yeniClusterID);
        FatFile83 noktaNokta = fatFile83olustur("..", true, anneClusterID);
        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(yeniClusterID), SEEK_SET);
        fwrite(&nokta, sizeof(FatFile83), 1, sisko32->imajFP);
        fwrite(&noktaNokta, sizeof(FatFile83), 1, sisko32->imajFP);
    }
    return fatFile83;
}

FatFile83 Dizin::fatFile83olustur(string dizinAdi, bool klasordur, uint32_t firstClusterID) {
    FatFile83 dosya{};

    memset(&dosya.filename, ' ', 11);
    string uzanti = getUzanti(dizinAdi);
    auto dizinAdiBoyutu = dizinAdi.size();
    if (!uzanti.empty()) {
        strncpy((char *) (dosya.extension), uzanti.c_str(), uzanti.size());
        dizinAdiBoyutu -= uzanti.size() + 1;
    }
    if (dizinAdiBoyutu > 8) {
        strncpy((char *) (dosya.filename), dizinAdi.c_str(), 6);
        dosya.filename[6] = '~';
        dosya.filename[7] = '1'; // todo: burasi kopya dosyalarda 1 den fazla olmalidir. lazim mi?
    } else {
        strncpy((char *) (dosya.filename), dizinAdi.c_str(), dizinAdiBoyutu);
    }
    buyukHarfYap((char *) dosya.filename, 11);
    if (klasordur) {
        dosya.attributes = 0x10;
    } else {
        dosya.attributes = 0x20;
    }
    dosya.setAllDateTimesToNow();

    dosya.firstCluster = firstClusterID & 0x0000FFFF;
    dosya.eaIndex = firstClusterID << 16;
    return dosya;
}

vector<FatFileEntry> Dizin::FatFileLFNlerOlustur(string &dizinAdi, const unsigned char *fatFile83filename) {
    vector<FatFileEntry> dizinEntrileri;
    for (int i = 0; i < dizinAdi.size(); i += 13) {
        FatFileEntry lfn{};
        lfn.lfn.setName(dizinAdi.substr(i, 13));
        lfn.lfn.sequence_number = i + 1;
        lfn.lfn.attributes = 0x0f;
        lfn.lfn.reserved = 0x00;
        lfn.lfn.firstCluster = 0x0000;
        lfn.lfn.checksum = lfn.lfn.lfn_checksum(fatFile83filename);
        dizinEntrileri.push_back(lfn);
    }
    reverse(dizinEntrileri.begin(), dizinEntrileri.end());
    // the entry representing the end of the filename comes first. The sequence number of this entry has bit 6 (0x40) set to represent that it is the last logical LFN entry
    dizinEntrileri[0].lfn.sequence_number |= 0x40;
    return dizinEntrileri;
}

bool Dizin::fatFileEntrySil(string altDizin) {
    vector<FatFileLFN> longFileNamesBuffer;
    for (auto suankiCid = dizinClusterID;
         FatFile83::clusterVar(suankiCid);
         suankiCid = sisko32->getSISKO(suankiCid)) {

        fseek(sisko32->imajFP, sisko32->clusterBaytAdresi(suankiCid), SEEK_SET);
        for (int i = 0; i < sisko32->clusterAzamiEntrySayisi; ++i) {
            FatFileEntry bilinmeyenDosya;
            fread(&bilinmeyenDosya, sizeof(bilinmeyenDosya), 1, sisko32->imajFP);
            if (bilinmeyenDosya.longFileNamedir()) {
                longFileNamesBuffer.push_back(bilinmeyenDosya.lfn);
            } else {
                pair<vector<FatFileLFN>, FatFile83> cocukDizin = {longFileNamesBuffer, bilinmeyenDosya.msdos};
                auto lfnSayisi = longFileNamesBuffer.size();
                longFileNamesBuffer.clear();
                if (dizinBosVeSonrasiYok(cocukDizin.second)) {
                    break;
                } else if (dizinSilinmis(cocukDizin.second)) {
                    //
                } else {
                    string cocukDizinAdi = dizinAdi(cocukDizin);
                    if (cocukDizinAdi == altDizin) {
                        FatFileEntry silinmisDosya{};
                        ((uint8_t *) &silinmisDosya.msdos.filename)[0] = 0xE5;
                        fseek(sisko32->imajFP, -((lfnSayisi + 1) * sizeof(FatFileEntry)), SEEK_CUR);
                        for (int j = 0; j < (lfnSayisi + 1); ++j) {
                            fwrite(&silinmisDosya, sizeof(FatFileEntry), 1, sisko32->imajFP);
                        }
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
