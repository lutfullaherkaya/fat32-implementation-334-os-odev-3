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
        listelemeDizini->git(*yol);
    }

    listelemeDizini->ls(ayrintili);

    if (listelemeDizini != &suAnkiDizin) {
        delete listelemeDizini;
    }
    promptYaz();
}

void Kabuk::mkdir(std::vector<string> &arglar) {

}

void Kabuk::touch(std::vector<string> &arglar) {

}

void Kabuk::mv(std::vector<string> &arglar) {

}

void Kabuk::cat(std::vector<string> &arglar) {

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
