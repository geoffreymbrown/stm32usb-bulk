#include "usbd_bulk.h"
#include "usbd_ctlreq.h"

/* USB BULK device Configuration Descriptor */
/* should be const but annoying ST library discards const */

__ALIGN_BEGIN uint8_t USBD_BULK_CfgFSDesc[USB_BULK_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,                               /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,        /* bDescriptorType: Configuration */
  USB_BULK_CONFIG_DESC_SIZ,            /* wTotalLength:no of returned bytes */
  0x00,
  0x01,                               /* bNumInterfaces: 1 interface */
  0x01,                               /* bConfigurationValue:  e   */
  0x00,                               /* Index of string descriptor */
  0xC0,                               /* bmAttributes: self powered */
  0x32,                               /* MaxPower 50x2 mA */
  
  /*---------------------------------------------------------------------------*/
  
  /*Interface Descriptor */
  0x09,                               /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,            /* bDescriptorType: Interface */
  0x00,                               /* bInterfaceNumber: Number of Interface */
  0x00,                               /* bAlternateSetting: Alternate setting  */
  0x02,                               /* bNumEndpoints: two endpoints used     */
  0xFF,                               /* bInterfaceClass: Vendor Specific      */
  0x00,                               /* bInterfaceSubClass:                   */
  0x00,                               /* bInterfaceProtocol:                   */
  0x00,                               /* iInterface:                           */
  
  /*Endpoint OUT Descriptor*/
  0x07,                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,             /* bDescriptorType: Endpoint */
  BULK_OUT_EP,                         /* bEndpointAddress */
  0x02,                               /* bmAttributes: Bulk */
  LOBYTE(BULK_DATA_FS_MAX_PACKET_SIZE),/* wMaxPacketSize: */
  HIBYTE(BULK_DATA_FS_MAX_PACKET_SIZE),
  0x00,                               /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,             /* bDescriptorType: Endpoint */
  BULK_IN_EP,                          /* bEndpointAddress */
  0x02,                               /* bmAttributes: Bulk */
  LOBYTE(BULK_DATA_FS_MAX_PACKET_SIZE),/* wMaxPacketSize: */
  HIBYTE(BULK_DATA_FS_MAX_PACKET_SIZE),
  0x00                                /* bInterval: ignore for Bulk transfer */
} ;

static uint8_t  USBD_BULK_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  uint8_t ret = 0;
  USBD_BULK_HandleTypeDef   *hcdc;
  
  {
    /* Open EP IN */
    USBD_LL_OpenEP(pdev,
                   BULK_IN_EP,
                   USBD_EP_TYPE_BULK,
                   BULK_DATA_FS_IN_PACKET_SIZE);
    
    /* Open EP OUT */
    USBD_LL_OpenEP(pdev,
                   BULK_OUT_EP,
                   USBD_EP_TYPE_BULK,
                   BULK_DATA_FS_OUT_PACKET_SIZE);
  }

  pdev->pClassData = USBD_malloc(sizeof (USBD_BULK_HandleTypeDef));
  
  if(pdev->pClassData == NULL)
  {
    ret = 1; 
  }
  else
  {
    hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
    
    /* Init  physical Interface components */

    ((USBD_BULK_ItfTypeDef *)pdev->pUserData)->Init();
    
    /* Init Xfer states */

    hcdc->TxState =0;
    hcdc->RxState =0;
       
      /* Prepare Out endpoint to receive next packet */

    USBD_LL_PrepareReceive(pdev,
			   BULK_OUT_EP,
			   hcdc->RxBuffer,
			   BULK_DATA_FS_OUT_PACKET_SIZE);
  }
  return ret;
}

static uint8_t  USBD_BULK_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{
  uint8_t ret = 0;
  
  /* Close EP IN */
  USBD_LL_CloseEP(pdev,
              BULK_IN_EP);
  
  /* Close EP OUT */
  USBD_LL_CloseEP(pdev,
              BULK_OUT_EP);
  
  /* DeInit  physical Interface components */
  if(pdev->pClassData != NULL)
  {
    ((USBD_BULK_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  
  return ret;
}

static uint8_t  USBD_BULK_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
  static uint8_t ifalt = 0;
    
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        &ifalt,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      break;
    }
 
  default: 
    break;
  }
  return USBD_OK;
}

