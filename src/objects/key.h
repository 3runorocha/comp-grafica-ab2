#ifndef OBJECTS_KEY_H
#define OBJECTS_KEY_H

#include "../node.h"

// chave antiga de latão
typedef struct {
    Node *root;
} Key;

// cria a chave
void key_create(Key *k, vec3 pos, float yaw_deg);

#endif
