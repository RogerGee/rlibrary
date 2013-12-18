/* riodevice.cpp
 *  Compile target platform flags:
 *   RLIBRARY_BUILD_POSIX - build targeting POSIX
 *   RLIBRARY_BUILD_WIN32 - build targeting Windows API
 */
#include "riodevice.h"

// define target-specific code
//  -> Gets: 'using namespace rtypes'

#if defined(RLIBRARY_BUILD_POSIX)
#include "riodevice_posix.cpp"
#elif defined(RLIBRARY_BUILD_WIN32)
#include "riodevice_win32.cpp"
#else
using namespace rtypes;
#endif

// define target-independent code
