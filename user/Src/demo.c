/*----------------------------------------------------------------------
 * demo.c 
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

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>

// #include "fsm.h"
// #include "demo_context.h"

// #include "demo_session_fsm.h"
#include "main.h"


// static demo_config_t config;  

// static demo_context_t context;  

demo_config_t config;  

demo_context_t context; 


void demo_main (void)
{
    context.timeout_count = 0;
    config.tmo_threshold = 3; 
    demo_fsm_create(&config, &context);  
 

    demo_fsm_show_history(&config, &context);  

    demo_fsm_show_state_table(&config, &context);

    demo_fsm_destroy(&config, &context);
}

// void demo_main (void)
// {
   
//     fsm_t *fsm;
//     RC_FSM_t rc;
    
//     /* 创建状态机 */
//     demo_fsm_create(&config, &context);
//     context.timeout_count = 0;
//     config.tmo_threshold = 3; 

//     fsm_engine(fsm, start_init_e, &config, &context);
//     fsm_engine(fsm, init_tmo_e, &config, &context);
//     fsm_engine(fsm, init_tmo_e, &config, &context);
//     fsm_engine(fsm, init_ack_e, &config, &context);
//     fsm_engine(fsm, start_term_e, &config, &context);
//     fsm_engine(fsm, term_ack_e, &config, &context);
    
//     fsm_show_history(fsm);
//     fsm_display_table(fsm);
//     fsm_destroy(&fsm);

// }


