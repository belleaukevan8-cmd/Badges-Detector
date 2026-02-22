#include "nfc_reader.h"

#include <furi.h>
#include <lib/nfc/nfc.h>
#include <lib/nfc/nfc_scanner.h>
#include <lib/nfc/protocols/nfc_protocol.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    NfcProtocol proto;
    bool        done;
    FuriMutex*  mtx;
} NfcCtx;

static void nfc_scanner_cb(NfcScannerEvent event, void* ctx) {
    NfcCtx* c = ctx;
    /* NfcScannerEventTypeDetected = card found.
       event.data is NfcScannerEventData which contains:
         - NfcProtocol protocols[NfcProtocolNum]
         - size_t protocol_count                        */
    if(event.type == NfcScannerEventTypeDetected) {
        furi_mutex_acquire(c->mtx, FuriWaitForever);
        if(!c->done && event.data.protocol_count > 0) {
            c->proto = event.data.protocols[0];
            c->done  = true;
        }
        furi_mutex_release(c->mtx);
    }
}

static const char* proto_name(NfcProtocol p) {
    switch(p) {
    case NfcProtocolIso14443_3a:  return "ISO 14443-A";
    case NfcProtocolIso14443_3b:  return "ISO 14443-B";
    case NfcProtocolIso14443_4a:  return "ISO 14443-4A";
    case NfcProtocolIso15693_3:   return "ISO 15693";
    case NfcProtocolMfClassic:    return "Mifare Classic";
    case NfcProtocolMfUltralight: return "NTAG/Ultralight";
    case NfcProtocolMfPlus:       return "Mifare Plus";
    case NfcProtocolMfDesfire:    return "Mifare DESFire";
    default:                       return "NFC Inconnu";
    }
}

bool nfc_read(BadgeInfo* out, uint32_t timeout_ms) {
    furi_assert(out);
    memset(out, 0, sizeof(BadgeInfo));

    NfcCtx ctx = {.done = false, .mtx = furi_mutex_alloc(FuriMutexTypeNormal)};

    Nfc*        nfc  = nfc_alloc();
    NfcScanner* scan = nfc_scanner_alloc(nfc);
    nfc_scanner_start(scan, nfc_scanner_cb, &ctx);

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

    nfc_scanner_stop(scan);
    nfc_scanner_free(scan);
    nfc_free(nfc);

    if(found) {
        snprintf(out->type_str,  sizeof(out->type_str),  "NFC 13.56MHz");
        snprintf(out->proto_str, sizeof(out->proto_str), "%s", proto_name(ctx.proto));
        snprintf(out->uid_str,   sizeof(out->uid_str),   "Detected");
        out->valid = true;
    }

    furi_mutex_free(ctx.mtx);
    return found;
}
