#include "data.h"

ReadStatus loadFlatfile(
  Filepath filepath, void *catcher,
  bool (*onCategory)(Category const*, void *),
  bool (*onRecord)(ProductRecord const*, void *)) {
  return READ_OK;
}

ReadStatus loadDemoFlatfile(
    void* catcher,
    bool (* onDefCategory)(Category const*, void*),
    bool (* onDefRecord)(ProductRecord const*, void*)) {
}
