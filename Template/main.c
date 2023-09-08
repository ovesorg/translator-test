#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f10x_eval.h"

/* open the macro to use USART0 remapping */
#define USART0_REMAP


void gpioInit(void);
void UartConfig(uint32_t uart_no,uint32_t bandrate);
// void usart_config(void);
// void Uart1Init(void);
// void Uart2Init(void);
// void Uart3Init(void);
// void Uart4Init(void);
void Uart5Init(void);
void nvic_config(void);


int main(void)
{
    Uart5Init();
    nvic_config();
    systick_config();
    TimerInit();

    printf("system run!\n");


    // /* configure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    // fwdgt_config(2 * 500, FWDGT_PSC_DIV64);
    // /* after 1.6 seconds to generate a reset */
    // fwdgt_enable();

    // /* check if the system has resumed from FWDGT reset */
    // if(RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)) 
    // {
    //     printf("\r\nsystem resumed from FWDGT reset!\r\n");
    //     /* clear the FWDGT reset flag */
    //     rcu_all_reset_flag_clear();
    // }
    

    //translater_task_init();
    context.timeout_count = 0;
    config.tmo_threshold = 3; 
    demo_fsm_create(&config, &context);
    demo_fsm_engine(translator_start_init_e, &config, &context);
    
    // demo_fsm_engine(translator_systick_e, &config, &context);
    // demo_fsm_engine(translator_transmit_e, &config, &context);
    // demo_fsm_engine(translator_systick_e, &config, &context);
    // demo_fsm_engine(translator_transmit_e, &config, &context);
    // demo_fsm_show_history(&config, &context);  

    while(1)
    {
        demo_fsm_engine(translator_systick_e, &config, &context);
        delay_1ms(1000);
        demo_fsm_engine(translator_transmit_e, &config, &context);
        //fwdgt_counter_reload();

        // demo_fsm_engine(translator_transmit_e, &config, &context);
        // delay_1ms(1000);
        // demo_fsm_engine(translator_systick_e, &config, &context);
        

    }
 

    // translater_task_init();
    // operateAllDevices(&deviceList_translator);

    while(1)
    {
        // if(g_TimerFlag[TIMER_BMS_COM] == TRUE)
        // {
        //     operateAllDevices(&deviceList_translator);
        //     g_TimerFlag[TIMER_BMS_COM] = FALSE;
        // }
        if(g_TimerFlag[TIMER_EVENT_SYSTICK] == TRUE)
        {
            printf("demo_fsm_engine(translator_systick_e, &config, &context)\r\n");
            demo_fsm_engine(translator_systick_e, &config, &context);
            g_TimerFlag[TIMER_EVENT_SYSTICK] = FALSE;
        }
        if(g_TimerFlag[TIMER_EVENT_TRANSMIT] == TRUE)
        {
            printf("demo_fsm_engine(translator_transmit_e, &config, &context)\r\n");
            demo_fsm_engine(translator_transmit_e, &config, &context);
            g_TimerFlag[TIMER_EVENT_TRANSMIT] = FALSE;
        }
        if(g_TimerFlag[TIMER_FSM_HISTORY] == TRUE)
        {
            printf("demo_fsm_show_history(&config, &context)\r\n");
            demo_fsm_show_history(&config, &context);  
            g_TimerFlag[TIMER_FSM_HISTORY] = FALSE;
        }
    }


}


void gpioInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
}

void UartConfig(uint32_t uart_no,uint32_t bandrate)
{
	/* USART configure */
	usart_deinit(uart_no);
	usart_baudrate_set(uart_no, bandrate);
	usart_word_length_set(uart_no, USART_WL_8BIT);
	usart_stop_bit_set(uart_no, USART_STB_1BIT);
	usart_parity_config(uart_no, USART_PM_NONE);
	usart_hardware_flow_rts_config(uart_no, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(uart_no, USART_CTS_DISABLE);
	usart_receive_config(uart_no, USART_RECEIVE_ENABLE);
	usart_transmit_config(uart_no, USART_TRANSMIT_ENABLE);
	usart_enable(uart_no);

	/* enable USART TBE interrupt */  
	usart_interrupt_enable(uart_no, USART_INT_FLAG_RBNE);

}

void Uart5Init(void) //EXT UART
{
	/* USART interrupt configuration */
    nvic_irq_enable(UART4_IRQn, 0, 0);
    /* configure COM0 */
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART4);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    UartConfig(UART4,38400U);

}

void nvic_config(void)
{
    /* configure CAN0 NVIC */
    nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn,1,1);
    nvic_irq_enable(CAN0_RX1_IRQn,1,1);
}


void Uart5Send(uint8_t *buffer,uint16_t size)
{
	uint16_t i;
	
	for(i=0;i<size;i++)
	{
		 usart_data_transmit(UART4, buffer[i]);
   		 while(RESET == usart_flag_get(UART4, USART_FLAG_TBE));
		}
}


/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(UART4, (uint8_t)ch);
    while(RESET == usart_flag_get(UART4, USART_FLAG_TBE));

    return ch;
}

