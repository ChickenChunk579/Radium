#include <string>
#include <Radium/Nodes/ClassDB.hpp>

#if defined(_MSC_VER)
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <dbghelp.h>
    #pragma comment(lib, "dbghelp.lib")
#else
    #include <cxxabi.h>
    #include <cstdlib>
#endif

namespace Radium::Nodes {

    namespace ClassDB {
        std::unordered_map<std::string, ClassDB::ClassInfo> registeredClasses;
        std::vector<std::string> enums;
    }
    

    std::string Demangle(const char* name) {
    #if defined(_MSC_VER)
        char demangledName[1024];
        if (UnDecorateSymbolName(name, demangledName, sizeof(demangledName), UNDNAME_COMPLETE)) {
            std::string result = std::string(demangledName);

            const std::vector<std::string> prefixes = {"class ", "struct ", "enum "};
            for (const auto& prefix : prefixes) {
                if (result.find(prefix) == 0) {
                    result = result.substr(prefix.size());
                    break;
                }
            }

            return result;
        } else {
            // fallback to original if demangling fails
            return std::string(name);
        }
    #else
        int status = -4; // some arbitrary value to eliminate the compiler warning
        char* res = abi::__cxa_demangle(name, nullptr, nullptr, &status);
        const char* const demangledName = (status == 0) ? res : name;
        std::string ret(demangledName);
        free(res);
        return ret;
    #endif
    }
}
