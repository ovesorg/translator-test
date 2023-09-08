/*!
    \file    main.h
    \brief   the header file of main

    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef MAIN_H
#define MAIN_H

#include "gd32f10x.h"
#include "gd32f10x_it.h"
#include "stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "string.h"
#include "systick.h"
#include "translater.h"
#include "can_obj.h"
#include "CRC16.h"
#include <stddef.h>
#include "fsm.h"
#include "demo_context.h"
#include "demo_event_handlers.h"
#include "demo_session_fsm.h"
#include "runtime_environment.h"
#include "demo.h"
#include "Timer.h"

extern FlagStatus g_can0_receive_flag;

extern can_receive_message_struct g_receive_message;
extern can_trasnmit_message_struct g_transmit_message;

extern thread_safe_queue transmit_buffer;



extern uint8_t can_test_Data[];

extern DeviceList deviceList_translator;

extern uint8_t g_TimerFlag[];

extern state_tuple_t  demo_state_table[];

extern demo_config_t config;  
extern demo_context_t context; 
//extern fsm_t *fsm_handle;

#endif /* MAIN_H */
