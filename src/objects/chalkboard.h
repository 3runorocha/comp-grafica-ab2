#ifndef OBJECTS_CHALKBOARD_H
#define OBJECTS_CHALKBOARD_H

#include "../node.h"

// quadro-negro em cavalete
typedef struct {
    Node *root;
} Chalkboard;

// cria o quadro-negro
void chalkboard_create(Chalkboard *c, vec3 pos, float yaw_deg);

#endif
