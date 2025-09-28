#pragma once

namespace Rune {

// Empty/no-op hooks by default
inline void TraceZoneBegin(const char* name) {}
inline void TraceZoneEnd() {}

}
