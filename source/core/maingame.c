#include "robot.h"
#include "update.h"
#include "draw.h"

static void updateEntityAux(void* data) {
    entity_t* ent = (entity_t*)data;
    const sprite_t* spr = getSprite(ent->sprite);
    if (!ent || !spr) return;

    // Se o objeto está se movendo...
    if (ent->moving) {
        // Desloca-se "speed" pixels na direção "xdir" e "ydir"
        ent->x += ent->xdir * ent->speed;
        ent->y += ent->ydir * ent->speed;

        // Se o objeto estiver alinhado com a grade...
        if (IS_SNAPPED(ent->x, ent->y)) {
            // Para de se mover
            ent->moving = 0;

            // Verifica em qual tile está pisando
            switch(TILE_GET(ent->x, ent->y)) {
                case TL_HOLE: {
                    ent->visible = 0;
                    error(getMessage(MSG_GAMEOVER), editorStopLevel);
                    return;
                }
                case TL_PORTAL: {
                    if ((TILE_GET(ent->xstart, ent->ystart)!=TL_WALL) && !entityCollision(ent->xstart, ent->ystart, 32, 32, ENT_ANY)) {
                        ent->x = ent->xstart;
                        ent->y = ent->ystart;
                    }
                    break;
                }
            }
            if (ent->id == ENT_BOX) {
                if (TILE_GET(ent->x, ent->y)==TL_BOXPOST) {
                    if ((int)ent->frame==0) {
                        level.boxes--;
                        if (level.boxes == 0) {
                            error(getMessage(MSG_COMPLETE), editorStopLevel);
                            return;
                        }
                    }
                    ent->frame = 1;
                } else {
                    if ((int)ent->frame==1)
                        level.boxes++;
                    ent->frame = 0;
                }
            }
        }
    }
    ent->frame += ent->anispd;
}

static void updateEntities() {
    int i;
    for (i = 0; i < ENT_COUNT; i++) {
        foreach(getEntityList(i), updateEntityAux);
    }
}

void updateMainGame() {
    if (isKeyDown(KEY_CANCEL)) {
        editorStopLevel();
        return;
    }
    updateRobot();
    updateEntities();
}