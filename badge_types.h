#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BADGE_UID_MAX 10

typedef struct {
    char    type_str[32];
    char    proto_str[32];
    uint8_t uid[BADGE_UID_MAX];
    uint8_t uid_len;
    char    uid_str[BADGE_UID_MAX * 3 + 1];
    bool    valid;
} BadgeInfo;

static inline void badge_build_uid_str(BadgeInfo* b) {
    b->uid_str[0] = '\0';
    for(uint8_t i = 0; i < b->uid_len; i++) {
        char h[4];
        __builtin_snprintf(h, sizeof(h), i == 0 ? "%02X" : ":%02X", b->uid[i]);
        uint8_t l = (uint8_t)__builtin_strlen(b->uid_str);
        uint8_t r = sizeof(b->uid_str) - l - 1;
        if(r == 0) break;
        for(uint8_t j = 0; h[j] && r; j++, r--) b->uid_str[l++] = h[j];
        b->uid_str[l] = '\0';
    }
}
