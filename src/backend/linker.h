#ifndef __LINKER_H
#define __LINKER_H

#include <string>

namespace backend {
    bool link(const std::string& objectFile, const std::string& outputFile);
}

#endif
