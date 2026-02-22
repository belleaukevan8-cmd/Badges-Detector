#include "ui_result.h"
#include <gui/elements.h>
#include <furi_hal_rtc.h>
#include <input/input.h>
#include <string.h>
#include <stdio.h>

typedef struct { BadgeInfo info; char dt[24]; App* app; } ResultModel;

static void draw_cb(Canvas* c, void* m) {
    ResultModel* mdl = m;
    canvas_clear(c);
    canvas_set_color(c, ColorBlack);
    canvas_draw_box(c, 0, 0, 128, 14);
    canvas_set_color(c, ColorWhite);
    canvas_set_font(c, FontPrimary);
    canvas_draw_str_aligned(c, 64, 3, AlignCenter, AlignTop, "Résultat");
    canvas_set_color(c, ColorBlack);
    canvas_set_font(c, FontSecondary);
    char buf[48];
    snprintf(buf, sizeof(buf), "Type : %.20s", mdl->info.type_str);
    canvas_draw_str(c, 2, 25, buf);
    snprintf(buf, sizeof(buf), "Proto: %.20s", mdl->info.proto_str);
    canvas_draw_str(c, 2, 35, buf);
    snprintf(buf, sizeof(buf), "UID  : %.24s", mdl->info.uid_str[0] ? mdl->info.uid_str : "-");
    canvas_draw_str(c, 2, 45, buf);
    canvas_draw_str(c, 2, 55, mdl->dt);
    elements_button_left(c, "Retour");
}

static bool input_cb(InputEvent* ev, void* ctx) {
    View* v = ctx;
    if(ev->type != InputTypeShort) return false;
    if(ev->key == InputKeyBack || ev->key == InputKeyLeft) {
        with_view_model(v, ResultModel* m, {
            view_dispatcher_switch_to_view(m->app->vd, ViewMain);
        }, false);
        return true;
    }
    return false;
}

View* ui_result_alloc(App* app) {
    View* v = view_alloc();
    view_set_context(v, v);
    view_allocate_model(v, ViewModelTypeLocking, sizeof(ResultModel));
    view_set_draw_callback(v, draw_cb);
    view_set_input_callback(v, input_cb);
    with_view_model(v, ResultModel* m, {
        memset(&m->info, 0, sizeof(BadgeInfo));
        snprintf(m->info.type_str,  sizeof(m->info.type_str),  "Inconnu");
        snprintf(m->info.proto_str, sizeof(m->info.proto_str), "-");
        snprintf(m->info.uid_str,   sizeof(m->info.uid_str),   "-");
        snprintf(m->dt, sizeof(m->dt), "--:--  --/--/----");
        m->app = app;
    }, false);
    return v;
}

void ui_result_refresh(View* v, const BadgeInfo* info) {
    FuriHalRtcDateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    with_view_model(v, ResultModel* m, {
        m->info = *info;
        snprintf(m->dt, sizeof(m->dt), "%02d:%02d  %02d/%02d/%04d",
            dt.hour, dt.minute, dt.day, dt.month, dt.year);
    }, true);
}
