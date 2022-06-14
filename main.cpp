//
// Created by lutfullah on 13.06.2022.
//

#include "main.h"

using namespace std;

int main(int argc, char** argv) {
    SISKO32 sisko(argv[1]);
    Kabuk kabuk(&sisko);
    vector<string> komutKelimeleri;
    kabuk.promptYaz();
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



