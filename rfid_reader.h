#pragma once
#include "badge_types.h"

bool rfid_read(BadgeInfo* out, uint32_t timeout_ms);
