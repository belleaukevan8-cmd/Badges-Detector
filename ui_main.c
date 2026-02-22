#include "ui_main.h"
#include <gui/elements.h>
#include <input/input.h>
#include <string.h>

#define N_ITEMS 2
static const char* LABELS[N_ITEMS] = {"Scanner Badge", "Historique"};

typedef struct { uint8_t sel; App* app; } MainModel;

static void draw_cb(Canvas* c, void* m) {
    MainModel* mdl = m;
    canvas_clear(c);
    canvas_set_color(c, ColorBlack);
    canvas_draw_box(c, 0, 0, 128, 14);
    canvas_set_color(c, ColorWhite);
    canvas_set_font(c, FontPrimary);
    canvas_draw_str_aligned(c, 64, 3, AlignCenter, AlignTop, "Badge Detector");
    canvas_set_color(c, ColorBlack);
    canvas_set_font(c, FontSecondary);
    for(uint8_t i = 0; i < N_ITEMS; i++) {
        int16_t y = 24 + (int16_t)(i * 18);
        if(mdl->sel == i) {
            canvas_draw_box(c, 0, y - 10, 128, 13);
            canvas_set_color(c, ColorWhite);
            canvas_draw_str(c, 6, y, LABELS[i]);
            canvas_set_color(c, ColorBlack);
        } else {
            canvas_draw_str(c, 6, y, LABELS[i]);
        }
    }
    elements_button_right(c, "OK");
}

static bool input_cb(InputEvent* ev, void* ctx) {
    View* v = ctx;
    if(ev->type != InputTypeShort && ev->type != InputTypeRepeat) return false;
    bool consumed = true;
    with_view_model(v, MainModel* mdl, {
        switch(ev->key) {
        case InputKeyUp:
            if(mdl->sel > 0) mdl->sel--;
            break;
        case InputKeyDown:
            if(mdl->sel < N_ITEMS - 1) mdl->sel++;
            break;
        case InputKeyOk: {
            App* app = mdl->app;
            if(mdl->sel == 0) {
                /* Scanner */
                BadgeInfo info;
                bool found = rfid_read(&info, 1500);
                if(!found) found = nfc_read(&info, 1500);
                if(!found) {
                    memset(&info, 0, sizeof(BadgeInfo));
                    snprintf(info.type_str,  sizeof(info.type_str),  "Inconnu");
                    snprintf(info.proto_str, sizeof(info.proto_str), "-");
                    snprintf(info.uid_str,   sizeof(info.uid_str),   "-");
                } else {
                    storage_mgr_log(app->storage, &info);
                    notification_message(app->notif, &sequence_success);
                }
                app->last = info;
            }
            view_dispatcher_switch_to_view(mdl->app->vd, ViewResult);
            break;
        }
        case InputKeyBack:
            view_dispatcher_stop(mdl->app->vd);
            break;
        default:
            consumed = false;
            break;
        }
    }, true);
    return consumed;
}

View* ui_main_alloc(App* app) {
    View* v = view_alloc();
    view_set_context(v, v);
    view_allocate_model(v, ViewModelTypeLocking, sizeof(MainModel));
    view_set_draw_callback(v, draw_cb);
    view_set_input_callback(v, input_cb);
    with_view_model(v, MainModel* m, { m->sel = 0; m->app = app; }, false);
    return v;
}
