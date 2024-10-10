#! /usr/bin/python3

import platform
import usb1  # https://pypi.org/project/libusb1/

class vendor_usb:
    VID = 0X04D8 # Microchip Vendor IO, must match the VID on the target device
    PID = 0X0B0A # Prototying Product ID, must match the PID on the target device

    # Interface and Alternate Interface Indexes
    INTERFACE_ENDPOINTS = 0
    INTERFACE_ALTERNATE_BULKINT = 1
    INTERFACE_ALTERNATE_ISO1 = 2
    INTERFACE_ALTERNATE_ISO1023_OUT = 3
    INTERFACE_ALTERNATE_ISO1023_IN = 4

    # Indexes for the custom Vendor Requests
    REQUEST_VENDOR_TEST_CONTROL = 0
    REQUEST_VENDOR_CONFIGURE_BULKINT_ZLP = 1
    REQUEST_VENDOR_CONFIGURE_BULKINT_SIZE = 2

    def __init__(self):
        self.device = None
        self.configuration = None
        self.interfacenumber = None
        self.alternatesetting = None
        
        self.context = usb1.USBContext()

    def connect_device(self):
        device_handle = self.context.openByVendorIDAndProductID(
            self.VID,
            self.PID,
            skip_on_error=True,
        )
        if device_handle:
            self.device = device_handle
            self.device.setConfiguration(1)
            self.set_interface(0,0)
            self.configuration = self.device.getConfiguration()
            return True
        return False

    def get_endpoints(self, type=None, direction=None, confignumber=None, interfacenumber=None, alternatesetting=None):
        endpoints = []

        device = self.device.getDevice()
        for config in device.iterConfigurations():
            if confignumber == None or config.getConfigurationValue() == confignumber:
                for interface in config.iterInterfaces():
                    for alternateint in interface.iterSettings():
                        if interfacenumber == None or alternateint.getNumber() == interfacenumber:
                            if alternatesetting == None or alternateint.getAlternateSetting() == alternatesetting:
                                for endpoint in alternateint.iterEndpoints():
                                    attributes = endpoint.getAttributes()
                                    if type == None or attributes & usb1.libusb1.LIBUSB_TRANSFER_TYPE_MASK == type:
                                        if direction == None or endpoint.getAddress() & usb1.libusb1.LIBUSB_ENDPOINT_DIR_MASK == direction:
                                            endpoints.append(dict(
                                                type = endpoint.getAttributes(),
                                                address = endpoint.getAddress(),
                                                size = endpoint.getMaxPacketSize(),
                                                interface =  alternateint.getNumber(),
                                                alternate = alternateint.getAlternateSetting(),
                                                configuration = config.getConfigurationValue(),
                                            ))
        return endpoints

    def print_descriptors(self):
        deviceinfo = self.device.getDevice()
        
        print("VendorID: 0x{:04X}".format(deviceinfo.getVendorID()))
        print("ProductID: 0x{:04X}".format(deviceinfo.getProductID()))

        langIDs = self.device.getSupportedLanguageList()
        if len(langIDs) > 0:
            try:
            	# Unknow failure when using these functions
                print("Manufacturer:", self.device.getManufacturer())
                print("Product:", self.device.getProduct())
                print("Serial:", self.device.getSerialNumber())
            except usb1.USBErrorIO:
            	# Backup functions work
                print("Manufacturer:", self.device.getStringDescriptor(1, langIDs[0], errors = 'ignore'))
                print("Product:", self.device.getStringDescriptor(2, langIDs[0], errors = 'ignore'))
                print("Serial:", self.device.getStringDescriptor(3, langIDs[0], errors = 'ignore'))
        else:
            print("No supported language ids, so not expecting any strings")

    def set_interface(self, setting, alternate=0):
        if setting != self.interfacenumber:
            print("Claiming interface {}".format(setting))
            if self.interfacenumber:
                self.device.releaseInterface(self.interfacenumber)
            self.device.claimInterface(setting)
        if setting != self.interfacenumber or alternate != self.alternatesetting:
            print("Switching to interface {}, alternate number {}".format(setting, alternate))
            self.device.setInterfaceAltSetting(interface = setting, alt_setting = alternate)
        self.interfacenumber = setting
        self.alternatesetting = alternate

    def submit_wait_blocking(self, transfer):
        try:
            transfer.submit()
        except:
            raise Exception("Check that the driver is libusbK, it is need for isochronous endpoints")
        while transfer.isSubmitted():
            try:
                self.context.handleEvents()
            except usb1.USBErrorInterrupted:
                pass

if __name__ == '__main__':
    test = vendor_usb()
    if test.connect_device():
        print ("Valid vendor device connected, printing descriptors\n")
        test.print_descriptors()
        endpoints = test.get_endpoints()
        for idx,endpoint in enumerate(endpoints):
            print("Endpoint {}/{}: {}".format(idx, len(endpoints), endpoint))

    else:
        print ("No valid vendor device connected")