#include "badge_detector.h"
#include "ui_main.h"
#include "ui_result.h"
#include <string.h>

int32_t badge_detector_app(void* p) {
    UNUSED(p);

    App* app = malloc(sizeof(App));
    furi_assert(app);
    memset(app, 0, sizeof(App));

    /* Ouvrir les records système */
    app->gui     = furi_record_open(RECORD_GUI);
    app->notif   = furi_record_open(RECORD_NOTIFICATION);
    app->storage = furi_record_open(RECORD_STORAGE);

    /* ViewDispatcher */
    app->vd = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->vd);
    view_dispatcher_attach_to_gui(app->vd, app->gui, ViewDispatcherTypeFullscreen);

    /* Créer les vues */
    app->view_main   = ui_main_alloc(app);
    app->view_result = ui_result_alloc(app);

    view_dispatcher_add_view(app->vd, ViewMain,   app->view_main);
    view_dispatcher_add_view(app->vd, ViewResult, app->view_result);

    /* État initial du dernier badge */
    memset(&app->last, 0, sizeof(BadgeInfo));
    snprintf(app->last.type_str,  sizeof(app->last.type_str),  "Inconnu");
    snprintf(app->last.proto_str, sizeof(app->last.proto_str), "-");
    snprintf(app->last.uid_str,   sizeof(app->last.uid_str),   "-");

    /* Init log CSV */
    storage_mgr_init(app->storage);

    /* Lancer */
    view_dispatcher_switch_to_view(app->vd, ViewMain);
    view_dispatcher_run(app->vd);

    /* Nettoyage */
    view_dispatcher_remove_view(app->vd, ViewMain);
    view_dispatcher_remove_view(app->vd, ViewResult);
    view_free(app->view_main);
    view_free(app->view_result);
    view_dispatcher_free(app->vd);

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    free(app);
    return 0;
}
