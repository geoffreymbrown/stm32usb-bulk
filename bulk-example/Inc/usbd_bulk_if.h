#ifndef __USBD_BULK_IF_H
#define __USBD_BULK_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_bulk.h"
extern  USBD_BULK_ItfTypeDef  USBD_Interface_fops_FS;
uint8_t BULK_Transmit_FS(uint8_t* Buf, uint16_t Len);
#ifdef __cplusplus
}
#endif
  
#endif /* __USBD_BULK_IF_H */

