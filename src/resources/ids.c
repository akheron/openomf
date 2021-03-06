#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "resources/ids.h"
#include "game/settings.h"
#include "utils/log.h"

struct music_override_t {
    int id;
    const char *name;
};

void get_filename_by_id(int id, char *ptr) {
    const char* path = "resources/";

    // Declare music overrides
    settings *s = settings_get();
    struct music_override_t overrides[] = {
        {PSM_ARENA0, s->sound.music_arena0},
        {PSM_ARENA1, s->sound.music_arena1},
        {PSM_ARENA2, s->sound.music_arena2},
        {PSM_ARENA3, s->sound.music_arena3},
        {PSM_ARENA4, s->sound.music_arena4},
        {PSM_MENU,   s->sound.music_menu},
        {PSM_END,    s->sound.music_end}
    };

    for(int i = 0; i < 7; i++) {
        if(id == overrides[i].id && strlen(overrides[i].name) > 0) {
            DEBUG("Overriding %s to %s.", get_id_name(id), overrides[i].name);
            sprintf(ptr, "%s", overrides[i].name);
            return;
        }
    }

    switch(id) {
        case SCENE_INTRO:    sprintf(ptr, "%sINTRO.BK", path);    break;
        case SCENE_MENU:     sprintf(ptr, "%sMAIN.BK", path);     break;
        case SCENE_ARENA0:   sprintf(ptr, "%sARENA0.BK", path);   break;
        case SCENE_ARENA1:   sprintf(ptr, "%sARENA1.BK", path);   break;
        case SCENE_ARENA2:   sprintf(ptr, "%sARENA2.BK", path);   break;
        case SCENE_ARENA3:   sprintf(ptr, "%sARENA3.BK", path);   break;
        case SCENE_ARENA4:   sprintf(ptr, "%sARENA4.BK", path);   break;
        case SCENE_NEWSROOM: sprintf(ptr, "%sNEWSROOM.BK", path); break;
        case SCENE_END:      sprintf(ptr, "%sEND.BK", path);      break;
        case SCENE_END1:     sprintf(ptr, "%sEND1.BK", path);     break;
        case SCENE_END2:     sprintf(ptr, "%sEND2.BK", path);     break;
        case SCENE_CREDITS:  sprintf(ptr, "%sCREDITS.BK", path);  break;
        case SCENE_MECHLAB:  sprintf(ptr, "%sMECHLAB.BK", path);  break;
        case SCENE_MELEE:    sprintf(ptr, "%sMELEE.BK", path);    break;
        case SCENE_VS:       sprintf(ptr, "%sVS.BK", path);       break;
        case SCENE_NORTHAM:  sprintf(ptr, "%sNORTH_AM.BK", path); break;
        case SCENE_KATUSHAI: sprintf(ptr, "%sKATUSHAI.BK", path); break;
        case SCENE_WAR:      sprintf(ptr, "%sWAR.BK", path);      break;
        case SCENE_WORLD:    sprintf(ptr, "%sWORLD.BK", path);    break;
        case HAR_JAGUAR:     sprintf(ptr, "%sFIGHTR0.AF", path);  break;
        case HAR_SHADOW:     sprintf(ptr, "%sFIGHTR1.AF", path);  break;
        case HAR_THORN:      sprintf(ptr, "%sFIGHTR2.AF", path);  break;
        case HAR_PYROS:      sprintf(ptr, "%sFIGHTR3.AF", path);  break;
        case HAR_ELECTRA:    sprintf(ptr, "%sFIGHTR4.AF", path);  break;
        case HAR_KATANA:     sprintf(ptr, "%sFIGHTR5.AF", path);  break;
        case HAR_SHREDDER:   sprintf(ptr, "%sFIGHTR6.AF", path);  break;
        case HAR_FLAIL:      sprintf(ptr, "%sFIGHTR7.AF", path);  break;
        case HAR_GARGOYLE:   sprintf(ptr, "%sFIGHTR8.AF", path);  break;
        case HAR_CHRONOS:    sprintf(ptr, "%sFIGHTR9.AF", path);  break;
        case HAR_NOVA:       sprintf(ptr, "%sFIGHTR10.AF", path); break;
        case PSM_MENU:       sprintf(ptr, "%sMENU.PSM", path);    break;
        case PSM_END:        sprintf(ptr, "%sEND.PSM", path);     break;
        case PSM_ARENA0:     sprintf(ptr, "%sARENA0.PSM", path);  break;
        case PSM_ARENA1:     sprintf(ptr, "%sARENA1.PSM", path);  break;
        case PSM_ARENA2:     sprintf(ptr, "%sARENA2.PSM", path);  break;
        case PSM_ARENA3:     sprintf(ptr, "%sARENA3.PSM", path);  break;
        case PSM_ARENA4:     sprintf(ptr, "%sARENA4.PSM", path);  break;
        case DAT_SOUNDS:     sprintf(ptr, "%sSOUNDS.DAT", path);  break;
        case DAT_ENGLISH:    sprintf(ptr, "%sENGLISH.DAT", path); break;
        case DAT_GERMAN:     sprintf(ptr, "%sGERMAN.DAT", path);  break;
        case DAT_GRAPHCHR:   sprintf(ptr, "%sGRAPHCHR.DAT", path); break;
        case DAT_CHARSMAL:   sprintf(ptr, "%sCHARSMAL.DAT", path); break;
        case DAT_ALTPALS:    sprintf(ptr, "%sALTPALS.DAT", path); break;
        default: 
            ptr[0] = 0;
            break;
    }
}

