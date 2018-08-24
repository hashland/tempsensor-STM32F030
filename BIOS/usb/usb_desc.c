//*----------------------------------------------------------------------------
// USB_DESC.C
//*----------------------------------------------------------------------------
#include "usb_main.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "sys_config.h"
//*----------------------------------------------------------------------------
// HID REPORT DESCRIPTOR
//*----------------------------------------------------------------------------
const uint8_t HID_ReportDescriptor[] = {
  HID_UsagePageVendor(0xAB),
  HID_UsageS(0x0200),
  HID_Collection(HID_Application),
    HID_ReportSize(8),
    HID_LogicalMin(0),
    HID_LogicalMaxS(0xFF),
    HID_ReportCount(64),  
    HID_Usage(0x01),
    HID_Input(2),
    HID_ReportCount(64),  
    HID_Usage(0x02),
    HID_Output(2),
  HID_EndCollection,
};
//*----------------------------------------------------------------------------
const uint32_t HID_ReportDescSize = sizeof(HID_ReportDescriptor);
//*----------------------------------------------------------------------------
// DEVICE-DESCRIPTOR
//*----------------------------------------------------------------------------
const S_usb_device_descriptor DevDesc = {
  .bLength          = sizeof(S_usb_device_descriptor), // Size of this descriptor
  .bDescriptorType  = USB_DESC_TYPE_DEVICE,            // DEVICE Descriptor Type
  .bcdUSB           = USB2_00,                         // USB 2.00 specification
  .bDeviceClass     = 0x00,                            // CLASS
  .bDeviceSubClass  = 0x00,                            // Device subclass code
  .bDeviceProtocol  = 0x00,                            // Device protocol code
  .bMaxPacketSize0  = EP_CONTROL_SIZE,                 // Maximum packet size for endpoint zero
  .idVendor         = USB_VID,                         // VID
  .idProduct        = USB_PID,                         // PID
  .bcdDevice        = (BIOS_VER_MAJOR << 8) | BIOS_VER_MINOR, // Device release number 
  .iManufacturer    = 0x01,                            // Index of manufacturer description
  .iProduct         = 0x02,                            // Index of product description
  .iSerialNumber    = 0x00,                            // Index of serial number description
  .bNumConfig       = 0x01                             // One possible configuration
};   
//*----------------------------------------------------------------------------
// CONFIGURATION DESCRIPTOR
//*----------------------------------------------------------------------------
const S_ser_configuration_descriptor ConfDesc = {
  .sCfg.bLength               = sizeof(S_usb_configuration_descriptor), // Size of this descriptor
  .sCfg.bDescriptorType       = USB_DESC_TYPE_CONFIGURATION,            // CONFIGURATION descriptor type
  .sCfg.wTotalLength          = sizeof(S_ser_configuration_descriptor), // Total size of this configuration (including other descriptors)
  .sCfg.bNumInterfaces        = 0x01,                                   // 1 interface is used by this configuration
  .sCfg.bConfValue            = 0x01,                                   // Value 0x01 is used to select this configuration
  .sCfg.iConfiguration        = 0x00,                                   // No string is used to describe this configuration
  .sCfg.bmAttributes          = USB_CONFIG_SELF_WAKEUP,                 // Device is BUS-powered and does not support remote wakeup
  .sCfg.bMaxPower             = USB_POWER_MA(100),                      // Maximum power consumption of the device is 100mA
  //......................................................................................    
  // HID - INTERFACE
  //......................................................................................    
  .sHid.bLength             = sizeof(S_usb_interface_descriptor),       // Size of this descriptor
  .sHid.bDescriptorType     = USB_DESC_TYPE_INTERFACE,                  // INTERFACE descriptor type
  .sHid.bInterfaceNumber    = USB_HID_IF_NUM,                           // Interface 0x00
  .sHid.bAlternateSetting   = 0x00,                                     // No alternate settings
  .sHid.bNumEndpoints       = 0x02,                                     // Num Endpoint used
  .sHid.bInterfaceClass     = USB_CLASS_HUMAN_INTERFACE,                // Data interface class
  .sHid.bInterfaceSubClass  = HID_SUBCLASS_NONE,                        // No subclass
  .sHid.bInterfaceProtocol  = HID_PROTOCOL_NONE,                        // No protocol code
  .sHid.iInterface          = 0x00,                                     // No description string
  
  .sHDesc.bLength           = sizeof(S_hid_descriptor),                 // Size of this descriptor
  .sHDesc.bDescriptorType   = HID_HID_DESCRIPTOR_TYPE,                  // INTERFACE descriptor type
  .sHDesc.bcdHID            = 0x0111,
  .sHDesc.bCountryCode      = 0x00,
  .sHDesc.bNumDescriptors   = 0x01,
  .sHDesc.bHidType          = HID_REPORT_DESCRIPTOR_TYPE,
  .sHDesc.wDescriptorLength = sizeof(HID_ReportDescriptor),
  
  .sHidIn.bLength            = sizeof(S_usb_endpoint_descriptor),      // Size of this descriptor in bytes
  .sHidIn.bDescriptorType    = USB_DESC_TYPE_ENDPOINT,                 // ENDPOINT descriptor type
  .sHidIn.bEndpointAddress   = EP_HID_IN | EP_DIR_IN,                  // IN endpoint
  .sHidIn.bmAttributes       = EP_TYPE_INTR,                           // Bulk endpoint
  .sHidIn.wMaxPacketSize     = EP_HID_SIZE,                            // Endpoint size 
  .sHidIn.bInterval          = 2,                                      // Must be 0x00 for full-speed bulk
    
  .sHidOut.bLength           = sizeof(S_usb_endpoint_descriptor),      // Size of this descriptor in bytes
  .sHidOut.bDescriptorType   = USB_DESC_TYPE_ENDPOINT,                 // ENDPOINT descriptor type
  .sHidOut.bEndpointAddress  = EP_HID_OUT | EP_DIR_OUT,                // OUT endpoint
  .sHidOut.bmAttributes      = EP_TYPE_INTR,                           // Bulk endpoint
  .sHidOut.wMaxPacketSize    = EP_HID_SIZE,                            // Endpoint size 
  .sHidOut.bInterval         = 8,                                      // Must be 0x00 for full-speed bulk
  //......................................................................................    
};