static uint8_t  USBD_BULK_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_BULK_HandleTypeDef   *hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
  
  if(pdev->pClassData != NULL)
  {
    
    hcdc->TxState = 0;

    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

static uint8_t  USBD_BULK_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum)
{      
  USBD_BULK_HandleTypeDef   *hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
  
  /* Get the received data length */
  hcdc->RxLength = USBD_LL_GetRxDataSize (pdev, epnum);
  
  /* USB data will be immediately processed, this allow next USB traffic being 
  NAKed till the end of the application Xfer */
  if(pdev->pClassData != NULL)
  {
    ((USBD_BULK_ItfTypeDef *)pdev->pUserData)->Receive(hcdc->RxBuffer, 
						      &hcdc->RxLength);

    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

static uint8_t  USBD_BULK_EP0_RxReady (USBD_HandleTypeDef *pdev)
{ 
  return USBD_OK;
}

static uint8_t  *USBD_BULK_GetFSCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_BULK_CfgFSDesc);
  return USBD_BULK_CfgFSDesc;
}

uint8_t  USBD_BULK_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                      USBD_BULK_ItfTypeDef *fops)
{
  uint8_t ret = USBD_FAIL;    
  if(fops != NULL)
  {
    pdev->pUserData= fops;
    ret = USBD_OK;    
  }
  
  return ret;
}

uint8_t  USBD_BULK_SetTxBuffer  (USBD_HandleTypeDef   *pdev,
                                uint8_t  *pbuff,
                                uint16_t length)
{
  USBD_BULK_HandleTypeDef   *hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
  
  hcdc->TxBuffer = pbuff;
  hcdc->TxLength = length;  
  
  return USBD_OK;  
}

uint8_t  USBD_BULK_SetRxBuffer  (USBD_HandleTypeDef   *pdev,
                                   uint8_t  *pbuff)
{
  USBD_BULK_HandleTypeDef   *hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
  
  hcdc->RxBuffer = pbuff;
  
  return USBD_OK;
}

uint8_t  USBD_BULK_TransmitPacket(USBD_HandleTypeDef *pdev)
{      
  USBD_BULK_HandleTypeDef   *hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
  
  if(pdev->pClassData != NULL)
  {
    if(((volatile uint32_t)hcdc->TxState) == 0)
    {
      /* Tx Transfer in progress */
      hcdc->TxState = 1;
      
      /* Transmit next packet */
      USBD_LL_Transmit(pdev,
                       BULK_IN_EP,
                       hcdc->TxBuffer,
                       hcdc->TxLength);
      
      return USBD_OK;
    }
    else
    {
      return USBD_BUSY;
    }
  }
  else
  {
    return USBD_FAIL;
  }
}

uint8_t  USBD_BULK_ReceivePacket(USBD_HandleTypeDef *pdev)
{      
  USBD_BULK_HandleTypeDef   *hcdc = (USBD_BULK_HandleTypeDef*) pdev->pClassData;
  
  /* Suspend or Resume USB Out process */
  if(pdev->pClassData != NULL)
  {
    USBD_LL_PrepareReceive(pdev,
			   BULK_OUT_EP,
			   hcdc->RxBuffer,
			   BULK_DATA_FS_OUT_PACKET_SIZE);
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

/* BULK interface class callbacks structure */


USBD_ClassTypeDef  USBD_BULK = 
{
  USBD_BULK_Init,
  USBD_BULK_DeInit,
  USBD_BULK_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_BULK_EP0_RxReady,
  USBD_BULK_DataIn,
  USBD_BULK_DataOut,
  NULL,
  NULL,
  NULL,     
  USBD_BULK_GetFSCfgDesc,  
  USBD_BULK_GetFSCfgDesc,    
  USBD_BULK_GetFSCfgDesc, 
  NULL,
};
