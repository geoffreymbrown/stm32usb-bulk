#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <libusb.h>

int print_configuration(struct libusb_config_descriptor *config)    
{    
  //  unsigned char *data;    
  int index;    

  //  data = (unsigned char *)malloc(512);    
  //  memset(data,0,512);    

  if (config == NULL)
    return 0;
  index = config->iConfiguration;    

  //  libusb_get_string_descriptor_ascii(hDevice,index,data,512);    

  printf("\nInterface Descriptors: ");    
  printf("\n\tLength : %d",config->bLength);    
  printf("\n\tDesc_Type : %d",config->bDescriptorType);    
  printf("\n\tTotal length : %hu",config->wTotalLength);    
  printf("\n\tNumber of Interfaces : %d",config->bNumInterfaces);    
  printf("\n\tConfig_index : %d",config->iConfiguration);    
  printf("\n\tConfiguration Value  : %d",config->bConfigurationValue);    
  printf("\n\tConfiguration Attributes : %d",config->bmAttributes);    
  printf("\n\tMaxPower(mA) : %d\n",2*config->MaxPower);    

  //  free(data);    
  return 0;    
}    

struct libusb_endpoint_descriptor* active_config(struct libusb_device *dev)//,struct libusb_device_handle *handle)    
{    
  struct libusb_device_handle *hDevice_req;    
  struct libusb_config_descriptor *config;    
  const struct libusb_endpoint_descriptor *endpoint;    
  int altsetting_index,interface_index=0,ret_active;    
  int i,ret_print;    
  int interface_number;

  //  hDevice_req = handle;    

  ret_active = libusb_get_active_config_descriptor(dev,&config);    
  if (ret_active)
    return 0;
  ret_print = print_configuration(config);    

  for(interface_index=0;interface_index<config->bNumInterfaces;interface_index++)    
    {    
      const struct libusb_interface *iface = &config->interface[interface_index];    
      for(altsetting_index=0;altsetting_index<iface->num_altsetting;altsetting_index++)    
        {    
	  const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_index];    

	  int endpoint_index;    
	  for(endpoint_index=0;endpoint_index<altsetting->bNumEndpoints;endpoint_index++)    
            {    
	      const struct libusb_endpoint_descriptor *ep = &altsetting->endpoint[endpoint_index];    
	      endpoint = ep;      
	      //	      alt_interface = altsetting->bAlternateSetting;    
	      //	      interface_number = altsetting->bInterfaceNumber;    

	  printf("\nEndPoint Descriptors: ");    
	  printf("\n\tSize of EndPoint Descriptor : %d",endpoint->bLength);    
	  printf("\n\tType of Descriptor : %d",endpoint->bDescriptorType);    
	  printf("\n\tEndpoint Address : 0x0%x",endpoint->bEndpointAddress);    
	  printf("\n\tMaximum Packet Size: %x",endpoint->wMaxPacketSize);    
	  printf("\n\tAttributes applied to Endpoint: %d",endpoint->bmAttributes);    
	  printf("\n\tInterval for Polling for data Tranfer : %d\n",endpoint->bInterval);    
	    }
        }    
    }    
  libusb_free_config_descriptor(NULL);    
  //  return endpoint;    
  return 0;
}    


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
    if(desc.idVendor == 0x483 && desc.idProduct == 0x5740){//0x3748){//
      printf("\nDevice Descriptors: ");    
      printf("\n\tVendor ID : %x",desc.idVendor);    
      printf("\n\tProduct ID : %x",desc.idProduct);    
      printf("\n\tSerial Number : %x", desc.iSerialNumber);
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

  // can't get the descriptor !

  //  printf("print configuration\n");
  if (!(libusb_get_config_descriptor (my_device, 0, &config))) {
    printf("\nConfiguration 0\n");
    printf("\tConfiguration Length %d\n", config->bLength);
    printf("\tDescriptor type %d\n", config->bDescriptorType);
    printf("\tDescriptor interfaces %d\n", config->bNumInterfaces);
    printf("\tDescriptor attributes %d\n", config->bmAttributes);
    printf("\tDescriptor max power %d\n", config->MaxPower);
    //    libusb_free_config_descriptor(config);
    config = NULL;
  } else { 
    printf("\ncouldn't open configuration 0\n");
  }

  active_config(my_device);

  if(my_device != NULL) {
    libusb_device_handle *handle;
    result = libusb_open(my_device, &handle);
    /*
    printf("libusb_open result %d\n", result);
    unsigned char data[512];
    if (libusb_get_descriptor(handle, LIBUSB_DT_CONFIG, 0, data, 512) > 0) {
      printf("config 0 descriptor:");
      for (int i = 0; i < data[0]; i++)
	printf("%x ", data[i]);
      printf("\n");
    }
    */

    char serial[256];
    for (int i = 0; i < 20; i++) {
      if ((libusb_get_string_descriptor_ascii(handle,i,
					      (unsigned char *) serial,256)) < 0)
	continue;
      printf("\tstring %d : %s\n", i, serial);
    }

    int kernelActive = libusb_kernel_driver_active(handle, 0);
    if(kernelActive == 1) {
      result = libusb_detach_kernel_driver(handle, 0);
    }
    result = libusb_claim_interface (handle, 0);
    if (result) 
      printf("couldn't claim interface \n");
    //    result = libusb_control_transfer(handle,0x21,34,0x0003,0,NULL,0,0);
    
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
