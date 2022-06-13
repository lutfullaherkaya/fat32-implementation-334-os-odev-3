//
// Created by lutfullah on 13.06.2022.
//

#include "main.h"

using namespace std;

int main() {
    SISKO32 sisko;
    Kabuk kabuk;
    vector<string> komutKelimeleri;

    do {
        string komut;
        getline(cin, komut);
        komutKelimeleri = ayir(komut, ' ');

        if (!komutKelimeleri.empty()) {
            if (komutKelimeleri[0] == "cd") {
                kabuk.cd(komutKelimeleri);
            } else if (komutKelimeleri[0] == "ls") {
                kabuk.ls(komutKelimeleri);
            } else if (komutKelimeleri[0] == "mkdir") {
                kabuk.mkdir(komutKelimeleri);
            } else if (komutKelimeleri[0] == "touch") {
                kabuk.touch(komutKelimeleri);
            } else if (komutKelimeleri[0] == "mv") {
                kabuk.mv(komutKelimeleri);
            } else if (komutKelimeleri[0] == "cat") {
                kabuk.cat(komutKelimeleri);
            }
        }

    } while (komutKelimeleri.empty() || komutKelimeleri[0] != "quit");

    return 0;
}

vector<string> ayir(string &yazi, char ayrac) {
    vector<string> sonuc;
    string kelime;
    stringstream ss(yazi);
    while (getline(ss, kelime, ayrac)) {
        sonuc.push_back(kelime);
    }
    return sonuc;
}

