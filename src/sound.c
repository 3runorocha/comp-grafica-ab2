#include "sound.h"
#include <stdio.h>

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#include <miniaudio.h>

// arquivo e volume de cada som
static const struct { const char *file; float volume; } table[SND_COUNT] = {
    [SND_MUSIC]  = { "assets/sounds/bgm_0e.wav",   0.315f },
    [SND_STEP_L] = { "assets/sounds/ft_conca.wav", 0.60f },
    [SND_STEP_R] = { "assets/sounds/ft_concb.wav", 0.60f },
    [SND_GLOBE]  = { "assets/sounds/crank.wav",    0.90f },
    [SND_DRAWER] = { "assets/sounds/drw_c_op.wav", 0.90f },
    [SND_FIRE]   = { "assets/sounds/se_06.wav",    0.35f },
    [SND_SWITCH] = { "assets/sounds/sw_btn.wav",   0.90f },
    [SND_CHAIR]  = { "assets/sounds/slback_c.wav", 0.90f },
};

static ma_engine engine;
static ma_sound sounds[SND_COUNT];
static int loaded[SND_COUNT];
static int engine_ok;

// inicia o áudio e carrega os sons
int sound_init(void)
{
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        fprintf(stderr, "Aviso: sem dispositivo de audio, jogo sem som\n");
        return 0;
    }
    engine_ok = 1;
    // carrega cada som
    int count = 0;
    for (int i = 0; i < SND_COUNT; i++) {
        if (ma_sound_init_from_file(&engine, table[i].file, MA_SOUND_FLAG_DECODE, NULL, NULL, &sounds[i]) == MA_SUCCESS) {
            ma_sound_set_volume(&sounds[i], table[i].volume);
            loaded[i] = 1;
            count++;
        } else {
            fprintf(stderr, "Aviso: som '%s' nao encontrado (veja assets/sounds/README.md)\n", table[i].file);
        }
    }
    printf("%d sons carregados\n", count);
    return 1;
}

// libera os sons e o áudio
void sound_shutdown(void)
{
    if (!engine_ok)
        return;
    for (int i = 0; i < SND_COUNT; i++)
        if (loaded[i])
            ma_sound_uninit(&sounds[i]);
    ma_engine_uninit(&engine);
    engine_ok = 0;
}

// toca um efeito do início
void sound_play(SoundId id)
{
    if (!loaded[id])
        return;
    ma_sound_seek_to_pcm_frame(&sounds[id], 0);
    ma_sound_start(&sounds[id]);
}

// liga ou desliga som em loop
void sound_loop(SoundId id, int on)
{
    if (!loaded[id])
        return;
    if (on) {
        if (ma_sound_is_playing(&sounds[id]))
            return;
        ma_sound_set_looping(&sounds[id], MA_TRUE);
        ma_sound_seek_to_pcm_frame(&sounds[id], 0);
        ma_sound_start(&sounds[id]);
    } else {
        ma_sound_stop(&sounds[id]);
    }
}
