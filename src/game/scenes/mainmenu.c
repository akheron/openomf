#include <SDL2/SDL.h>
#include <shadowdive/shadowdive.h>
#include "utils/log.h"
#include "game/text/text.h"
#include "audio/music.h"
#include "game/scene.h"
#include "game/scenes/mainmenu.h"
#include "game/menu/menu.h"
#include "game/menu/textbutton.h"

font font_small;
menu smenu;
component pvp_button;
component tourn_button;
component settings_button;
component exit_button;

void mainmenu_quit(component *c, void *userdata) {
    scene *scene = userdata;
    scene->next_id = SCENE_CREDITS;
}

int mainmenu_init(scene *scene) {
    // Force music playback
    if(!music_playing()) {
        music_play("resources/MENU.PSM");
    }
    
    // Create font
    font_create(&font_small);
    if(font_load(&font_small, "resources/CHARSMAL.DAT", FONT_SMALL)) {
        PERROR("Error while loading small font!");
        font_free(&font_small);
        return 1;
    }
    
    // Create menu
    menu_create(&smenu, 160, 20, 140, 200);
    textbutton_create(&pvp_button, &font_small, "1 vs. 1");
    textbutton_create(&tourn_button, &font_small, "Tournament");
    textbutton_create(&settings_button, &font_small, "Settings");
    textbutton_create(&exit_button, &font_small, "Exit");
    menu_attach(&smenu, &pvp_button, 10);
    menu_attach(&smenu, &tourn_button, 10);
    menu_attach(&smenu, &settings_button, 10);
    menu_attach(&smenu, &exit_button, 10);
    
    // Events
    exit_button.userdata = (void*)scene;
    exit_button.click = mainmenu_quit;
    
    // All done
    return 0;
}

void mainmenu_deinit(scene *scene) {
    textbutton_free(&settings_button);
    textbutton_free(&tourn_button);
    textbutton_free(&pvp_button);
    textbutton_free(&exit_button);
    menu_free(&smenu);
    font_free(&font_small);
}

void mainmenu_tick(scene *scene) {
    menu_tick(&smenu);
}

int mainmenu_event(scene *scene, SDL_Event *event) {
    // the menu will need to handle escapes to exit submenus
    // but we're going to hijack them for now
    if(event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        scene->next_id = SCENE_CREDITS;
        return 1;
    }
    return menu_handle_event(&smenu, event);
}

void mainmenu_render(scene *scene) {
    menu_render(&smenu);
}

void mainmenu_load(scene *scene) {
    scene->event = mainmenu_event;
    scene->render = mainmenu_render;
    scene->init = mainmenu_init;
    scene->deinit = mainmenu_deinit;
    scene->tick = mainmenu_tick;
}

