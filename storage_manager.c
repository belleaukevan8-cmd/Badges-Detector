#include "storage_manager.h"
#include <furi_hal_rtc.h>
#include <string.h>
#include <stdio.h>

#define LOG_DIR  EXT_PATH("badge_detector")
#define LOG_PATH EXT_PATH("badge_detector/badge_log.csv")
#define CSV_HDR  "date,heure,type,protocole,uid\r\n"

void storage_mgr_init(Storage* s) {
    if(!storage_dir_exists(s, LOG_DIR))
        storage_simply_mkdir(s, LOG_DIR);
    if(!storage_file_exists(s, LOG_PATH)) {
        File* f = storage_file_alloc(s);
        if(storage_file_open(f, LOG_PATH, FSAM_WRITE, FSOM_CREATE_NEW)) {
            storage_file_write(f, CSV_HDR, strlen(CSV_HDR));
            storage_file_close(f);
        }
        storage_file_free(f);
    }
}

void storage_mgr_log(Storage* s, const BadgeInfo* b) {
    FuriHalRtcDateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    char line[128];
    int  n = snprintf(
        line, sizeof(line),
        "%04d-%02d-%02d,%02d:%02d:%02d,%s,%s,%s\r\n",
        dt.year, dt.month, dt.day,
        dt.hour, dt.minute, dt.second,
        b->type_str[0]  ? b->type_str  : "?",
        b->proto_str[0] ? b->proto_str : "?",
        b->uid_str[0]   ? b->uid_str   : "-");
    if(n <= 0) return;
    File* f = storage_file_alloc(s);
    if(storage_file_open(f, LOG_PATH, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        storage_file_write(f, line, (uint16_t)n);
        storage_file_close(f);
    }
    storage_file_free(f);
}
