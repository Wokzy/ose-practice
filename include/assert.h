#pragma once

#include "panic.h"

#define assert(x) if (!(x)) { kernel_panic("assertion failed: %s (%s, line %u)", #x, __FILE__, __LINE__); }
