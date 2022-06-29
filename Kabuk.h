//
// Created by lutfullah on 13.06.2022.
//

#ifndef INC_334_OS_ODEV_3_KABUK_H
#define INC_334_OS_ODEV_3_KABUK_H


#include <vector>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "SISKO32.h"

using namespace std;

vector<string> ayir(string &yazi, char ayrac);

class Kabuk {

public:
    explicit Kabuk(SISKO32 *sisko32Ptr);

    void cd(std::vector<string> &arglar);

    void ls(std::vector<string> &arglar);

    void mkdir(std::vector<string> &arglar);

    void touch(std::vector<string> &arglar);

    void mv(std::vector<string> &arglar);

    void cat(std::vector<string> &arglar);

    void promptYaz();

    SISKO32 &sisko32;
    Dizin suAnkiDizin;

    void dosyaOlustur(vector<string> &arglar, bool klasordur);
};


#endif //INC_334_OS_ODEV_3_KABUK_H
