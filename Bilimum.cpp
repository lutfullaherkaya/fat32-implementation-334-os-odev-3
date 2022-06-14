//
// Created by lutfullah on 14.06.2022.
//

#include "Bilimum.h"

using namespace std;

vector<string> ayir(string &yazi, char ayrac) {
    vector<string> sonuc;
    string kelime;
    stringstream ss(yazi);
    while (getline(ss, kelime, ayrac)) {
        sonuc.push_back(kelime);
    }
    return sonuc;
}
