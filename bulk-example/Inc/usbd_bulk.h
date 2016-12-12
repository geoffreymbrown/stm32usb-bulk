#ifndef __USB_BULK_H
#define __USB_BULK_H

#ifdef __cplusplus
 extern "C" {
#endif


#include  "usbd_ioreq.h"

#define BULK_IN_EP                        0x81  /* EP1 for data IN */
#define BULK_OUT_EP                       0x02  /* EP1 for data OUT */


/* BULK Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */

#define BULK_DATA_HS_MAX_PACKET_SIZE      64
#define BULK_DATA_FS_MAX_PACKET_SIZE      64  /* Endpoint IN & OUT Packet size */
#define BULK_CMD_PACKET_SIZE              8  /* Control Endpoint Packet size */ 

#define USB_BULK_CONFIG_DESC_SIZ          32

#define BULK_DATA_FS_IN_PACKET_SIZE       BULK_DATA_FS_MAX_PACKET_SIZE
#define BULK_DATA_FS_OUT_PACKET_SIZE      BULK_DATA_FS_MAX_PACKET_SIZE

typedef struct _USBD_BULK_Itf
{
  int8_t (* Init)          (void);
  int8_t (* DeInit)        (void);
  int8_t (* Receive)       (uint8_t *, uint32_t *);  

}USBD_BULK_ItfTypeDef;


typedef struct
{
  uint32_t data[BULK_DATA_HS_MAX_PACKET_SIZE/4];      /* Force 32bits alignment */
  uint32_t RxLength;
  uint32_t TxLength;    
  uint8_t  *RxBuffer;  
  uint8_t  *TxBuffer;   
  __IO uint32_t TxState;     
  __IO uint32_t RxState;    
}
USBD_BULK_HandleTypeDef; 

   // should be const, but annoying st library discards const

extern USBD_ClassTypeDef  USBD_BULK;
#define USBD_BULK_CLASS    &USBD_BULK

uint8_t  USBD_BULK_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                      USBD_BULK_ItfTypeDef *fops);

uint8_t  USBD_BULK_SetTxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff,
                                      uint16_t length);

uint8_t  USBD_BULK_SetRxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff);
  
uint8_t  USBD_BULK_ReceivePacket      (USBD_HandleTypeDef *pdev);

uint8_t  USBD_BULK_TransmitPacket     (USBD_HandleTypeDef *pdev);

#ifdef __cplusplus
}
#endif

#endif  /* __USB_BULK_H */
