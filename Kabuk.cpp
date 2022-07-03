//
// Created by lutfullah on 13.06.2022.
//

#include "Kabuk.h"
#include "Bilimum.h"


void Kabuk::cd(vector<string> &arglar) {
    Dizin cdDizini(suAnkiDizin);
    if (cdDizini.git(arglar[1])) {
        if (Dizin::klasordur(cdDizini.dizin)) {
            suAnkiDizin = cdDizini;
        }
    }

}

/**
 * @param arglar:
 * ls
 * ls path
 * ls -l
 * ls -l path
 */
void Kabuk::ls(std::vector<string> &arglar) {
    string *yol = nullptr;
    bool ayrintili = false;
    if (arglar.size() > 1) {
        if (arglar[1] == "-l") {
            ayrintili = true;
        } else {
            yol = &arglar[1];
        }
        if (arglar.size() == 3) {
            yol = &arglar[2];
        }
    }

    Dizin listelemeDizini(suAnkiDizin);
    if (yol) {
        if (!listelemeDizini.git(*yol)) {
            return;
        }
    }
    listelemeDizini.ls(ayrintili);
}

void Kabuk::mkdir(std::vector<string> &arglar) {
    dosyaOlustur(arglar, true);
}

void Kabuk::touch(std::vector<string> &arglar) {
    dosyaOlustur(arglar, false);
}

void Kabuk::dosyaOlustur(vector<string> &arglar, bool klasordur) {
    auto listelemeDizini = Dizin(suAnkiDizin);

    vector<string> gidilecekDizinKelimeleri = ayir(arglar[1], '/');
    string dizinAdi = gidilecekDizinKelimeleri.back();
    gidilecekDizinKelimeleri.pop_back();

    if (listelemeDizini.git(gidilecekDizinKelimeleri)) {
        if (!listelemeDizini.git(dizinAdi)) { // zaten varmis.
            listelemeDizini.dosyaOlustur(dizinAdi, klasordur);
        }
    }
}

void Kabuk::mv(std::vector<string> &arglar) {
    Dizin kaynakDizinAnnesi(suAnkiDizin);
    Dizin hedefDizin(suAnkiDizin);
    if (arglar.size() != 3 || !hedefDizin.git(arglar[2])) {
        return;
    }

    vector<string> kaynakDizinKelimeleri = ayir(arglar[1], '/');
    string kaynakDizinAdi = kaynakDizinKelimeleri.back();
    kaynakDizinKelimeleri.pop_back();

    if (kaynakDizinAnnesi.git(kaynakDizinKelimeleri)) {
        Dizin kaynakDizin(kaynakDizinAnnesi);
        if (kaynakDizin.git(kaynakDizinAdi) && !kaynakDizin.kokDizindir() && kaynakDizin != suAnkiDizin) {
            if (kaynakDizinAnnesi.fatFileEntrySil(kaynakDizinAdi)) {
                hedefDizin.dizinEntrileriEkle(kaynakDizin.dizin);

                auto anaID = hedefDizin.dizinClusterID;
                hedefDizin.git(kaynakDizinAdi);

                FatFile83 noktaNokta = sisko32.diskEntriOku(sisko32.clusterBaytAdresi(hedefDizin.dizinClusterID) + sizeof(FatFile83)).msdos;
                noktaNokta.firstCluster = anaID & 0x0000FFFF;
                noktaNokta.eaIndex = anaID >> 16;
                sisko32.diskEntriYaz(sisko32.clusterBaytAdresi(hedefDizin.dizinClusterID) + sizeof(FatFile83), (FatFileEntry*)&noktaNokta);
            }
        }
    }
}

void Kabuk::cat(std::vector<string> &arglar) {
    auto okunacakDizin = Dizin(&sisko32);
    if (okunacakDizin.git(arglar[1])) {
        okunacakDizin.cat();
    }
}


Kabuk::Kabuk(SISKO32 *sisko32Ptr) : sisko32(*sisko32Ptr), suAnkiDizin(sisko32Ptr) {
}

void Kabuk::promptYaz() {
    if (suAnkiDizin.dizinKelimeleri.empty()) {
        cout << "/> ";
    } else {
        for (auto &kelime: suAnkiDizin.dizinKelimeleri) {
            cout << "/" << kelime;
        }
        cout << "> ";
    }

}


