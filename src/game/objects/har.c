#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "game/objects/har.h"
#include "game/objects/scrap.h"
#include "game/objects/projectile.h"
#include "game/protos/intersect.h"
#include "game/scenes/arena.h"
#include "game/game_state.h"
#include "resources/af_loader.h"
#include "resources/ids.h"
#include "resources/animation.h"
#include "controller/controller.h"
#include "utils/log.h"

void har_free(object *obj) {
    har *h = object_get_userdata(obj);
    af_free(&h->af_data);
    free(h);
}

// Simple helper function
void har_set_ani(object *obj, int animation_id, int repeat) {
    har *har = object_get_userdata(obj);
    object_set_animation(obj, &af_get_move(&har->af_data, animation_id)->ani);
    object_set_repeat(obj, repeat);
    object_tick(obj);
    har->damage_done = 0;
    har->damage_received = 0;
}

// Callback for spawning new objects, eg. projectiles
void cb_har_spawn_object(object *parent, int id, vec2i pos, int g, void *userdata) {
    har *har = userdata;
    af_move *move = af_get_move(&har->af_data, id);
    if(move != NULL) {
        vec2i npos;
        npos.x = parent->pos.x + pos.x + move->ani.start_pos.x;
        npos.y = parent->pos.y + pos.y + move->ani.start_pos.y;

        object *obj = malloc(sizeof(object));
        object_create(obj, npos, vec2f_create(0,0));
        object_set_stl(obj, object_get_stl(parent));
        object_set_palette(obj, object_get_palette(parent), 0);
        object_set_animation(obj, &move->ani);
        // Set all projectiles to their own layer + har layer
        object_set_layers(obj, LAYER_PROJECTILE|LAYER_HAR); 
        // To avoid projectile-to-projectile collisions, set them to same group
        object_set_group(obj, GROUP_PROJECTILE); 
        object_set_repeat(obj, 1);
        object_set_direction(obj, object_get_direction(parent));
        projectile_create(obj);
        game_state_add_object(obj);
    }
}

void har_move(object *obj) {
    har *har = object_get_userdata(obj);
    vec2f vel = object_get_vel(obj);
    obj->pos.x += vel.x;
    obj->pos.y += (vel.y*0.003);

    if(obj->pos.y > 190) {
        // We collided with ground, so set vertical velocity to 0 and
        // make sure object is level with ground
        obj->pos.y = 190;
        object_set_vel(obj, vec2f_create(vel.x, 0));

        // Change animation from jump to walk or idle,
        // depending on horizontal velocity
        // If HAR has been hit in mid-air, then
        // har should be in RECOIL state and needs to be set to GETTING_UP
        if(har->state == STATE_JUMPING) {
            if(object_get_hstate(obj) == OBJECT_MOVING) {
                har->state = STATE_WALKING;
                har_set_ani(obj, ANIM_WALKING, 1);
            } else {
                har->state = STATE_STANDING;
                har_set_ani(obj, ANIM_IDLE, 1);
            }
        } else if(har->state == STATE_JUMP_RECOIL) {
            har->state = STATE_GETTING_UP;
            har_set_ani(obj, ANIM_STANDUP, 0);
        }
    } else if(har->state == STATE_JUMPING || har->state == STATE_JUMP_RECOIL) {
        object_set_vel(obj, vec2f_create(vel.x, vel.y + obj->gravity));
    }
}

void har_take_damage(object *obj, int damage_type) {
    har *har = object_get_userdata(obj);
    har->health -= 25;

    // Set hit animation
    object_set_animation(obj, &af_get_move(&har->af_data, ANIM_DAMAGE)->ani);
    object_set_repeat(obj, 0);
    if(har->state == STATE_CROUCHING) {
        object_set_custom_string(obj, "G3-H3-I3-H3-G3");
    } else if(damage_type == DAMAGETYPE_HIGH) {
        object_set_custom_string(obj, "A3-B3-C3-B3-A3");
    } else if(damage_type == DAMAGETYPE_LOW) {
        object_set_custom_string(obj, "D3-E3-F3-E3-D3");
    }
    object_tick(obj);
}

void har_spawn_scrap(object *obj, vec2i pos) {
    float amount = 5;
    float rv = 0.0f;
    float velx, vely;
    har *har = object_get_userdata(obj);
    for(int i = 0; i < amount; i++) {
        // Calculate velocity etc.
        rv = (rand() % 100) / 100.0f - 0.5;
        velx = 5 * cos(90 + i-(amount) / 2 + rv);
        vely = -12 * sin(i / amount + rv);

        // Create the object
        object *scrap = malloc(sizeof(object));
        int anim_no = rand() % 3 + ANIM_SCRAP_METAL;
        object_create(scrap, pos, vec2f_create(velx, vely));
        object_set_animation(scrap, &af_get_move(&har->af_data, anim_no)->ani);
        object_set_palette(scrap, object_get_palette(obj), 0);
        object_set_repeat(scrap, 1);
        object_set_gravity(scrap, 1);
        object_set_layers(scrap, LAYER_SCRAP);
        object_tick(scrap);
        scrap_create(scrap);
        game_state_add_object(scrap);
    }
}

