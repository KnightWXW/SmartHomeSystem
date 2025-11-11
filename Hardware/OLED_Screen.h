#ifndef _OLED_SCREEN_H_
#define _OLED_SCREEN_H_

typedef enum
{
    SCREEN_MAIN,
    SCREEN_SET_TEMP,
    SCREEN_SET_HUM,
};

void OLED_ShowScreenOfMain(void);
void OLED_ShowScreenOfSetTemp(void);
void OLED_ShowScreenOfSetHum(void);
void OLED_ShowScreen(uint8_t num);

#endif
