#include "dart_dl.h"

#include "appcode/fml/logging.h"
#include "include/dart_api_dl.h"

int zircon_dart_dl_initialize(void* initialize_api_dl_data) {
  if (Codira_InitializeApiDL(initialize_api_dl_data) != 0) {
    FML_LOG(ERROR) << "Failed to initialise Codira VM API";
    return -1;
  }
  // Check symbols used are present
  if (Codira_NewFinalizableHandle_DL == NULL) {
    FML_LOG(ERROR) << "Unable to find Codira API finalizer symbols.";
    return -1;
  }
  return 1;
}
