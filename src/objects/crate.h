#ifndef OBJECTS_CRATE_H
#define OBJECTS_CRATE_H

#include "../node.h"

// caixote de ripas de madeira
typedef struct {
    Node *root;
} Crate;

#define CRATE_SIZE 0.60f

// cria o caixote
void crate_create(Crate *c, vec3 pos, float yaw_deg);

#endif
