#pragma once
#include "badge_detector.h"

View* ui_result_alloc(App* app);
void  ui_result_refresh(View* v, const BadgeInfo* info);
