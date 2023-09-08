/*----------------------------------------------------------------------
 * demo_session_fsm.h
 *
 * January 2009, Bo Berry
 *
 * Copyright (c) 2009 Cisco Systems, Inc 
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *---------------------------------------------------------------------
 */

#ifndef  __DEMO_SESSION_FSM_H__
#define  __DEMO_SESSION_FSM_H__

#include "demo_context.h"


/*
 * normalized events
 */
typedef enum {
  
    translator_start_init_e,
    translator_receive_e,
    translator_systick_e,
    translator_transmit_e,
    translator_start_sleep_e,
    translator_wakeup_e,

} session_events_e;


/*
 * normalized states
 */
typedef enum 
{
    translator_idle_s,
    translator_ready_s,
    translator_running_s,
    translator_sleeping_s,
} demo_states_e;




extern uint32_t
demo_fsm_get_state(demo_config_t *p2config,
                   demo_context_t *p2context); 

extern void 
demo_fsm_show_counter(demo_config_t *p2config,
                      demo_context_t *p2context); 

extern void 
demo_fsm_show_state_table(demo_config_t *p2config,
                          demo_context_t *p2context); 

extern void 
demo_fsm_show_history(demo_config_t *p2config,
                      demo_context_t *p2context); 

extern void 
demo_fsm_engine(uint32_t normalized_event,
                demo_config_t *p2config,
                demo_context_t *p2context);

extern void
demo_fsm_destroy(demo_config_t *p2config,
                 demo_context_t *p2context); 

extern void
demo_fsm_create(demo_config_t *p2config,
                demo_context_t *p2context); 


#endif

