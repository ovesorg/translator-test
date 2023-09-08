#include "main.h"


TIMER_TypeDef g_BmsCom_timer;
TIMER_TypeDef g_EventSystick_timer;
TIMER_TypeDef g_EventTransmit_timer;
TIMER_TypeDef g_FSMHistory_timer;


__IO uint32_t uwTick;

const TIMER_LIST_TypeDef g_Timer[]=
{
	{TIMER_BMS_COM,COUNT_SUB,&g_BmsCom_timer,TimerBmsComCallback},
	{TIMER_EVENT_SYSTICK,COUNT_SUB,&g_EventSystick_timer,TimerEventSystickCallback},
	{TIMER_EVENT_TRANSMIT,COUNT_SUB,&g_EventTransmit_timer,TimerEventTransmitCallback},
	{TIMER_FSM_HISTORY,COUNT_SUB,&g_FSMHistory_timer,TimerEventFsmHistoryCallback},
};

uint8_t g_TimerCount=sizeof(g_Timer)/sizeof(TIMER_LIST_TypeDef);
uint8_t g_TimerFlag[TIMER_COUNT];


uint32_t HAL_GetTick(void)
{
  return uwTick;
}


void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a period to guaranty minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += 1;
  }

  while((HAL_GetTick() - tickstart) < wait)
  {
  }
}


void HAL_SYSTICK_Callback(void)
{

	uint8_t i;

	uwTick++;
	//printf("%s-%d\r\n",__FUNCTION__,__LINE__);
	for(i=0;i<sizeof(g_Timer)/sizeof(TIMER_LIST_TypeDef);i++)
	{
		//printf("Timer=%u enable=%u count=%u cnt_type=%u\r\n",g_Timer[i].timer_id,g_Timer[i].p->enable,g_Timer[i].p->count,g_Timer[i].cnt_type);

		if(g_Timer[i].p->count&&g_Timer[i].p->enable&&g_Timer[i].cnt_type==COUNT_SUB)
		{
			g_Timer[i].p->count--;
			//printf("Timer%d count=%d\r\n",g_Timer[i].timer_id,g_Timer[i].p->count);

			if(g_Timer[i].p->count==0)
			{
				g_Timer[i].p->event=TRUE;
				//printf("Timer%d event\r\n",g_Timer[i].timer_id);

				if(g_Timer[i].p->period==FALSE)
				{
					g_Timer[i].p->enable=FALSE;
					//printf("Timer%d disable\r\n",g_Timer[i].timer_id);
				}				
				if(g_Timer[i].callback!=NULL)
				{
					g_Timer[i].callback(g_Timer[i].p);
					//printf("Timer%d callback\r\n",g_Timer[i].timer_id);
				}
				else
				{
					//printf("Timer%d callback is NULL\r\n",g_Timer[i].timer_id);
				}	 
			}
		}
	}
}

void TimerInit(void)
{

	g_BmsCom_timer.enable=FALSE;
	g_BmsCom_timer.period=TRUE;
	g_BmsCom_timer.count=1000u;
	g_BmsCom_timer.event=FALSE;

	g_EventSystick_timer.enable=FALSE;
	g_EventSystick_timer.period=TRUE;
	g_EventSystick_timer.count=2000u;
	g_EventSystick_timer.event=FALSE;

	g_EventTransmit_timer.enable=FALSE;
	g_EventTransmit_timer.period=TRUE;
	g_EventTransmit_timer.count=2000u;
	g_EventTransmit_timer.event=FALSE;

	g_FSMHistory_timer.enable=TRUE;
	g_FSMHistory_timer.period=TRUE; 
	g_FSMHistory_timer.count=10000u;
	g_FSMHistory_timer.event=FALSE;
}

void TimerBmsComCallback(TIMER_TypeDef *p)
{
	p->count=1000;
	printf("TimerBmsComCallback\r\n");
	g_TimerFlag[TIMER_BMS_COM] = TRUE;
	//operateAllDevices(&deviceList_translator);
	//delay_1ms(100);
}

void TimerEventSystickCallback(TIMER_TypeDef *p)
{
	p->count=2000;
	printf("TimerEventSystickCallback\r\n");
	g_TimerFlag[TIMER_EVENT_SYSTICK] = TRUE;
	//demo_fsm_engine(translator_systick_e, &config, &context);
}

void TimerEventTransmitCallback(TIMER_TypeDef *p)
{
	p->count=2000;
	printf("TimerEventTransmitCallback\r\n");
	g_TimerFlag[TIMER_EVENT_SYSTICK] = TRUE;
	//demo_fsm_engine(translator_transmit_e, &config, &context);
}

void TimerEventFsmHistoryCallback(TIMER_TypeDef *p)
{
	p->count=10000;
	printf("TimerEventFsmHistoryCallback\r\n");
	g_TimerFlag[TIMER_EVENT_SYSTICK] = TRUE;
    demo_fsm_show_history(&config, &context);  
}

void  TimerSet(uint8_t timer,uint32_t count)
{
	uint8_t i=0;
	
    for(i=0;i<TIMER_COUNT;i++)
	{	
		if(g_Timer[i].timer_id==timer)
		{
			g_Timer[i].p->enable=TRUE;
			g_Timer[i].p->count=count;
			g_Timer[i].p->event=FALSE;

			break;
		}
    }

}

void  TimerEventClear(uint8_t timer)
{
	uint8_t i=0;
	
    for(i=0;i<TIMER_COUNT;i++)
	{	
		if(g_Timer[i].timer_id==timer)
		{
			g_Timer[i].p->event=FALSE;
			break;
			}
        }
}

uint8_t  TimerGetEventState(uint8_t timer)
{
	uint8_t i=0;
	
    for(i=0;i<TIMER_COUNT;i++)
	{	
		if(g_Timer[i].timer_id==timer)
		{
			return g_Timer[i].p->event;
			}
        }

	return FALSE;
}






