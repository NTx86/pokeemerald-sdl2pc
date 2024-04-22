#ifndef GUARD_PLATFORM_H
#define GUARD_PLATFORM_H

struct SiiRtcInfo;

u16 Platform_GetKeyInput(void);
void Platform_StoreSaveFile(void);
void Platform_GetStatus(struct SiiRtcInfo *rtc);
void Platform_SetStatus(struct SiiRtcInfo *rtc);
void Platform_GetDateTime(struct SiiRtcInfo *rtc);
void Platform_SetDateTime(struct SiiRtcInfo *rtc);
void Platform_GetTime(struct SiiRtcInfo *rtc);
void Platform_SetTime(struct SiiRtcInfo *rtc);
void Platform_SetAlarm(u8 *alarmData);
int getScreenBase(int bg);
void setScreenBase(int bg, int charBase);

#endif  // GUARD_PLATFORM_H