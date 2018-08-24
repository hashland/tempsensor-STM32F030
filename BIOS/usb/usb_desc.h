//*----------------------------------------------------------------------------
//* USB_DESC.H
//*----------------------------------------------------------------------------
#ifndef __USB_DESC_H
#define __USB_DESC_H
//*----------------------------------------------------------------------------
#include "stm32f0xx.h"
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char  bLength;              //!< Size of this descriptor in bytes
   unsigned char  bDescriptorType;      //!< DEVICE descriptor type
   unsigned short bcdUSB;               //!< USB specification release number
   unsigned char  bDeviceClass;         //!< Class code
   unsigned char  bDeviceSubClass;      //!< Subclass code
   unsigned char  bDeviceProtocol;      //!< Protocol code
   unsigned char  bMaxPacketSize0;      //!< Control endpoint 0 max. packet size
   unsigned short idVendor;             //!< Vendor ID
   unsigned short idProduct;            //!< Product ID
   unsigned short bcdDevice;            //!< Device release number
   unsigned char  iManufacturer;        //!< Index of manu. string descriptor
   unsigned char  iProduct;             //!< Index of prod. string descriptor
   unsigned char  iSerialNumber;        //!< Index of S.N.  string descriptor
   unsigned char  bNumConfig;           //!< Number of possible configurations
} S_usb_device_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char  bLength;              //!< Size of this descriptor in bytes
   unsigned char  bDescriptorType;      //!< CONFIGURATION descriptor type
   unsigned short wTotalLength;         //!< Total length of data returned for this configuration
   unsigned char  bNumInterfaces;       //!< Number of interfaces for this configuration
   unsigned char  bConfValue;           //!< Value to use as an argument
   unsigned char  iConfiguration;       //!< Index of string descriptor
   unsigned char  bmAttributes;         //!< Configuration characteristics
   unsigned char  bMaxPower;            //!< Maximum power consumption of the device
} S_usb_configuration_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bLength;               //!< Size of this descriptor in bytes
   unsigned char bDescriptorType;       //!< INTERFACE descriptor type
   unsigned char bInterfaceNumber;      //!< Number of this interface
   unsigned char bAlternateSetting;     //!< Value used to select this alternate setting
   unsigned char bNumEndpoints;         //!< Number of endpoints used by this interface (excluding endpoint zero)
   unsigned char bInterfaceClass;       //!< Class code
   unsigned char bInterfaceSubClass;    //!< Sub-class
   unsigned char bInterfaceProtocol;    //!< Protocol code
   unsigned char iInterface;            //!< Index of string descriptor
} S_usb_interface_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char  bLength;              //!< Size of this descriptor in bytes
   unsigned char  bDescriptorType;      //!< ENDPOINT descriptor type
   unsigned char  bEndpointAddress;     //!< Address of the endpoint on the USB
   unsigned char  bmAttributes;         //!< Endpoint attributes when configured
   unsigned short wMaxPacketSize;       //!< Maximum packet size this endpoint
   unsigned char  bInterval;            //!< Interval for polling endpoint for data transfers
} S_usb_endpoint_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char  bLength;              //!< Size of this descriptor in bytes
   unsigned char  bDescriptorType;      //!< STRING descriptor type
   unsigned short wLANGID;              //!< LANGID code zero
} S_usb_language_id;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bFunctionLength;       //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned char bDescriptorSubtype;    //! Header functional descriptor subtype
   unsigned short bcdCDC;               //! USB CDC specification release version
} S_cdc_header_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bFunctionLength;       //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned char bDescriptorSubtype;    //! Call management functional descriptor subtype
   unsigned char bmCapabilities;        //! The capabilities that this configuration supports
   unsigned char bDataInterface;        //! Interface number of the data class interface used for call management
} S_cdc_call_management_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bFunctionLength;       //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned char bDescriptorSubtype;    //! Abstract control management functional descriptor subtype
   unsigned char bmCapabilities;        //! Capabilities supported by this configuration
} S_cdc_abstract_control_management_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bFunctionLength;       //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned char bDescriptorSubtype;    //! Union functional descriptor subtype
   unsigned char bMasterInterface;      //! The interface number designated as master
} S_cdc_union_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   S_cdc_union_descriptor sUnion;       //! Union functional descriptor
   unsigned char bSlaveInterfaces[1];   //! Slave interface 0
} S_cdc_union_1slave_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bLength;               //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned char bFirstInterface;       
   unsigned char bInterfaceCount; 
   unsigned char bFunctionClass; 
   unsigned char bFunctionSubClass; 
   unsigned char bFunctionProtocol; 
   unsigned char iFunction; 
} S_iad_descriptor;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
   unsigned char bLength;               //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned short bcdHID;               //! bcdHID
   unsigned char bCountryCode;          //! bCountryCode
   unsigned char bNumDescriptors;       //! bNumDescriptors
   unsigned char bHidType;              //! bHidType
   unsigned short wDescriptorLength;    //! wDescriptorLength
} S_hid_descriptor;

typedef struct __attribute__ ((packed)) {
   unsigned char bLength;               //! Size of this descriptor in bytes
   unsigned char bDescriptorType;       //! CS_INTERFACE descriptor type
   unsigned short bcdUSB;               //!< USB specification release number
   unsigned char  bDeviceClass;         //!< Class code
   unsigned char  bDeviceSubClass;      //!< Subclass code
   unsigned char  bDeviceProtocol;      //!< Protocol code
   unsigned char  bMaxPacketSize0;      //!< Control endpoint 0 max. packet size
   unsigned char  bNumConfig;           //!< Number of possible configurations
   unsigned char  bReserved;            //!< Reserved
} S_device_qualifier;
//*----------------------------------------------------------------------------
typedef struct __attribute__ ((packed)) {
    S_usb_configuration_descriptor               sCfg;
   
    S_usb_interface_descriptor                   sHid;
    S_hid_descriptor                             sHDesc; 
    S_usb_endpoint_descriptor                    sHidOut;
    S_usb_endpoint_descriptor                    sHidIn;
    
} S_ser_configuration_descriptor;
//*----------------------------------------------------------------------------
// EXTERNALS
//*----------------------------------------------------------------------------
extern const char *StrDesc[];
//*----------------------------------------------------------------------------
extern const S_usb_device_descriptor        DevDesc;
extern const S_ser_configuration_descriptor ConfDesc;
//*----------------------------------------------------------------------------
extern const uint8_t  HID_ReportDescriptor[];
extern const uint32_t HID_ReportDescSize;
//*----------------------------------------------------------------------------
#endif

