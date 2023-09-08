
#ifndef __TIMER_H__
#define __TIMER_H__


#define SLEEP_PRIOD  (uint32_t)(2*60000u)
#define DEMO_PRIOD  (uint32_t)(15*60000u)

#define COUNT_ADD 0
#define COUNT_SUB 1

#define HAL_MAX_DELAY      0xFFFFFFFFU


enum
{
	TIMER_BMS_COM,
	TIMER_EVENT_SYSTICK,
	TIMER_EVENT_TRANSMIT,
	TIMER_FSM_HISTORY,
	TIMER_COUNT
};

typedef struct
{
    uint8_t enable:1;
    uint8_t period:1;	
    uint8_t holdlock:1;
    uint8_t event:5;	
	
    uint8_t retry;	

    uint16_t reserved;
    uint32_t count;
}TIMER_TypeDef;

typedef struct
{
    uint16_t timer_id;
    uint16_t cnt_type;	
    TIMER_TypeDef *p;
    void  (*callback)(TIMER_TypeDef *p);
	
}TIMER_LIST_TypeDef;

void TimerInit(void);

void TimerBmsComCallback(TIMER_TypeDef *p);
void TimerEventSystickCallback(TIMER_TypeDef *p);
void TimerEventTransmitCallback(TIMER_TypeDef *p);
void TimerEventFsmHistoryCallback(TIMER_TypeDef *p);

void  TimerEventClear(uint8_t timer);
uint8_t  TimerGetEventState(uint8_t timer);
void  TimerSet(uint8_t timer,uint32_t count);
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t Delay);
void HAL_SYSTICK_Callback(void);


#endif

