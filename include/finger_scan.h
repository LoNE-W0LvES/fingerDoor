#pragma once

#include "config.h"

// Fixed naming: "initialize" instead of "initilize", and consistent camelCase for "matchFingerprint"
void fingerprint_initialize();
uint8_t matchFingerprint();

// Backward compatibility macros for previous naming typos
#define fingerprint_initilize fingerprint_initialize
#define matchFingerPrint      matchFingerprint
