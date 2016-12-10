#include "usbd_bulk_if.h"

#define APP_RX_DATA_SIZE  64
extern int8_t Receive_FS(uint8_t* Buf,uint32_t *Len);

static uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

USBD_HandleTypeDef  *hUsbDevice_0;
extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t BULK_Init_FS    (void);
static int8_t BULK_DeInit_FS  (void);
static int8_t BULK_Control_FS (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t BULK_Receive_FS (uint8_t* Buf, uint32_t *Len);

// should be const, but annoying ST library is not declared const

USBD_BULK_ItfTypeDef USBD_Interface_fops_FS = 
{
  BULK_Init_FS,
  BULK_DeInit_FS,
  BULK_Receive_FS
};

static int8_t BULK_Init_FS(void)
{
  hUsbDevice_0 = &hUsbDeviceFS;
  // here's where the input buffer gets set to kick off 
  // the receiver
  USBD_BULK_SetRxBuffer(hUsbDevice_0,UserRxBufferFS);
  HAL_GPIO_WritePin(GPIOE,LD4_Pin,1);
  return (USBD_OK);
}

static int8_t BULK_DeInit_FS(void)
{
  HAL_GPIO_WritePin(GPIOE,LD4_Pin,0);
  hUsbDevice_0=NULL;
  return (USBD_OK);
}

static int8_t BULK_Receive_FS (uint8_t* Buf, uint32_t *Len)
{
  return Receive_FS(Buf, Len);
}

uint8_t BULK_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  USBD_BULK_HandleTypeDef *hcdc;
  hcdc = (USBD_BULK_HandleTypeDef*)hUsbDevice_0->pClassData;
  
  if (hUsbDevice_0 == NULL)
    return -1;

  USBD_BULK_SetTxBuffer(hUsbDevice_0, Buf, Len);
  if ((result = USBD_BULK_TransmitPacket(hUsbDevice_0) != USBD_OK))
    return result;

  // blocking transmit -- need to wait until transmit is complete
  // move this up to stlink level

  while (((volatile uint32_t) hcdc->TxState)) {}
  return result;
}

