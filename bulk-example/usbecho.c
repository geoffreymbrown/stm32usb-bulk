#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <libusb.h>



int main(void)
{
  int result;
  struct libusb_device_descriptor desc;
  struct libusb_config_descriptor *config;    

  libusb_device **list;
  libusb_device *my_device = NULL;

  result = libusb_init(NULL);
  libusb_set_debug(NULL, 3);

  ssize_t count = libusb_get_device_list(NULL, &list);
  for (int i = 0; i < count; i++) {
    libusb_device *device = list[i];
    result = libusb_get_device_descriptor(device, &desc);
    if(desc.idVendor == 0x483 && desc.idProduct == 0x5740){

      printf("\nDevice Descriptors: ");    
      printf("\n\tVendor ID : %x",desc.idVendor);    
      printf("\n\tProduct ID : %x",desc.idProduct);    
      printf("\n\tSerial Number : %x",desc.iSerialNumber);    
      printf("\n\tSize of Device Descriptor : %d",desc.bLength);    
      printf("\n\tType of Descriptor : %d",desc.bDescriptorType);    
      printf("\n\tUSB Specification Release Number : %d",desc.bcdUSB);    
      printf("\n\tDevice Release Number : %d",desc.bcdDevice);    
      printf("\n\tDevice Class : %d",desc.bDeviceClass);    
      printf("\n\tDevice Sub-Class : %d",desc.bDeviceSubClass);    
      printf("\n\tDevice Protocol : %d",desc.bDeviceProtocol);    
      printf("\n\tMax. Packet Size : %d",desc.bMaxPacketSize0);    
      printf("\n\tNo. of Configurations : %d\n",desc.bNumConfigurations);    

      my_device = device;
      break;
    }
  }

  if (my_device == NULL) {
    printf("can't find device\n");
    exit(1);
  }

  if(my_device != NULL) {
    char buf[256];
    libusb_device_handle *handle;
    result = libusb_open(my_device, &handle);
    int kernelActive = libusb_kernel_driver_active(handle, 0);
    if(kernelActive == 1) {
      result = libusb_detach_kernel_driver(handle, 0);
    }
    result = libusb_claim_interface (handle, 0);
    if (result) 
      printf("couldn't claim interface \n");
    //    result = libusb_control_transfer(handle,0x21,34,0x0003,0,NULL,0,0);
    
    char *end;
    while ((end = fgets(buf, sizeof buf, stdin))) {
	int t;
	int res;
	size_t size = strlen(buf) + 1;
	if ((t = libusb_bulk_transfer(handle,
				      0x01, (unsigned char *) buf,
				      size,
				      &res,
				      3000))) {
	  printf("[!] send request failed %s\n", libusb_error_name(t));
	  break;
	}
	printf("sent  %d bytes of %zu \n", res, size);
	if (!(res & 63)){ // send zlp
	  libusb_bulk_transfer(handle,
			       0x01, (unsigned char *) buf,
			       0,
			       &res,
			       3000);
	}
	
	buf[0] = 0;
	res = 0;

	if ((t = libusb_bulk_transfer(handle,
				      0x81, (unsigned char *) buf,
				      256,
				      &res,
				      3000))) {
	  printf("[!] receive reuest failed %s\n", libusb_error_name(t));
	  buf[0] = 0;
	  break;
	}
	printf("received  %d bytes: %s\n", res, buf);
    }
    libusb_release_interface (handle, 0);
    if(kernelActive == 1) {
      result = libusb_attach_kernel_driver(handle, 0);
    }
    libusb_close(handle);
  }
  libusb_free_device_list(list, 1);
  libusb_exit(NULL);
  return EXIT_SUCCESS;
}
