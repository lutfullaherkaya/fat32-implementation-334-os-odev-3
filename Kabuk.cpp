//
// Created by lutfullah on 13.06.2022.
//

#include "Kabuk.h"
#include "Bilimum.h"


void Kabuk::cd(vector<string> &arglar) {
    suAnkiDizin.git(arglar[1]);
    promptYaz();
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
    Dizin *listelemeDizini = &suAnkiDizin;
    if (yol) {
        listelemeDizini = new Dizin(&sisko32);
        if (!listelemeDizini->git(*yol)) {
            delete listelemeDizini;
            promptYaz();
            return;
        }
    }

    listelemeDizini->ls(ayrintili);

    if (listelemeDizini != &suAnkiDizin) {
        delete listelemeDizini;
    }
    promptYaz();
}

void Kabuk::mkdir(std::vector<string> &arglar) {
    auto listelemeDizini = Dizin(suAnkiDizin);

    vector<string> gidilecekDizinKelimeleri = ayir(arglar[1], '/');
    string dizinAdi = gidilecekDizinKelimeleri.back();
    gidilecekDizinKelimeleri.pop_back();

    if (listelemeDizini.git(gidilecekDizinKelimeleri)) {
        if (listelemeDizini.git(dizinAdi)) { // klasor zaten varmis.
            promptYaz();
            return;
        }
        listelemeDizini.mkdir(dizinAdi);
    }

    promptYaz();
}

void Kabuk::touch(std::vector<string> &arglar) {
    auto listelemeDizini = Dizin(suAnkiDizin);

    vector<string> gidilecekDizinKelimeleri = ayir(arglar[1], '/');
    string dosyaAdi = gidilecekDizinKelimeleri.back();
    gidilecekDizinKelimeleri.pop_back();

    if (listelemeDizini.git(gidilecekDizinKelimeleri)) {
        if (listelemeDizini.git(dosyaAdi)) { // dosya zaten varmis.
            promptYaz();
            return;
        }
        listelemeDizini.touch(dosyaAdi);
    }

    promptYaz();
}

void Kabuk::mv(std::vector<string> &arglar) {
    Dizin hedefDizin(suAnkiDizin);
    if (!hedefDizin.git(arglar[2])) {
        promptYaz();
        return;
    }
    vector<string> gidilecekDizinKelimeleri = ayir(arglar[1], '/');
    if (!gidilecekDizinKelimeleri.empty() && arglar.size() == 3) {
        auto listelemeDizini = Dizin(suAnkiDizin);
        string dosyaAdi = gidilecekDizinKelimeleri.back();
        gidilecekDizinKelimeleri.pop_back();

        if (listelemeDizini.git(gidilecekDizinKelimeleri) && listelemeDizini.git(dosyaAdi)) {
            pair<vector<FatFileLFN>, FatFile83> dizin = listelemeDizini.dizin;
            listelemeDizini.ustDizineCik();
            if (listelemeDizini.fatFileEntrySil(dosyaAdi)) {
                vector<FatFileEntry> entriler;
                for (auto &lfn: dizin.first) {
                    FatFileEntry ffn;
                    ffn.lfn = lfn;
                    entriler.push_back(ffn);
                }
                FatFileEntry ffn;
                ffn.msdos = dizin.second;
                entriler.push_back(ffn);
                hedefDizin.dizinEntrileriEkle(entriler);
                auto anaAdi = hedefDizin.dizinClusterID;
                hedefDizin.git(dosyaAdi);


                fseek(sisko32.imajFP, sisko32.clusterBaytAdresi(hedefDizin.dizinClusterID) + sizeof(FatFile83),
                      SEEK_SET);
                FatFile83 noktaNokta;
                fread(&noktaNokta, sizeof(FatFile83), 1, sisko32.imajFP);

                noktaNokta.firstCluster = anaAdi & 0x0000FFFF;
                noktaNokta.eaIndex = anaAdi << 16;

                fseek(sisko32.imajFP, sisko32.clusterBaytAdresi(hedefDizin.dizinClusterID) + sizeof(FatFile83),
                      SEEK_SET);
                fwrite(&noktaNokta, sizeof(FatFile83), 1, sisko32.imajFP);
            }

        }
    }
    promptYaz();
}

void Kabuk::cat(std::vector<string> &arglar) {
    auto okunacakDizin = Dizin(&sisko32);
    if (okunacakDizin.git(arglar[1])) {
        okunacakDizin.cat();
    }
    promptYaz();
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
