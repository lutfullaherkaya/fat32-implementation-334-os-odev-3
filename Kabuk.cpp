//
// Created by lutfullah on 13.06.2022.
//

#include "Kabuk.h"

vector<string> ayir(string &yazi, char ayrac) {
    vector<string> sonuc;
    string kelime;
    stringstream ss(yazi);
    while (getline(ss, kelime, ayrac)) {
        sonuc.push_back(kelime);
    }
    return sonuc;
}

void Kabuk::cd(vector<string> &arglar) {
    Dizin orijinalDizin = suAnkiDizin;
    auto dizinKelimeleri = ayir(arglar[1], '/');
    for (auto &kelime: dizinKelimeleri) {
        if (kelime == "..") {
            if (suAnkiDizin.kokDizindir()) {
                suAnkiDizin.dizinKelimeleri = orijinalDizin.dizinKelimeleri;
                suAnkiDizin.dizinClusterID = orijinalDizin.dizinClusterID;
                suAnkiDizin.dizin = orijinalDizin.dizin;
                break;
            } else {
                suAnkiDizin.ustDizineCik();
            }
        } else if (kelime == ".") {
            //
        } else if (kelime == "") {
            suAnkiDizin.kokeCik();
        } else {
            if (!suAnkiDizin.in(kelime)) {
                suAnkiDizin.dizinKelimeleri = orijinalDizin.dizinKelimeleri;
                suAnkiDizin.dizinClusterID = orijinalDizin.dizinClusterID;
                suAnkiDizin.dizin = orijinalDizin.dizin;
                break;
            }
        }
    }
    promptYaz();
}

void Kabuk::ls(std::vector<string> &arglar) {

}

void Kabuk::mkdir(std::vector<string> &arglar) {

}

void Kabuk::touch(std::vector<string> &arglar) {

}

void Kabuk::mv(std::vector<string> &arglar) {

}

void Kabuk::cat(std::vector<string> &arglar) {

}


Kabuk::Kabuk(SISKO32 *sisko32Ptr) : sisko32(*sisko32Ptr), suAnkiDizin(*sisko32Ptr)  {
}

void Kabuk::promptYaz() {
    if (suAnkiDizin.dizinKelimeleri.empty()) {
        cout << "/> ";
    } else {
        for (auto &kelime : suAnkiDizin.dizinKelimeleri) {
            cout << "/" << kelime;
        }
        cout << "> ";
    }

}
