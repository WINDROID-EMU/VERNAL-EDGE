#ifndef RUNNER_AUDIO_H
#define RUNNER_AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS    2
#define MAX_AUDIO_VOICES  64

/* Lifecycle */
bool audio_init(void);
void audio_shutdown(void);

/* Mixer callbacks for audio device / backend */
void audio_mix_stereo_f32(float *buffer, size_t frames);
void audio_mix_stereo_s16(int16_t *buffer, size_t frames);

/* GameMaker Audio Builtin Functions */
int32_t audio_play_sound(int32_t sound_idx, int priority, bool loop);
int32_t audio_play_sound_ext(int32_t sound_idx, int priority, bool loop, float gain, float offset, float pitch);
void audio_stop_sound(int32_t sound_or_voice);
void audio_stop_all(void);
void audio_sound_gain(int32_t sound_or_voice, float volume, int time_ms);
void audio_sound_pitch(int32_t sound_or_voice, float pitch);
bool audio_is_playing(int32_t sound_or_voice);
const char *audio_get_name(int32_t sound_idx);
float audio_sound_length(int32_t sound_idx);
void audio_sound_set_track_position(int32_t sound_or_voice, float pos_sec);
void audio_emitter_free(int32_t emitter_id);
bool audio_group_is_loaded(int32_t group_id);

#ifdef __cplusplus
}
#endif

#endif /* RUNNER_AUDIO_H */
