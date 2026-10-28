#ifndef OBJECTS_STEPLADDER_H
#define OBJECTS_STEPLADDER_H

#include "../node.h"

// escadinha de metal de dois degraus
typedef struct {
    Node *root;
} Stepladder;

// cria a escadinha
void stepladder_create(Stepladder *s, vec3 pos, float yaw_deg);

#endif