void har_check_closeness(object *obj_a, object *obj_b) {
    vec2i pos_a = object_get_pos(obj_a);
    vec2i pos_b = object_get_pos(obj_b);
    har *a = object_get_userdata(obj_a);
    int hard_limit = 35; // Stops movement if HARs too close. Harrison-Stetson method value.
    int soft_limit = 45; // Sets HAR A as being close to HAR B if closer than this.

    // Reset closeness state
    a->close = 0;

    // If HARs get too close together, handle it
    if(a->state == STATE_WALKING && object_get_direction(obj_a) == OBJECT_FACE_LEFT) {
        if(pos_a.x < pos_b.x + hard_limit && pos_a.x > pos_b.x) {
            pos_a.x = pos_b.x + hard_limit;
            object_set_pos(obj_a, pos_a);
        }
        if(pos_a.x < pos_b.x + soft_limit && pos_a.x > pos_b.x) {
            a->close = 1;
        }
    }
    if(a->state == STATE_WALKING && object_get_direction(obj_a) == OBJECT_FACE_RIGHT) {
        if(pos_a.x + hard_limit > pos_b.x && pos_a.x < pos_b.x) {
            pos_a.x = pos_b.x - hard_limit;
            object_set_pos(obj_a, pos_a);
        }
        if(pos_a.x + soft_limit > pos_b.x && pos_a.x < pos_b.x) {
            a->close = 1;
        }
    }
}

void har_collide_with_har(object *obj_a, object *obj_b) {
    har *a = object_get_userdata(obj_a);
    har *b = object_get_userdata(obj_b);

    // Check for collisions by sprite collision points
    int level = 2;
    int dmgtype = 0;
    vec2i hit_coord;
    if(a->damage_done == 0 && intersect_sprite_hitpoint(obj_a, obj_b, level, &hit_coord)) {
        dmgtype = (hit_coord.y > 140) ? DAMAGETYPE_LOW : DAMAGETYPE_HIGH;
        har_take_damage(obj_b, dmgtype);
        har_spawn_scrap(obj_b, hit_coord);
        a->damage_done = 1;
        b->damage_received = 1;
        if(b->state == STATE_JUMPING) {
            b->state = STATE_JUMP_RECOIL;
        } else {
            b->state = STATE_RECOIL;
        }
        vec2f vel = object_get_vel(obj_b);
        vel.x = 0.0f;
        object_set_vel(obj_b, vel);
        DEBUG("HAR %s to HAR %s collision at %d,%d!", 
            get_id_name(a->id), 
            get_id_name(b->id),
            hit_coord.x,
            hit_coord.y);
    }
}

void har_collide_with_projectile(object *o_har, object *o_pjt) {
    har *h = object_get_userdata(o_har);

    // Check for collisions by sprite collision points
    int level = 1;
    vec2i hit_coord;
    if(h->damage_done == 0 && intersect_sprite_hitpoint(o_pjt, o_har, level, &hit_coord)) {
        har_take_damage(o_har, DAMAGETYPE_HIGH);
        har_spawn_scrap(o_har, hit_coord);
        h->damage_received = 1;
        h->state = STATE_RECOIL;
        vec2f vel = object_get_vel(o_har);
        vel.x = 0.0f;
        object_set_vel(o_har, vel);
        o_pjt->animation_state.finished = 1;
        DEBUG("PROJECTILE %d to HAR %s collision at %d,%d!", 
            object_get_animation(o_pjt)->id, 
            get_id_name(h->id),
            hit_coord.x,
            hit_coord.y);
    }
}

void har_collide(object *obj_a, object *obj_b) {
    // Check if this is projectile to har collision
    if(object_get_layers(obj_a) & LAYER_PROJECTILE) {
        har_collide_with_projectile(obj_b, obj_a);
        return;
    }
    if(object_get_layers(obj_b) & LAYER_PROJECTILE) {
        har_collide_with_projectile(obj_a, obj_b);
        return;
    }

    // Check for closeness between HARs and handle it
    har_check_closeness(obj_a, obj_b);
    har_check_closeness(obj_b, obj_a);

    // Handle har collisions
    har_collide_with_har(obj_a, obj_b);
    har_collide_with_har(obj_b, obj_a);
}

