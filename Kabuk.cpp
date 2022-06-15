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
    auto listelemeDizini = Dizin(&sisko32);

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
    auto listelemeDizini = Dizin(&sisko32);

    vector<string> gidilecekDizinKelimeleri = ayir(arglar[1], '/');

    string dosyaAdi = gidilecekDizinKelimeleri.back();
    gidilecekDizinKelimeleri.pop_back();

    if(listelemeDizini.git(gidilecekDizinKelimeleri)) {
        if (listelemeDizini.git(dosyaAdi)) { // dosya zaten varmis.
            promptYaz();
            return;
        }
        listelemeDizini.touch(dosyaAdi);
    }

    promptYaz();
}

void Kabuk::mv(std::vector<string> &arglar) {
    /**
     * todo: kolay yolu:
     * 1 - traversal yapıp babasını bul, babasının çocuğu olan entry'i silik olarak işaretle. bu entry en üst klasör zaten
     * Çocuk entryi kopyala.
     * eklenecek yere çocuk olarak ekle. zaten en üst klasör olduğu için başka bir şey yapmana gerek yok.
     */
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
