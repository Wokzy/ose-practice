#pragma once

#include "panic.h"

#define assert(x) if (!(x)) { kernel_panic("assertion failed: ##x (%s, line %u)", __FILE__, __LINE__); }
