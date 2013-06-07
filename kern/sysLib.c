#include "sysLib.h"
#include "config.h"

char *sysPhysMemTop (void) {
  return ((char*)LOCAL_MEM_LOCAL_ADRS) + LOCAL_MEM_SIZE;
}

char *sysMemTop (void) {
  return ((char*)LOCAL_MEM_LOCAL_ADRS) + LOCAL_MEM_SIZE - USER_RESERVED_RAM;
}
