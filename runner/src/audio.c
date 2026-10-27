#include "audio.h"
#include "data_win.h"
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Include stb_vorbis implementation */
#include "stb_vorbis.c"

typedef struct {
    int32_t voice_id;
    int32_t sound_idx;
    bool active;
    bool loop;
    float volume;
    float pitch;
    int priority;

    /* In-memory PCM (WAV) */
    bool is_stream;
    const int16_t *pcm_samples;
    size_t total_frames;
    double frame_cursor;
    int src_channels;
    int sample_rate;

    /* Streaming OGG */
    stb_vorbis *stream;
} AudioVoice;

static AudioVoice s_voices[MAX_AUDIO_VOICES] = {0};
static int32_t s_next_voice_id = 1;

bool audio_init(void) {
    memset(s_voices, 0, sizeof(s_voices));
    s_next_voice_id = 1;
    printf("[Audio] Subsystem initialized (Rate: %d Hz, Stereo, Max voices: %d)\n",
           AUDIO_SAMPLE_RATE, MAX_AUDIO_VOICES);
    return true;
}

void audio_shutdown(void) {
    audio_stop_all();
    printf("[Audio] Subsystem shut down\n");
}

static AudioVoice *alloc_voice(int priority) {
    /* 1. Find inactive slot */
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        if (!s_voices[i].active) {
            return &s_voices[i];
        }
    }

    /* 2. Steal lowest priority active voice */
    int lowest_idx = -1;
    int lowest_prio = priority;
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        if (s_voices[i].priority < lowest_prio) {
            lowest_prio = s_voices[i].priority;
            lowest_idx = i;
        }
    }

    if (lowest_idx >= 0) {
        AudioVoice *v = &s_voices[lowest_idx];
        if (v->stream) {
            stb_vorbis_close(v->stream);
            v->stream = NULL;
        }
        memset(v, 0, sizeof(AudioVoice));
        return v;
    }

    return NULL;
}

int32_t audio_play_sound_ext(int32_t sound_idx, int priority, bool loop, float gain, float offset, float pitch) {
    if (sound_idx < 0 || (uint32_t)sound_idx >= g_DataWin.sound_count) return -1;
    SoundDef *snd = data_win_get_sound(&g_DataWin, sound_idx);
    if (!snd) return -1;

    AudioVoice *v = alloc_voice(priority);
    if (!v) return -1;

    v->voice_id = s_next_voice_id++;
    if (s_next_voice_id < 1) s_next_voice_id = 1;

    v->sound_idx = sound_idx;
    v->priority = priority;
    v->loop = loop;
    v->volume = gain >= 0.0f ? gain : (snd->volume > 0.0f ? snd->volume : 1.0f);
    v->pitch = pitch > 0.0f ? pitch : (snd->pitch > 0.0f ? snd->pitch : 1.0f);

    if (snd->is_streamed) {
        /* Open streaming OGG file */
        int err = 0;
        v->stream = stb_vorbis_open_filename(snd->file_name, &err, NULL);
        if (!v->stream && snd->file_name[0] != '/') {
            char path[512];
            const char *cand_dirs[] = {
                files_get_asset_directory(),
                files_get_save_directory(),
                "/sdcard/VernalEdge/",
                "/storage/emulated/0/VernalEdge/",
                "/sdcard/Android/data/com.vernaledge/files/",
                "/storage/emulated/0/Android/data/com.vernaledge/files/",
                "./"
            };
            for (size_t c = 0; c < sizeof(cand_dirs)/sizeof(cand_dirs[0]); c++) {
                const char *cdir = cand_dirs[c];
                if (!cdir || !cdir[0]) continue;
                snprintf(path, sizeof(path), "%s%s", cdir, snd->file_name);
                v->stream = stb_vorbis_open_filename(path, &err, NULL);
                if (v->stream) {
                    fprintf(stdout, "[Audio] Loaded OGG stream from %s\n", path);
                    break;
                }
            }
        }

        if (v->stream) {
            v->is_stream = true;
            v->total_frames = (size_t)stb_vorbis_stream_length_in_samples(v->stream);
            if (offset > 0.0f) {
                unsigned int sample_pos = (unsigned int)(offset * (float)AUDIO_SAMPLE_RATE);
                stb_vorbis_seek(v->stream, sample_pos);
            }
            v->active = true;
            return v->voice_id;
        } else {
            fprintf(stderr, "[Audio] Failed to open OGG stream: %s (err %d)\n", snd->file_name, err);
            memset(v, 0, sizeof(AudioVoice));
            return -1;
        }
    } else {
        /* Load in-memory PCM from data.win AUDO chunk */
        size_t pcm_len = 0;
        int channels = 2;
        int sample_rate = 44100;
        const uint8_t *pcm = data_win_get_sound_pcm(&g_DataWin, sound_idx, &pcm_len, &channels, &sample_rate);

        if (pcm && pcm_len > 0) {
            v->is_stream = false;
            v->pcm_samples = (const int16_t *)pcm;
            v->src_channels = channels;
            v->sample_rate = sample_rate;
            v->total_frames = pcm_len / (channels * sizeof(int16_t));
            v->frame_cursor = (double)(offset * (float)sample_rate);
            v->active = true;
            return v->voice_id;
        } else {
            fprintf(stderr, "[Audio] Failed to read PCM for sound %d (%s)\n", sound_idx, snd->name);
            memset(v, 0, sizeof(AudioVoice));
            return -1;
        }
    }
}

