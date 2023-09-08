/*----------------------------------------------------------------------
 * demo_session_fsm.c
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

// #include "safe_limits.h"
// #include "fsm.h"
// #include "demo_context.h"
// #include "demo_event_handlers.h"

// #include "demo_session_fsm.h"

#include "main.h"



/*
 * table of normalized event descriptions 
 */
static event_description_t normalized_event_table[] =
  {
   {translator_start_init_e, "Translator Start Session Init"},
   {translator_receive_e, "Translator Receive Session Init"},
   {translator_systick_e, "Translator Systick"},
   {translator_transmit_e, "Translator Transmit Session Init"},
   {translator_start_sleep_e, "Translator Start Sleep"},
   {translator_wakeup_e, "Translator Wakeup"},
   {FSM_NULL_EVENT_ID, NULL} };


/*
 * table of normalized state descriptions
 */
static state_description_t normalized_state_table[] =
  {
   {translator_idle_s, "Translator Idle State"},
   {translator_ready_s, "Translator Ready State"},
   {translator_running_s, "Translator Running State"},
   {translator_sleeping_s, "Translator Sleeping State"},
   {FSM_NULL_STATE_ID, NULL} };


/* state machine handle */ 

static fsm_t *fsm_handle;
//fsm_t *fsm_handle;


/*
 *  catch-all ignore event processing
 */
static RC_FSM_t event_ignore (void *p2event, void *p2parm)
{
    DEMO_TRACE("%s-%u\n", __FUNCTION__, __LINE__);
    return (RC_FSM_OK);
}



/*
 * State event tables
 */

static event_tuple_t  state_translator_idle_events[] =
    {  
       { translator_start_init_e, event_translator_start_init,     translator_ready_s},
       { translator_receive_e, event_ignore,        translator_idle_s},
       { translator_systick_e, event_ignore,        translator_idle_s},
       { translator_transmit_e, event_ignore,       translator_idle_s},
       { translator_start_sleep_e, event_ignore,    translator_idle_s},
       { translator_wakeup_e, event_ignore,         translator_idle_s} };



static event_tuple_t  state_translator_ready_events[] =
    {  
       { translator_start_init_e, event_ignore,     translator_ready_s},
       { translator_receive_e, event_translator_receive,        translator_ready_s},
       { translator_systick_e, event_translator_systick,        translator_running_s},
       { translator_transmit_e, event_ignore,       translator_ready_s},
       { translator_start_sleep_e, event_translator_start_sleep,    translator_sleeping_s},
       { translator_wakeup_e, event_ignore,         translator_ready_s} };

static event_tuple_t  state_translator_running_events[] =
    { 
       { translator_start_init_e, event_ignore,     translator_running_s},
       { translator_receive_e, event_translator_receive,        translator_running_s},
       { translator_systick_e, event_ignore,        translator_running_s},
       { translator_transmit_e, event_translator_transmit,       translator_ready_s},
       { translator_start_sleep_e, event_ignore,    translator_running_s},
       { translator_wakeup_e, event_ignore,         translator_running_s} };

static event_tuple_t  state_translator_sleeping_events[] =
    {  
       { translator_start_init_e, event_ignore,     translator_sleeping_s},
       { translator_receive_e, event_ignore,        translator_sleeping_s},
       { translator_systick_e, event_ignore,        translator_sleeping_s},
       { translator_transmit_e, event_ignore,       translator_sleeping_s},
       { translator_start_sleep_e, event_ignore,    translator_sleeping_s},
       { translator_wakeup_e, event_translator_wakeup,         translator_ready_s} };



/*
 * Session State table
 */
static state_tuple_t  demo_state_table[] =
/***   State ID  ******************   Event Table ***/ 
    {
        {translator_idle_s,             state_translator_idle_events},
        {translator_ready_s,            state_translator_ready_events},
        {translator_running_s,          state_translator_running_events},
        {translator_sleeping_s,         state_translator_sleeping_events},
      {FSM_NULL_STATE_ID, NULL} };  


/*
 * This function is used to return the current state 
 */
uint32_t
demo_fsm_get_state (demo_config_t *p2config,
                    demo_context_t *p2context)
{
    uint32_t state;

    state = (-1);

    if (p2context) {
        fsm_get_state(fsm_handle, &state); 
    } else {
        /* ignore */
    }
    return (state);
}



/*
 * This function is used to display the state machine sate table  
 */
void 
demo_fsm_show_state_table (demo_config_t *p2config, 
                           demo_context_t *p2context)
{
    if (p2context) {
        fsm_display_table(fsm_handle);
    } else {  
        /* ignore */
    } 
    return;
}


/*
 * This function is used to display a state machine's history    
 */
void 
demo_fsm_show_history (demo_config_t *p2config, 
                       demo_context_t *p2context)
{
    if (p2context) {
        fsm_show_history(fsm_handle);
    } else {  
        /* ignore */
    } 
    return;
}  


/*
 * demo state machine engine  
 */
void 
demo_fsm_engine (uint32_t normalized_event, 
                 demo_config_t *p2config, 
                 demo_context_t *p2context)
{
    RC_FSM_t  rc;

    if (!p2config) {
        DEMO_ERROR("%s-%u Error: Config is null \n",
                      __FUNCTION__,
                      __LINE__);
        return;
    } 

    if (!p2context) {
        DEMO_ERROR("%s-%u Error: Context is null \n",
                      __FUNCTION__,
                      __LINE__);
        return;
    } 

    rc = fsm_engine(fsm_handle, 
                    normalized_event, 
                    p2config, 
                    p2context);
    if (rc != RC_FSM_OK) {
        DEMO_ERROR("%s-%u Error: demo state machine rc=%u \n", 
                     __FUNCTION__,
                     __LINE__, 
                     rc);  
    }
    return;
}


/*
 * This function is used to destroy a demo state machine engine  
 */
void 
demo_fsm_destroy (demo_config_t *p2config,
                  demo_context_t *p2context)
{
    if (!p2config) {
        DEMO_ERROR("%s-%u Error: Config is null \n",
                      __FUNCTION__,
                      __LINE__);
        return;
    }

    if (!p2context) {
        DEMO_ERROR("%s-%u Error: Context is null \n",
                      __FUNCTION__,
                      __LINE__);
        return;
    }

    fsm_destroy((fsm_t **)&fsm_handle); 
    return;
}


/*
 * This function is used to create a demo state machine
 * when the session is instaniated.  
 */
void 
demo_fsm_create (demo_config_t *p2config, 
                 demo_context_t *p2context)
{
    RC_FSM_t  rc;

    if (!p2config) {
        DEMO_ERROR("%s-%u Error: Config is null \n",
                      __FUNCTION__,
                      __LINE__);
        return;
    } 

    if (!p2context) {
        DEMO_ERROR("%s-%u Error: Context is null \n",
                      __FUNCTION__,
                      __LINE__);
        return;
    } 

    fsm_handle = NULL;

    rc = fsm_create((fsm_t **)&fsm_handle, 
                    "Demo State Machine", 
                     translator_idle_s,
                     normalized_state_table, 
                     normalized_event_table, 
                     demo_state_table);  
   if (rc != RC_FSM_OK) {   
       DEMO_ERROR("%s-%u Error: demo state machine rc=%u \n", 
                __FUNCTION__,
                __LINE__, 
                rc);  
   } 

   return; 
} 


