#pragma once
#include <storage/storage.h>
#include "badge_types.h"

void storage_mgr_init(Storage* s);
void storage_mgr_log(Storage* s, const BadgeInfo* b);
