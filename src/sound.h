#ifndef SOUND_H
#define SOUND_H

// sons do jogo
typedef enum {
    SND_MUSIC,      // trilha de fundo
    SND_STEP_L,     // passo esquerdo
    SND_STEP_R,     // passo direito
    SND_GLOBE,      // globo girando
    SND_DRAWER,     // gaveta
    SND_FIRE,       // lareira
    SND_SWITCH,     // interruptor do abajur
    SND_CHAIR,      // cadeira deslizando
    SND_COUNT
} SoundId;

// inicia o áudio e carrega os sons
int sound_init(void);
// libera os sons e o áudio
void sound_shutdown(void);

// toca um efeito do início
void sound_play(SoundId id);

// liga ou desliga som em loop
void sound_loop(SoundId id, int on);

#endif
