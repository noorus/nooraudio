#pragma once

// Windows Vista or newer
#define NTDDI_VERSION NTDDI_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#include <sdkddkver.h>

// Generic headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// COM/WASAPI
#include <comdef.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>

// STL
#include <exception>
#include <vector>
#include <string>
#include <map>

// Boost
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>