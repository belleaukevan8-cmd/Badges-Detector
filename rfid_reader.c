#include "rfid_reader.h"

#include <furi.h>
#include <lib/lfrfid/lfrfid_worker.h>
#include <lib/toolbox/protocols/protocol_dict.h>
#include <string.h>
#include <stdio.h>

/* Symbol exported by the firmware for all RFID protocols */
extern const ProtocolBase* const lfrfid_protocols[];
extern const size_t              lfrfid_protocols_count;

typedef struct {
    ProtocolId pid;
    bool       done;
    FuriMutex* mtx;
} RfidCtx;

static void rfid_worker_cb(LFRFIDWorkerReadResult res, ProtocolId pid, void* ctx) {
    RfidCtx* c = ctx;
    if(res == LFRFIDWorkerReadDone) {
        furi_mutex_acquire(c->mtx, FuriWaitForever);
        c->pid  = pid;
        c->done = true;
        furi_mutex_release(c->mtx);
    }
}

bool rfid_read(BadgeInfo* out, uint32_t timeout_ms) {
    furi_assert(out);
    memset(out, 0, sizeof(BadgeInfo));

    RfidCtx ctx = {.pid = PROTOCOL_NO, .done = false, .mtx = furi_mutex_alloc(FuriMutexTypeNormal)};

    ProtocolDict* dict   = protocol_dict_alloc(lfrfid_protocols, lfrfid_protocols_count);
    LFRFIDWorker* worker = lfrfid_worker_alloc(dict);
    lfrfid_worker_start_thread(worker);
    lfrfid_worker_read_start(worker, LFRFIDWorkerReadTypeAuto, rfid_worker_cb, &ctx);

    uint32_t elapsed = 0;
    bool     found   = false;
    while(elapsed < timeout_ms) {
        furi_delay_ms(50);
        elapsed += 50;
        furi_mutex_acquire(ctx.mtx, FuriWaitForever);
        found = ctx.done;
        furi_mutex_release(ctx.mtx);
        if(found) break;
    }

    lfrfid_worker_stop(worker);
    lfrfid_worker_stop_thread(worker);

    if(found) {
        const char* name = protocol_dict_get_name(dict, (size_t)ctx.pid);
        snprintf(out->type_str,  sizeof(out->type_str),  "RFID 125kHz");
        snprintf(out->proto_str, sizeof(out->proto_str), "%s", name ? name : "Inconnu");

        size_t dsz = protocol_dict_get_data_size(dict, (size_t)ctx.pid);
        if(dsz > BADGE_UID_MAX) dsz = BADGE_UID_MAX;
        protocol_dict_get_data(dict, (size_t)ctx.pid, out->uid, dsz);
        out->uid_len = (uint8_t)dsz;
        badge_build_uid_str(out);
        out->valid = true;
    }

    lfrfid_worker_free(worker);
    protocol_dict_free(dict);
    furi_mutex_free(ctx.mtx);
    return found;
}
