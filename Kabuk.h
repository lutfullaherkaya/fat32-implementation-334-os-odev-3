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
using namespace std;

class Kabuk {

public:
    void cd(std::vector<string> &arglar);

    void ls(std::vector<string> &arglar);

    void mkdir(std::vector<string> &arglar);

    void touch(std::vector<string> &arglar);

    void mv(std::vector<string> &arglar);

    void cat(std::vector<string> &arglar);
};


#endif //INC_334_OS_ODEV_3_KABUK_H
