#ifndef RUNNER_ROOM_H
#define RUNNER_ROOM_H

#include "instance.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*RoomCreateFunc)(void);

typedef struct RoomDef {
    int32_t id;
    const char* name;
    int32_t width;
    int32_t height;
    double speed;
    bool persistent;
    RoomCreateFunc create_code;
} RoomDef;

// Room API
void RoomManager_Init(void);
void RoomManager_Register(int32_t room_id, const char* name, int32_t width, int32_t height, RoomCreateFunc create_code);
void room_goto(int32_t room_id);
void room_restart(void);
int32_t room_get_current(void);
int32_t room_get_width(void);
int32_t room_get_height(void);
double room_get_speed(void);

// Frame Step lifecycle for active room
void RoomManager_Step(void);
void RoomManager_Draw(void);
bool RoomManager_IsTitleRoom(void);

#ifdef __cplusplus
}
#endif

#endif // RUNNER_ROOM_H
