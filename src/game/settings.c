#include "game/settings.h"
#include "utils/config.h"
#include "utils/log.h"
#include <stddef.h> //offsetof
#include <stdlib.h>
#include <string.h>

#define F_INT(struct_, var, def) {#var, TYPE_INT, {.i=def}, offsetof(struct_, var)}
#define F_BOOL(struct_, var, def) {#var, TYPE_BOOL, {.b=def}, offsetof(struct_, var)}
#define F_FLOAT(struct_, var, def) {#var, TYPE_FLOAT, {.f=def}, offsetof(struct_, var)}
#define F_STRING(struct_, var, def) {#var, TYPE_STRING, {.s=def}, offsetof(struct_, var)}

#define NFIELDS(struct_) sizeof(struct_)/sizeof(field)

static settings _settings;
static const char *settings_path;

typedef enum field_type_t {
    TYPE_INT, 
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING
} field_type;

typedef union field_default_u {
    char *s;
    int i;
    int b;
    double f;
} field_default;

typedef struct field_t {
    char *name;
    field_type type;
    field_default def;
    int offset;
} field;

const field f_video[] = {
    F_INT(settings_video,  screen_w,      640),
    F_INT(settings_video,  screen_h,      400),
    F_BOOL(settings_video, vsync,           0),
    F_BOOL(settings_video, fullscreen,      0),
    F_INT(settings_video,  scaling,         0),
    F_BOOL(settings_video, instant_console, 0)
};

const field f_sound[] = {
    F_BOOL(settings_sound, sound_on,        1),
    F_BOOL(settings_sound, music_on,        1),
    F_BOOL(settings_sound, stereo_on,       1),
    F_BOOL(settings_sound, stereo_reversed, 0),
    F_INT(settings_sound,  sound_vol,       5),
    F_INT(settings_sound,  music_vol,       5),
    F_STRING(settings_sound, music_arena0, ""),
    F_STRING(settings_sound, music_arena1, ""),
    F_STRING(settings_sound, music_arena2, ""),
    F_STRING(settings_sound, music_arena3, ""),
    F_STRING(settings_sound, music_arena4, ""),
    F_STRING(settings_sound, music_end,    ""),
    F_STRING(settings_sound, music_menu,   "")
};

const field f_gameplay[] = {
    F_INT(settings_gameplay,  speed,       5),
    F_INT(settings_gameplay,  fight_mode,  0),
    F_INT(settings_gameplay,  power1,      5),
    F_INT(settings_gameplay,  power2,      5),
    F_BOOL(settings_gameplay, hazards_on,  1),
    F_INT(settings_gameplay,  difficulty,  1),
    F_INT(settings_gameplay,  rounds,      1)
};

const field f_keyboard[] = {
    // Player one
    F_STRING(settings_keyboard, key1_up,    "Up"),
    F_STRING(settings_keyboard, key1_down,  "Down"),
    F_STRING(settings_keyboard, key1_left,  "Left"),
    F_STRING(settings_keyboard, key1_right, "Right"),
    F_STRING(settings_keyboard, key1_kick,  "Right Shift"),
    F_STRING(settings_keyboard, key1_punch, "Return"),

    // Player two
    F_STRING(settings_keyboard, key2_up,    "W"),
    F_STRING(settings_keyboard, key2_down,  "S"),
    F_STRING(settings_keyboard, key2_left,  "A"),
    F_STRING(settings_keyboard, key2_right, "D"),
    F_STRING(settings_keyboard, key2_kick,  "Left Ctrl"),
    F_STRING(settings_keyboard, key2_punch, "Left Shift")
};

int *fieldint(void *st, int offset) {
    return (int*)((char*)st + offset);
}
double *fieldfloat(void *st, int offset) {
    return (double*)((char*)st + offset);
}
char* *fieldstr(void *st, int offset) {
    return (char**)((char*)st + offset);
}
int *fieldbool(void *st, int offset) {
    return fieldint(st, offset);
}