//*----------------------------------------------------------------------------
// STRING DESCRIPTOREN
//*----------------------------------------------------------------------------
const S_usb_language_id sLanguageID = {
  USB_STRING_DESCRIPTOR_SIZE(1),
  USB_DESC_TYPE_STRING, 
  0x0409
};
//*----------------------------------------------------------------------------
const char strDescriptor1[] = {
	USB_STRING_DESCRIPTOR_SIZE(11), // bLength
	USB_DESC_TYPE_STRING,           // bDescriptorType
  USB_UNICODE('D'),
  USB_UNICODE('I'),
  USB_UNICODE('A'),
  USB_UNICODE('M'),
  USB_UNICODE('E'),
  USB_UNICODE('X'),
  USB_UNICODE(' '),
  USB_UNICODE('G'),
  USB_UNICODE('m'),
  USB_UNICODE('b'),
  USB_UNICODE('H')
};
//*----------------------------------------------------------------------------
const char strDescriptor2[] = {
	USB_STRING_DESCRIPTOR_SIZE(18),  // bLength
	USB_DESC_TYPE_STRING,            // bDescriptorType
  USB_UNICODE('T'),
  USB_UNICODE('e'),
  USB_UNICODE('m'),
  USB_UNICODE('p'),
  USB_UNICODE('-'),
  USB_UNICODE('S'),
  USB_UNICODE('e'),
  USB_UNICODE('n'),
  USB_UNICODE('s'),
  USB_UNICODE('o'),
  USB_UNICODE('r'),
  USB_UNICODE('-'),
  USB_UNICODE('T'),
  USB_UNICODE('e'),
  USB_UNICODE('s'),
  USB_UNICODE('t'),
  USB_UNICODE('e'),
  USB_UNICODE('r'),
};
//*----------------------------------------------------------------------------
const char strDescriptor3[] = {
	USB_STRING_DESCRIPTOR_SIZE(9),  // bLength
	USB_DESC_TYPE_STRING,           // bDescriptorType
  USB_UNICODE('N'),
  USB_UNICODE('O'),
  USB_UNICODE(' '),
  USB_UNICODE('S'),
  USB_UNICODE('E'),
  USB_UNICODE('R'),
  USB_UNICODE('I'),
  USB_UNICODE('A'),
  USB_UNICODE('L')
};
//*----------------------------------------------------------------------------
// POINTER AUF STRING-DESCRIPTOREN
//*----------------------------------------------------------------------------
const char *StrDesc[4] = {
  (char *)&sLanguageID,
  (char *)&strDescriptor1,
  (char *)&strDescriptor2,
  (char *)&strDescriptor3
};  
//*----------------------------------------------------------------------------

