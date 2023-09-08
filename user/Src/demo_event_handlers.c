/*----------------------------------------------------------------------
 * demo_event_handlers.c
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
// #include <inttypes.h>

// #include "fsm.h"
// #include "demo_context.h"
// #include "demo_session_fsm.h"

#include "main.h"



RC_FSM_t event_translator_start_init (void *p2event, void *p2parm)
{
    demo_config_t *p2config;
    demo_context_t *p2context;
    RC_FSM_t rc;

    p2config = p2event; 
    p2context = p2parm; 

    translater_task_init();
    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);

    rc = RC_FSM_OK; 
    return (rc);
}

RC_FSM_t event_translator_receive(void *p2event, void *p2parm)
{
    demo_config_t *p2config;
    demo_context_t *p2context;
    RC_FSM_t rc;

    p2config = p2event; 
    p2context = p2parm; 

    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);
    

    rc = RC_FSM_OK; 
    return (rc);
}

RC_FSM_t event_translator_systick(void *p2event, void *p2parm)
{
    demo_config_t *p2config;
    demo_context_t *p2context;
    RC_FSM_t rc;

    p2config = p2event; 
    p2context = p2parm; 

    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);

    rc = RC_FSM_OK; 
    return (rc);
}

RC_FSM_t event_translator_transmit(void *p2event, void *p2parm)
{
    demo_config_t *p2config;
    demo_context_t *p2context;
    RC_FSM_t rc;

    p2config = p2event; 
    p2context = p2parm; 

    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);
    operateAllDevices(&deviceList_translator);

    rc = RC_FSM_OK; 
    return (rc);
}

RC_FSM_t event_translator_start_sleep(void *p2event, void *p2parm)
{
    demo_config_t *p2config;
    demo_context_t *p2context;
    RC_FSM_t rc;

    p2config = p2event; 
    p2context = p2parm; 

    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);

    rc = RC_FSM_OK; 
    return (rc);
}

RC_FSM_t event_translator_wakeup(void *p2event, void *p2parm)
{
    demo_config_t *p2config;
    demo_context_t *p2context;
    RC_FSM_t rc;

    p2config = p2event; 
    p2context = p2parm; 

    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);

    rc = RC_FSM_OK; 
    return (rc);
}