int32_t audio_play_sound(int32_t sound_idx, int priority, bool loop) {
    return audio_play_sound_ext(sound_idx, priority, loop, -1.0f, 0.0f, -1.0f);
}

void audio_stop_sound(int32_t sound_or_voice) {
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        AudioVoice *v = &s_voices[i];
        if (v->active && (v->voice_id == sound_or_voice || v->sound_idx == sound_or_voice)) {
            if (v->stream) {
                stb_vorbis_close(v->stream);
                v->stream = NULL;
            }
            v->active = false;
        }
    }
}

void audio_stop_all(void) {
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        AudioVoice *v = &s_voices[i];
        if (v->active) {
            if (v->stream) {
                stb_vorbis_close(v->stream);
                v->stream = NULL;
            }
            v->active = false;
        }
    }
}

void audio_sound_gain(int32_t sound_or_voice, float volume, int time_ms) {
    (void)time_ms;
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        AudioVoice *v = &s_voices[i];
        if (v->active && (v->voice_id == sound_or_voice || v->sound_idx == sound_or_voice)) {
            v->volume = volume;
        }
    }
}

void audio_sound_pitch(int32_t sound_or_voice, float pitch) {
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        AudioVoice *v = &s_voices[i];
        if (v->active && (v->voice_id == sound_or_voice || v->sound_idx == sound_or_voice)) {
            v->pitch = pitch > 0.0f ? pitch : 1.0f;
        }
    }
}

bool audio_is_playing(int32_t sound_or_voice) {
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        AudioVoice *v = &s_voices[i];
        if (v->active && (v->voice_id == sound_or_voice || v->sound_idx == sound_or_voice)) {
            return true;
        }
    }
    return false;
}

const char *audio_get_name(int32_t sound_idx) {
    SoundDef *snd = data_win_get_sound(&g_DataWin, sound_idx);
    return snd && snd->name ? snd->name : "";
}

float audio_sound_length(int32_t sound_idx) {
    SoundDef *snd = data_win_get_sound(&g_DataWin, sound_idx);
    if (!snd) return 0.0f;

    if (snd->is_streamed) {
        int err = 0;
        stb_vorbis *v = stb_vorbis_open_filename(snd->file_name, &err, NULL);
        if (v) {
            unsigned int samples = stb_vorbis_stream_length_in_samples(v);
            stb_vorbis_info info = stb_vorbis_get_info(v);
            stb_vorbis_close(v);
            return (float)samples / (float)info.sample_rate;
        }
    } else {
        size_t pcm_len = 0;
        int channels = 2, sample_rate = 44100;
        const uint8_t *pcm = data_win_get_sound_pcm(&g_DataWin, sound_idx, &pcm_len, &channels, &sample_rate);
        if (pcm && channels > 0 && sample_rate > 0) {
            size_t frames = pcm_len / (channels * sizeof(int16_t));
            return (float)frames / (float)sample_rate;
        }
    }
    return 0.0f;
}

