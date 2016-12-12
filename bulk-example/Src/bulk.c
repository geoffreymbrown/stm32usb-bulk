#include "stm32f3xx_hal.h"
#include "usb_device.h"
#include "usbd_bulk_if.h"

#define QUEUE_SIZE 256
extern USBD_HandleTypeDef  *hUsbDevice_0;
static volatile int rxKick = 0;
volatile int rxEOF  = 0;

uint8_t txbuf[256] __attribute__((aligned(4)));
static struct Queue rxQ;

struct Queue {
  uint32_t pRD, pWR;
  uint8_t  q[QUEUE_SIZE];
};

#define QueueAvail(q) ((QUEUE_SIZE + (q)->pWR - (q)->pRD) % QUEUE_SIZE)
#define QueueSpace(q) (QUEUE_SIZE - 1 - QueueAvail(q))

static int Enqueue(struct Queue *q, const uint8_t *data, uint32_t len){
  int i;
  len = MIN(len,QueueSpace(q));
  for (i = 0; i < len; i++) {
      q->q[q->pWR] = data[i];
      q->pWR = (q->pWR + 1) % QUEUE_SIZE;
    }
  return len;
}

static int Dequeue(struct Queue *q, uint8_t *data, uint32_t len){
  int i;
  len = MIN(len,QueueAvail(q));
  for (i = 0; i < len; i++){
      data[i] = q->q[q->pRD];
      q->pRD = (q->pRD + 1) % QUEUE_SIZE;
    }
  return len;
}

int8_t Receive_FS (uint8_t* Buf, uint32_t *Len){
  int count = *Len;

  if (*Len && ((Enqueue(&rxQ, Buf, *Len) != *Len))) {
    // Shouldn't happen ! this is an overflow
  }

  HAL_GPIO_WritePin(GPIOE,LD7_Pin, *Len == 0);
  if (*Len == 0)
    rxEOF = 1;
  //  HAL_GPIO_WritePin(GPIOE,LD8_Pin, *Len == 64);

  // release receive buffer ?  Only if there's enough space
  // for the next packet
  
  if (USB_FS_MAX_PACKET_SIZE >= QueueSpace(&rxQ)) {
    rxKick = 1;
  } else {
    USBD_BULK_ReceivePacket(hUsbDevice_0);
  }
  return (USBD_OK);
}

uint32_t bulk_read_cnt(void) {
  return QueueAvail(&rxQ);
}

uint32_t bulk_read(uint8_t *pBuf, uint32_t buf_len) {

  uint32_t cnt = Dequeue(&rxQ, pBuf, buf_len);

  // release receive buffer 

  if (rxKick && (USB_FS_MAX_PACKET_SIZE <= QueueSpace(&rxQ))) {
    //    HAL_GPIO_TogglePin(GPIOE,LD5_Pin);
    rxKick = 0;
    USBD_BULK_ReceivePacket(hUsbDevice_0);
  }

  return cnt;
}