void settings_add_fields(const field *fields, int nfields) {
    for(int i=0;i < nfields;++i) {
        const field *f = &fields[i];
        switch(f->type) {
            case TYPE_INT:
                conf_addint(f->name, f->def.i);
                break;
                
            case TYPE_FLOAT:
                conf_addfloat(f->name, f->def.f);
                break;
                
            case TYPE_BOOL:
                conf_addbool(f->name, f->def.b);
                break;
                
            case TYPE_STRING:
                conf_addstring(f->name, f->def.s);
                break;
        }
    }
}

void settings_load_fields(void *st, const field *fields, int nfields) {
    for(int i=0;i < nfields;++i) {
        const field *f = &fields[i];
        switch(f->type) {
            case TYPE_INT:
                *fieldint(st, f->offset) = conf_int(f->name);
                break;
                
            case TYPE_FLOAT:
                *fieldfloat(st, f->offset) = conf_float(f->name);
                break;
                
            case TYPE_BOOL:
                *fieldbool(st, f->offset) = conf_bool(f->name);
                break;
                
            case TYPE_STRING:
                {
                    // make a copy of the string
                    char **s = fieldstr(st, f->offset);
                    if(*s) {
                        free(*s);
                    }
                    const char *s2 = conf_string(f->name);
                    *s = malloc(strlen(s2)+1);
                    strcpy(*s, s2);
                }
                break;
        }
    }
}

void settings_save_fields(void *st, const field *fields, int nfields) {
    for(int i=0;i < nfields;++i) {
        const field *f = &fields[i];
        switch(f->type) {
            case TYPE_INT:
                conf_setint(f->name, *fieldint(st, f->offset));
                break;
                
            case TYPE_FLOAT:
                conf_setfloat(f->name, *fieldfloat(st, f->offset));
                break;
                
            case TYPE_BOOL:
                conf_setbool(f->name, *fieldbool(st, f->offset));
                break;
                
            case TYPE_STRING:
                conf_setstring(f->name, *fieldstr(st, f->offset));
                break;
        }
    }
}

void settings_free_strings(void *st, const field *fields, int nfields) {
    for(int i = 0; i < nfields; ++i) {
        const field *f = &fields[i];
        if(f->type == TYPE_STRING) {
            char **s = fieldstr(st, f->offset);
            if(*s) { 
                free(*s); 
                *s = NULL;
            }
        }
    }
}

int settings_write_defaults(const char *path) {
    int r = 0;
    settings_init(path);
    if(conf_write_config(path)) {
        r = 1;
    }
    settings_free();
    return r;
}

int settings_init(const char *path) {
    settings_path = path;
    memset(&_settings, 0, sizeof(settings));
    settings_add_fields(f_video, NFIELDS(f_video));
    settings_add_fields(f_sound, NFIELDS(f_sound));
    settings_add_fields(f_gameplay, NFIELDS(f_gameplay));
    settings_add_fields(f_keyboard, NFIELDS(f_keyboard));
    return conf_init(settings_path);
}

void settings_load() {
    settings_load_fields(&_settings.video, f_video, NFIELDS(f_video));
    settings_load_fields(&_settings.sound, f_sound, NFIELDS(f_sound));
    settings_load_fields(&_settings.gameplay, f_gameplay, NFIELDS(f_gameplay));
    settings_load_fields(&_settings.keys, f_keyboard, NFIELDS(f_keyboard));
}

void settings_save() {
    settings_save_fields(&_settings.video, f_video, NFIELDS(f_video));
    settings_save_fields(&_settings.sound, f_sound, NFIELDS(f_sound));
    settings_save_fields(&_settings.gameplay, f_gameplay, NFIELDS(f_gameplay));
    settings_save_fields(&_settings.keys, f_keyboard, NFIELDS(f_keyboard));
    if(conf_write_config(settings_path)) {
        PERROR("Failed to write config file!\n");
    }
}

void settings_free() {
    settings_free_strings(&_settings.video, f_video, NFIELDS(f_video));
    settings_free_strings(&_settings.sound, f_sound, NFIELDS(f_sound));
    settings_free_strings(&_settings.gameplay, f_gameplay, NFIELDS(f_gameplay));
    settings_free_strings(&_settings.keys, f_keyboard, NFIELDS(f_keyboard));
    conf_close();
}

settings *settings_get() {
    return &_settings;
}

