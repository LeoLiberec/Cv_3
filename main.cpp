#include "mbed.h"
#include "rtos.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <list>

TS_StateTypeDef TS_State;
Semaphore sem(1);
int clickNum = 0;
volatile int repeats[4] = {1, 1, 1, 1};
volatile uint32_t colors [] = {LCD_COLOR_GREEN, LCD_COLOR_LIGHTBLUE, LCD_COLOR_LIGHTMAGENTA, LCD_COLOR_ORANGE};
Thread thread[4];

void threadTakesCharge(int* threadNum){
    while(1){
        if(repeats[(uint32_t)threadNum] > 0){
            sem.acquire();
            uint8_t text[30];
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);   
            BSP_LCD_Clear(colors[(uint32_t)threadNum]);
            BSP_LCD_SetBackColor(colors[(uint32_t)threadNum]);
            sprintf((char*)text, "Vlakno cislo: %d", threadNum);
            BSP_LCD_DisplayStringAt(0, LINE(4), (uint8_t *)&text, CENTER_MODE);
            ThisThread::sleep_for(2s);
            
            BSP_LCD_Clear(LCD_COLOR_CYAN);
            BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);
            BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
            sprintf((char*)text, "    ");
            BSP_LCD_DisplayStringAt(30, LINE(2), (uint8_t *)&text, CENTER_MODE);
            
            BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);
            BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
            sprintf((char*)text, " Nic se nedeje ");
            BSP_LCD_DisplayStringAt(15, LINE(3), (uint8_t *)&text, CENTER_MODE);
            sprintf((char*)text, " Zedne vlakno nebezi ");
            BSP_LCD_DisplayStringAt(0, LINE(4), (uint8_t *)&text, CENTER_MODE);
            
            BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);  
            BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
            sprintf((char*)text, "                ");
            BSP_LCD_DisplayStringAt(-25, LINE(5), (uint8_t *)&text, CENTER_MODE);
            sprintf((char*)text, "    ");
            BSP_LCD_DisplayStringAt(-12, LINE(6), (uint8_t *)&text, CENTER_MODE);   
            repeats[(uint32_t)threadNum]--;
            clickNum = 0;
            sem.release(); 
        }       
    }   
}

bool touch(){
    BSP_TS_GetState(&TS_State);
    if(TS_State.touchDetected) {
        return true;
    }
    return false;   
}

int main() {   
    uint16_t text[23]; 
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    uint16_t status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    HAL_Delay(1000);
    
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_Clear(LCD_COLOR_CYAN);
    BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
    
    sprintf((char*)text, " Zadne vlakno nebezi :( ");
    BSP_LCD_DisplayStringAt(0, LINE(3), (uint8_t *)&text, CENTER_MODE);
    
    while(1){
        bool touched = touch();
        if(touched && clickNum < 4){
            thread[clickNum].start(callback(threadTakesCharge, (int*)clickNum));
            HAL_Delay(100);
            clickNum++;
        }  
        else if(touched && clickNum >= 4){
            repeats[clickNum % 4]++;
            HAL_Delay(100);
            clickNum++;
        }
    }
}


