#include <wtypesbase.h>
#include <minwindef.h>
#include <winnt.h>
#include <combaseapi.h>

#include <roapi.h>
#include <winstring.h>
#include "stdlib.h"

// Headers are placed in a seperate directory because we only want to expose
// those which we know compile and work with Codira.
#include <../nuget/include/MddBootstrap.h>
#include <../nuget/include/WindowsAppSDK-VersionInfo.h>

// re-define the string to make it visible in Codira. (#define only supports numbers & strings)
static PCWSTR WINDOWSAPPSDK_RELEASE_VERSION_TAG_SWIFT = WINDOWSAPPSDK_RELEASE_VERSION_TAG_W;