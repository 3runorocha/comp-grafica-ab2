#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include <GLFW/glfw3.h>

// personagem controlado pelo jogador
typedef struct {
    vec3 pos;
    float yaw;          // direção em graus
    float walk_phase;   // fase do passo
    float walk_blend;   // 0 parado, 1 andando
    Character model;
    int has_model;
} Player;

// carrega o personagem
void player_init(Player *p, vec3 pos);
// movimento e animação
void player_update(Player *p, GLFWwindow *win, float dt);
// desenha o personagem
void player_draw(Player *p);

#endif