char* get_id_name(int id) {
    switch(id) {
        case SCENE_INTRO:    return "INTRO";
        case SCENE_MENU:     return "MAIN";
        case SCENE_ARENA0:   return "ARENA0";
        case SCENE_ARENA1:   return "ARENA1"; 
        case SCENE_ARENA2:   return "ARENA2"; 
        case SCENE_ARENA3:   return "ARENA3";
        case SCENE_ARENA4:   return "ARENA4";
        case SCENE_NEWSROOM: return "NEWSROOM";
        case SCENE_END:      return "END";
        case SCENE_END1:     return "END1";
        case SCENE_END2:     return "END2";
        case SCENE_CREDITS:  return "CREDITS";
        case SCENE_MECHLAB:  return "MECHLAB";
        case SCENE_MELEE:    return "MELEE";
        case SCENE_VS:       return "VS";
        case SCENE_NORTHAM:  return "NORHAM";
        case SCENE_KATUSHAI: return "KATUSHAI";
        case SCENE_WAR:      return "WAR";
        case SCENE_WORLD:    return "WORLD";
        case HAR_JAGUAR:     return "JAGUAR";
        case HAR_SHADOW:     return "SHADOW";
        case HAR_THORN:      return "THORN";
        case HAR_PYROS:      return "PYROS";
        case HAR_ELECTRA:    return "ELECTRA";
        case HAR_KATANA:     return "KATANA";
        case HAR_SHREDDER:   return "SHREDDER";
        case HAR_FLAIL:      return "FLAIL";
        case HAR_GARGOYLE:   return "GARGOYLE";
        case HAR_CHRONOS:    return "CHRONOS";
        case HAR_NOVA:       return "NOVA";
        case PSM_ARENA0:     return "ARENA0.PSM";
        case PSM_ARENA1:     return "ARENA1.PSM";
        case PSM_ARENA2:     return "ARENA2.PSM";
        case PSM_ARENA3:     return "ARENA3.PSM";
        case PSM_ARENA4:     return "ARENA4.PSM";
        case PSM_MENU:       return "MENU.PSM";
        case PSM_END:        return "END.PSM";
        case DAT_SOUNDS:     return "SOUNDS.DAT";
        case DAT_ENGLISH:    return "ENGLISH.DAT";
        case DAT_GERMAN:     return "GERMAN.DAT";
        case DAT_CHARSMAL:   return "CHARSMAL.DAT";
        case DAT_GRAPHCHR:   return "GRAPHCHR.DAT";
    }
    return "UNKNOWN";
}

int is_scene(int id) {
    return (id >= SCENE_INTRO && id <= SCENE_WORLD);
}

int is_har(int id) {
    return (id >= HAR_JAGUAR && id <= HAR_NOVA);
}

int is_music(int id) {
    return (id >= PSM_ARENA0 && id <= PSM_END);
}