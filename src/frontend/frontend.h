#ifndef __FRONTEND_H
#define __FRONTEND_H

#include <string>

namespace frontend {
    bool compile(const std::string& inputFile, const std::string& outputFile, bool debug = false);
    bool interpret(const std::string& inputFile, bool debug = false);
}

#endif
