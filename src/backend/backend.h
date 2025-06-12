#ifndef __BACKEND_H
#define __BACKEND_H

#include <string>

namespace backend {
    bool compile(const std::string& inputFile, const std::string& outputFile, bool emitAsm = false);
}

#endif
