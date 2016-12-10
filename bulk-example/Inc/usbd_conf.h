#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdlib.h>
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "usbd_def.h"

#define USBD_MAX_NUM_INTERFACES        1
#define USBD_MAX_NUM_CONFIGURATION     1
#define USBD_MAX_STR_DESC_SIZ        512
#define USBD_SUPPORT_USER_STRING       0
#define USBD_DEBUG_LEVEL               0
#define USBD_SELF_POWERED              1
#define USBD_BULKC_INTERVAL         1000
#define DEVICE_FS 		       0

/* Memory management macros */  

#define USBD_malloc               (uint32_t *)USBD_static_malloc
#define USBD_free                 USBD_static_free
#define USBD_memset               /* Not used */
#define USBD_memcpy               /* Not used */

#define USBD_Delay   HAL_Delay

void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);    

/* DEBUG macros */    

#define USBD_UsrLog(...)   
#define USBD_ErrLog(...)   
#define USBD_DbgLog(...)                         

#ifdef __cplusplus
}
#endif
#endif //__USBD_CONF__H__