void har_tick(object *obj) {
    // Make sure HAR doesn't walk through walls
    // TODO: Roof!
    vec2i pos = object_get_pos(obj);
    if(pos.x <  15) pos.x = 15;
    if(pos.x > 305) pos.x = 305;
    object_set_pos(obj, pos);
}

void add_input(har *har, char c) {
    // only add it if it is not the current head of the array
    if(har->inputs[0] == c) {
        return;
    }

    // use memmove to move everything over one spot in the array, leaving the first slot free
    memmove((har->inputs)+1, har->inputs, 9);
    // write the new first element
    har->inputs[0] = c;
}

void har_act(object *obj, int act_type) {
    har *har = object_get_userdata(obj);
    int direction = object_get_direction(obj);
    if(!(har->state == STATE_STANDING ||
         har->state == STATE_CROUCHING ||
         har->state == STATE_WALKING ||
         har->state == STATE_JUMPING ||
         har->state == STATE_JUMP_RECOIL ||
         har->state == STATE_GETTING_UP)) {
        // doing something else, ignore input
        return;
    }

    // Don't allow movement if arena is starting or ending
    int arena_state = arena_get_state(game_state_get_scene());
    if(arena_state == ARENA_STATE_STARTING || arena_state == ARENA_STATE_ENDING) {
        return;
    }

    // Don't allow new moves while we're still executing a previous one.
    if(har->executing_move) {
        return;
    }

   // for the reason behind the numbers, look at a numpad sometime
    switch(act_type) {
        case ACT_UP:
            add_input(har, '8');
            break;
        case ACT_DOWN:
            add_input(har, '2');
            break;
        case ACT_LEFT:
            if(direction == OBJECT_FACE_LEFT) {
                add_input(har, '6');
            } else {
                add_input(har, '4');
            }
            break;
        case ACT_RIGHT:
            if(direction == OBJECT_FACE_LEFT) {
                add_input(har, '4');
            } else {
                add_input(har, '6');
            }
            break;
        case ACT_UPRIGHT:
            if(direction == OBJECT_FACE_LEFT) {
                add_input(har, '7');
            } else {
                add_input(har, '9');
            }
            break;
        case ACT_UPLEFT:
            if(direction == OBJECT_FACE_LEFT) {
                add_input(har, '9');
            } else {
                add_input(har, '7');
            }
            break;
        case ACT_DOWNRIGHT:
            if(direction == OBJECT_FACE_LEFT) {
                add_input(har, '1');
            } else {
                add_input(har, '3');
            }
            break;
        case ACT_DOWNLEFT:
            if(direction == OBJECT_FACE_LEFT) {
                add_input(har, '3');
            } else {
                add_input(har, '1');
            }
            break;
        case ACT_KICK:
            add_input(har, 'K');
            break;
        case ACT_PUNCH:
            add_input(har, 'P');
            break;
        case ACT_STOP:
            add_input(har, '5');
            break;
    }

    af_move *move;
    size_t len;
    for(int i = 0; i < 70; i++) {
        if((move = af_get_move(&har->af_data, i))) {
            len = move->move_string.len;
            if(!strncmp(str_c(&move->move_string), har->inputs, len)) {
                DEBUG("matched move %d with string %s", i, str_c(&move->move_string));
                DEBUG("input was %s", har->inputs);

                // Stop horizontal movement, when move is done
                // TODO: Make this work better
                vec2f spd = object_get_vel(obj);
                spd.x = 0.0f;
                object_set_vel(obj, spd);

                // Set correct animation etc.
                // executing_move = 1 prevents new moves while old one is running.
                har_set_ani(obj, i, 0);
                har->inputs[0] = '\0';
                har->executing_move = 1;
                return;
            }
        }
    }

    if(obj->pos.y < 190) {
        // airborne
        return;
    }

    // no moves matched, do player movement
    float vx, vy;
    switch(act_type) {
        case ACT_DOWN:
        case ACT_DOWNRIGHT:
        case ACT_DOWNLEFT:
            if(har->state != STATE_CROUCHING) {
                har_set_ani(obj, ANIM_CROUCHING, 1);
                object_set_vel(obj, vec2f_create(0,0));
                har->state = STATE_CROUCHING;
            }
            break;
        case ACT_STOP:
            if(har->state != STATE_STANDING) {
                har_set_ani(obj, ANIM_IDLE, 1);
                object_set_vel(obj, vec2f_create(0,0));
                obj->slide_state.vel.x = 0;
                har->state = STATE_STANDING;
            }
            break;
        case ACT_LEFT:
            if(har->state != STATE_WALKING) {
                har_set_ani(obj, ANIM_WALKING, 1);
                har->state = STATE_WALKING;
            }
            vx = har->af_data.reverse_speed*-1/(float)320;
            if(direction == OBJECT_FACE_LEFT) {
                vx = (har->af_data.forward_speed*-1)/(float)320;
            }
            object_set_vel(obj, vec2f_create(vx,0));
            break;
        case ACT_RIGHT:
            if(har->state != STATE_WALKING) {
                har_set_ani(obj, ANIM_WALKING, 1);
                har->state = STATE_WALKING;
            }
            vx = har->af_data.forward_speed/(float)320;
            if(direction == OBJECT_FACE_LEFT) {
                vx = har->af_data.reverse_speed/(float)320;
            }
            object_set_vel(obj, vec2f_create(vx,0));
            break;
        case ACT_UP:
            if(har->state != STATE_JUMPING) {
                har_set_ani(obj, ANIM_JUMPING, 1);
                vy = (float)har->af_data.jump_speed;
                object_set_vel(obj, vec2f_create(0,vy));
                har->state = STATE_JUMPING;
            }
            break;
        case ACT_UPLEFT:
            if(har->state != STATE_JUMPING) {
                har_set_ani(obj, ANIM_JUMPING, 1);
                vy = (float)har->af_data.jump_speed;
                vx = har->af_data.reverse_speed*-1/(float)320;
                if(direction == OBJECT_FACE_LEFT) {
                    vx = (har->af_data.forward_speed*-1)/(float)320;
                }
                object_set_vel(obj, vec2f_create(vx,vy));
                har->state = STATE_JUMPING;
            }
            break;
        case ACT_UPRIGHT:
            if(har->state != STATE_JUMPING) {
                har_set_ani(obj, ANIM_JUMPING, 1);
                vy = (float)har->af_data.jump_speed;
                vx = har->af_data.forward_speed/(float)320;
                if(direction == OBJECT_FACE_LEFT) {
                    vx = har->af_data.reverse_speed/(float)320;
                }
                object_set_vel(obj, vec2f_create(vx,vy));
                har->state = STATE_JUMPING;
            }
            break;
    }
}

