/* pevent --- portable event objects
 * Copyright (C) 2015 Katayama Hirofumi MZ.
 * This file is released under the terms of the Modified BSD License.
 */

#ifdef _WIN32

/*--------------------------------------------------------------------------*/

#include <windows.h>

#include "pevent.h"

/*--------------------------------------------------------------------------*/
/* the definitions of C functions (not optimized; see win32_pevent.h) */

#ifdef __cplusplus
extern "C" {
#endif

pe_event_t pe_create_event(bool manual_reset, bool initial_state) {
    HANDLE handle = CreateEvent(NULL, manual_reset, initial_state, NULL);
    return (pe_event_t)handle;
} /* pe_create_event */

bool pe_close_event(pe_event_t event) {
    HANDLE handle = (HANDLE)event;
    BOOL result = CloseHandle(handle);
    return (result != FALSE);
} /* pe_close_event */

bool pe_wait_for_event(pe_event_t event, uint32_t milliseconds) {
    HANDLE handle = (HANDLE)event;
    DWORD result = WaitForSingleObject(handle, milliseconds);
    return (result == WAIT_TIMEOUT);
} /* pe_wait_for_event */

bool pe_set_event(pe_event_t event) {
    HANDLE handle = (HANDLE)event;
    BOOL result = SetEvent(handle);
    return (result != FALSE);
} /* pe_set_event */

bool pe_reset_event(pe_event_t event) {
    HANDLE handle = (HANDLE)event;
    BOOL result = ResetEvent(handle);
    return (result != FALSE);
} /* pe_reset_event */

bool pe_pulse_event(pe_event_t event) {
    HANDLE handle = (HANDLE)event;
    BOOL result = PulseEvent(handle);
    return (result != FALSE);
} /* pe_pulse_event */

#ifdef __cplusplus
} // extern "C"
#endif

/*--------------------------------------------------------------------------*/

#endif  /* def _WIN32 */

/*--------------------------------------------------------------------------*/