void audio_sound_set_track_position(int32_t sound_or_voice, float pos_sec) {
    for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
        AudioVoice *v = &s_voices[i];
        if (v->active && (v->voice_id == sound_or_voice || v->sound_idx == sound_or_voice)) {
            if (v->is_stream && v->stream) {
                unsigned int sample_pos = (unsigned int)(pos_sec * (float)AUDIO_SAMPLE_RATE);
                stb_vorbis_seek(v->stream, sample_pos);
            } else {
                v->frame_cursor = (double)(pos_sec * (float)v->sample_rate);
            }
        }
    }
}

void audio_emitter_free(int32_t emitter_id) {
    (void)emitter_id;
}

bool audio_group_is_loaded(int32_t group_id) {
    (void)group_id;
    return true;
}

/* ================= Realtime Audio Mixer ================= */

void audio_mix_stereo_f32(float *buffer, size_t frames) {
    if (!buffer || frames == 0) return;
    memset(buffer, 0, frames * 2 * sizeof(float));

    short scratch_ogg[1024 * 2];

    for (int v_idx = 0; v_idx < MAX_AUDIO_VOICES; v_idx++) {
        AudioVoice *v = &s_voices[v_idx];
        if (!v->active) continue;

        float vol = v->volume;
        float pitch = v->pitch;

        if (v->is_stream && v->stream) {
            size_t frames_left = frames;
            size_t out_offset = 0;

            while (frames_left > 0 && v->active) {
                int to_read = frames_left > 1024 ? 1024 : (int)frames_left;
                int read = stb_vorbis_get_samples_short_interleaved(v->stream, 2, scratch_ogg, to_read * 2);

                if (read > 0) {
                    for (int f = 0; f < read; f++) {
                        float sl = (float)scratch_ogg[f * 2 + 0] / 32768.0f * vol;
                        float sr = (float)scratch_ogg[f * 2 + 1] / 32768.0f * vol;
                        buffer[(out_offset + f) * 2 + 0] += sl;
                        buffer[(out_offset + f) * 2 + 1] += sr;
                    }
                    out_offset += read;
                    frames_left -= read;
                }

                if (read < to_read) {
                    /* End of OGG stream reached */
                    if (v->loop) {
                        stb_vorbis_seek_start(v->stream);
                    } else {
                        stb_vorbis_close(v->stream);
                        v->stream = NULL;
                        v->active = false;
                        break;
                    }
                }
            }
        } else if (!v->is_stream && v->pcm_samples) {
            const int16_t *src = v->pcm_samples;
            size_t total = v->total_frames;
            int ch = v->src_channels;

            for (size_t f = 0; f < frames; f++) {
                size_t frame_idx = (size_t)v->frame_cursor;
                if (frame_idx >= total) {
                    if (v->loop && total > 0) {
                        v->frame_cursor = fmod(v->frame_cursor, (double)total);
                        frame_idx = (size_t)v->frame_cursor;
                    } else {
                        v->active = false;
                        break;
                    }
                }

                float sl, sr;
                if (ch == 1) {
                    sl = sr = (float)src[frame_idx] / 32768.0f * vol;
                } else {
                    sl = (float)src[frame_idx * 2 + 0] / 32768.0f * vol;
                    sr = (float)src[frame_idx * 2 + 1] / 32768.0f * vol;
                }

                buffer[f * 2 + 0] += sl;
                buffer[f * 2 + 1] += sr;

                v->frame_cursor += (double)pitch;
            }
        }
    }

    /* Clamp buffer between -1.0f and +1.0f */
    for (size_t i = 0; i < frames * 2; i++) {
        if (buffer[i] > 1.0f) buffer[i] = 1.0f;
        else if (buffer[i] < -1.0f) buffer[i] = -1.0f;
    }
}

void audio_mix_stereo_s16(int16_t *buffer, size_t frames) {
    if (!buffer || frames == 0) return;

    float f32_buf[1024 * 2];
    size_t remaining = frames;
    size_t offset = 0;

    while (remaining > 0) {
        size_t chunk = remaining > 1024 ? 1024 : remaining;
        audio_mix_stereo_f32(f32_buf, chunk);

        for (size_t i = 0; i < chunk * 2; i++) {
            float s = f32_buf[i];
            int32_t val = (int32_t)(s * 32767.0f);
            if (val > 32767) val = 32767;
            else if (val < -32768) val = -32768;
            buffer[offset + i] = (int16_t)val;
        }

        offset += chunk * 2;
        remaining -= chunk;
    }
}
