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

/**
 *
 * @param isim
 * @return en cok 3 harflik uzanti
 */
string getUzanti(string& isim) {
    int i;
    for (i = isim.size()-1; i >= 0; --i) {
        if (isim[i] == '.') {
            break;
        }
    }
    if (i == -1) {
        return "";
    } else {
        string sonuc;
        for (int j = i+1; j < i+4; ++j) {
            if (j < isim.size()) {
                sonuc += isim[j];
            }
        }
        return sonuc;
    }
}
void buyukHarfYap(char * str, int n) {
    for (int i = 0; i < n; ++i) {
        str[i] = toupper(str[i]);
    }
}