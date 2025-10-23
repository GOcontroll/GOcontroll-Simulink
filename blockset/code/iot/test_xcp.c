/*
 * This file is cursed, moving this into XcpTargetSpecific with SEGGER_RTT
 * stuff breaks the build?
 */

#include <stdint.h>

#include "SYS_config.h"

uint32_t uniqueIdLength = (uint32_t)kXcpStationIdLength;
char uniqueIdString[] = kXcpStationIdString;