void har_finished(object *obj) {
    har *har = object_get_userdata(obj);
    if(har->state == STATE_JUMPING) {
        har_set_ani(obj, ANIM_JUMPING, 1);
    } else if(har->state == STATE_CROUCHING) {
        har_set_ani(obj, ANIM_CROUCHING, 1);
    } else {
        har->state = STATE_STANDING;
        har_set_ani(obj, ANIM_IDLE, 1);
    }
    har->executing_move = 0;
}

void har_fix_sprite_coords(animation *ani, int fix_x, int fix_y) {
    iterator it;
    sprite *s;
    vector_iter_begin(&ani->sprites, &it);
    while((s = iter_next(&it)) != NULL) {
        s->pos.x += fix_x;
        s->pos.y += fix_y;
    }
}

int har_create(object *obj, palette *pal, int dir, int har_id) {
    // Create local data
    har *local = malloc(sizeof(har));
    object_set_userdata(obj, local);

    // Load AF
    if(load_af_file(&local->af_data, har_id)) {
        PERROR("Unable to load HAR %s (%d)!", get_id_name(har_id), har_id);
        free(local);
        return 1;
    }

    // Fix some coordinates on jump sprites
    har_fix_sprite_coords(&af_get_move(&local->af_data, ANIM_JUMPING)->ani, 0, -50);

    // Save har id
    local->id = har_id;

    // Health, endurance
    local->health_max = local->health = 1000;
    local->endurance_max = local->endurance = 1000;
    local->close = 0;
    local->state = STATE_STANDING;
    local->executing_move = 0;

    // Object related stuff
    object_set_gravity(obj, local->af_data.fall_speed);
    object_set_layers(obj, LAYER_HAR);
    object_set_direction(obj, dir);
    object_set_repeat(obj, 1);
    object_set_stl(obj, local->af_data.sound_translation_table);

    // New object spawner callback
    object_set_spawn_cb(obj, cb_har_spawn_object, local);

    // Set running animation 
    object_set_palette(obj, pal, 0);
    har_set_ani(obj, ANIM_IDLE, 1);

    // fill the input buffer with 'pauses'
    memset(local->inputs, '5', 10);
    local->inputs[10] = '\0';

    // Callbacks and userdata
    object_set_free_cb(obj, har_free);
    object_set_act_cb(obj, har_act);
    object_set_tick_cb(obj, har_tick);
    object_set_move_cb(obj, har_move);
    object_set_collide_cb(obj, har_collide);
    object_set_finish_cb(obj, har_finished);

    // All done
    DEBUG("Har %s (%d) loaded!", get_id_name(har_id), har_id);
    return 0;
}
