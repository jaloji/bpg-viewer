/* FFmpeg's libavutil/internal.h forces the linker to keep avpriv_strtod and avpriv_snprintf when _MSC_VER is
   defined; they are FFmpeg's fallbacks for old MSVC C runtimes and are not part of libbpg. The current Windows
   C runtime is C99 compliant, so they forward to it. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

double avpriv_strtod(const char* nptr, char** endptr) { return strtod(nptr, endptr); }

int avpriv_snprintf(char* s, size_t n, const char* fmt, ...) {
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vsnprintf(s, n, fmt, ap);
    va_end(ap);
    return ret;
}
