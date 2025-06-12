#ifndef __MIDDLEEND_H
#define __MIDDLEEND_H

#include <string>

namespace middleend {
    bool optimize(const std::string& inputFile, int optLevel);
}

#endif
