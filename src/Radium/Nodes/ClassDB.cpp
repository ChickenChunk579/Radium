#include <Radium/Nodes/ClassDB.hpp>

namespace Radium::Nodes::ClassDB {
    std::unordered_map<std::string, ClassInfo> registeredClasses;
}
namespace Radium::Nodes {
    std::string Demangle(const char* name) {
        int status = -4; // some arbitrary value to eliminate the compiler warning

        // enable c++11 by passing the flag -std=c++11 to g++
        char* res = abi::__cxa_demangle(name, NULL, NULL, &status);

        const char* const demangledName = (status == 0) ? res : name;

        std::string ret(demangledName);
        free(res);
        return ret;
    }
}