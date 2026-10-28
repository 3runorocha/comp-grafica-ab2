#ifndef OBJECTS_CHAIR_H
#define OBJECTS_CHAIR_H

#include "../node.h"

// cadeira de madeira estofada
typedef struct {
    Node *root;
    vec3 tucked, pulled;   // posições guardada e puxada
    float out, target;     // 0 guardada, 1 puxada
} Chair;

#define CHAIR_W 0.44f   // largura da cadeira
#define CHAIR_D 0.42f   // profundidade da cadeira

// cria a cadeira
void chair_create(Chair *c, vec3 pos, float yaw_deg);

// define o trilho da cadeira
void chair_set_track(Chair *c, vec3 tucked, vec3 pulled);
// alterna guardada/puxada
void chair_toggle(Chair *c);
// anima o deslizamento
void chair_update(Chair *c, float dt);

#endif
