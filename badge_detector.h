#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>
#include "badge_types.h"
#include "rfid_reader.h"
#include "nfc_reader.h"
#include "storage_manager.h"

typedef enum { ViewMain = 0, ViewResult = 1 } AppViewId;

typedef struct {
    Gui*             gui;
    ViewDispatcher*  vd;
    NotificationApp* notif;
    Storage*         storage;
    View*            view_main;
    View*            view_result;
    BadgeInfo        last;
} App;

int32_t badge_detector_app(void* p);
