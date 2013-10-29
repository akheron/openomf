#ifndef _HAR_H
#define _HAR_H

#include "resources/af.h"
#include "resources/animation.h"
#include "game/protos/object.h"
#include "utils/list.h"

#define LAYER_HAR 0x02
#define LAYER_SCRAP 0x04
#define LAYER_PROJECTILE 0x08

#define GROUP_PROJECTILE 2

enum {
    CAT_MISC = 0,
    CAT_CLOSE = 2,
    CAT_CROUCH = 4,
    CAT_STANDING = 5,
    CAT_WALKING, // may also include standing
    CAT_JUMPING,
    CAT_PROJECTILE,
    CAT_BASIC,
    CAT_VICTORY = 10, // or defeat
    CAT_SCRAP = 12,
    CAT_DESTRUCTION
};

enum {
    STATE_STANDING,
    STATE_WALKING,
    STATE_CROUCHING,
    STATE_JUMPING,
    STATE_JUMP_RECOIL,
    STATE_RECOIL,
    STATE_STUNNED,
    STATE_VICTORY,
    STATE_DEFEAT,
    STATE_SCRAP,
    STATE_DESTRUCTION,
    STATE_GETTING_UP
};

enum {
    DAMAGETYPE_LOW, // Damage to low area of har
    DAMAGETYPE_HIGH // Damage to high area of har
};

typedef struct har_t {
    int id;
    af af_data;
    unsigned int state;
    unsigned int executing_move;
    int close;
    int damage_done; // Damage was done this animation
    int damage_received; // Damage was received this animation

    int health_max, health;
    int endurance_max, endurance;
    char inputs[10];
} har;

int har_create(object *obj, palette *pal, int dir, int har_id);

#endif // _HAR_H
