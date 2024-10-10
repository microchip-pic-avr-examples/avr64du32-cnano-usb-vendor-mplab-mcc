#! /usr/bin/python3

try:
    import sys
    from time import sleep
    import random
    import argparse
    import array
    import usb1  # https://pypi.org/project/libusb1/
    from vendor_usb import vendor_usb
except ImportError:
    sys.exit("""ImportError: You are probably missing some modules.
To add needed modules, run 'python -m pip install -U libusb1'""")


class usb_vendor_test_endpoints:
    # Size of buffers used for the loopback
    VENDOR_LOOPBACK_SIZE = 4096
    ISO_NB_TRANS = (VENDOR_LOOPBACK_SIZE*2)
    VENDOR_LOOPBACK_REPEAT = 2

    def __init__(self):
        self.vendor_ep_interrupt_in = None
        self.vendor_ep_interrupt_out = None
        self.vendor_ep_interrupt_alt = None

        self.vendor_ep_bulk_in = None
        self.vendor_ep_bulk_out = None
        self.vendor_ep_bulk_alt = None

        self.vendor_ep_iso_in = None
        self.vendor_ep_iso_out = None
        self.vendor_ep_iso_alt = None
        self.vendor_ep_iso_size = None

        self.vendor_ep_iso2_in = None
        self.vendor_ep_iso2_out = None
        self.vendor_ep_iso2_out_alt = None
        self.vendor_ep_iso2_in_alt = None
        self.vendor_ep_iso2_size = None

        self.vendor = vendor_usb()
        self.vendor.connect_device()
        

    def main(self):
        # Get descriptors and endpoints
        self.get_device_data()
        self.run_tests()

    def get_device_data(self):
        self.vendor.print_descriptors()

        interrupt_endpoints = self.vendor.get_endpoints(
            type=usb1.libusb1.LIBUSB_TRANSFER_TYPE_INTERRUPT)
        bulk_endpoints = self.vendor.get_endpoints(
            type=usb1.libusb1.LIBUSB_TRANSFER_TYPE_BULK)
        iso_endpoints = self.vendor.get_endpoints(
            type=usb1.libusb1.LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)

        if len(interrupt_endpoints):
            for endpoint in interrupt_endpoints:
                print(
                    "  - Interface {0} alternate {1}:".format(endpoint['interface'], endpoint['alternate']))
                self.vendor_ep_interrupt_alt = endpoint['alternate']
                if (endpoint['address'] & usb1.libusb1.LIBUSB_ENDPOINT_IN):
                    self.vendor_ep_interrupt_in = endpoint['address']
                    print("  - Endpoint interrupt IN:  0x%02X" %
                          endpoint['address'])
                else:
                    self.vendor_ep_interrupt_out = endpoint['address']
                    print("  - Endpoint interrupt OUT: 0x%02X" %
                          endpoint['address'])

        if len(bulk_endpoints):
            for endpoint in bulk_endpoints:
                print(
                    "  - Interface {0} alternate {1}:".format(endpoint['interface'], endpoint['alternate']))
                self.vendor_ep_bulk_alt = endpoint['alternate']
                if (endpoint['address'] & usb1.libusb1.LIBUSB_ENDPOINT_IN):
                    self.vendor_ep_bulk_in = endpoint['address']
                    print("  - Endpoint bulk      IN:  0x%02X" %
                        endpoint['address'])
                else:
                    self.vendor_ep_bulk_out = endpoint['address']
                    print("  - Endpoint bulk      OUT: 0x%02X" %
                        endpoint['address'])

        if len(iso_endpoints):
            for endpoint in iso_endpoints:
                print(
                    "  - Interface {0} alternate {1}:".format(endpoint['interface'], endpoint['alternate']))
                if endpoint['alternate'] == 2:
                    self.vendor_ep_iso_alt = endpoint['alternate']
                    self.vendor_ep_iso_size = endpoint['size']
                    if (endpoint['address'] & usb1.libusb1.LIBUSB_ENDPOINT_IN):
                        self.vendor_ep_iso_in = endpoint['address']
                        print("  - Endpoint iso 1     IN:  0x%02X" %
                              endpoint['address'])
                    else:
                        self.vendor_ep_iso_out = endpoint['address']
                        print("  - Endpoint iso 1     OUT: 0x%02X" %
                              endpoint['address'])
                    print("  - Endpoint iso 1     size: %i" % endpoint['size'])
                else:
                    self.vendor_ep_iso2_size = endpoint['size']
                    if (endpoint['address'] & usb1.libusb1.LIBUSB_ENDPOINT_IN):
                        self.vendor_ep_iso2_in_alt = endpoint['alternate']
                        self.vendor_ep_iso2_in = endpoint['address']
                        print("  - Endpoint iso 2     IN:  0x%02X" %
                              endpoint['address'])
                    else:
                        self.vendor_ep_iso2_out_alt = endpoint['alternate']
                        self.vendor_ep_iso2_out = endpoint['address']
                        print("  - Endpoint iso 2     OUT: 0x%02X" %
                              endpoint['address'])
                    print("  - Endpoint iso 2     size:%i" % endpoint['size'])

    def run_tests(self):
        print("\n------ Starting Vendor Data Transfers")

        try:
            print("Run Control endpoint loop back...")
            self.init_buffers()
            self.loop_back_control(self.VENDOR_LOOPBACK_SIZE)
            self.init_buffers()
            self.loop_back_control(self.VENDOR_LOOPBACK_SIZE-123)
        
            # Send configure requests before changing to the alternate interface

            # Configure azlp using the control endpoint
            self.vendor.device.controlWrite(usb1.libusb1.LIBUSB_REQUEST_TYPE_VENDOR |
                                            usb1.libusb1.LIBUSB_RECIPIENT_INTERFACE, 0, args.azlp, self.vendor.REQUEST_VENDOR_CONFIGURE_BULKINT_ZLP, [], timeout=1000)

            # Configure the bulk and interrupt transfer size
            self.vendor.device.controlWrite(usb1.libusb1.LIBUSB_REQUEST_TYPE_VENDOR |
                                            usb1.libusb1.LIBUSB_RECIPIENT_INTERFACE, 0, self.VENDOR_LOOPBACK_SIZE, self.vendor.REQUEST_VENDOR_CONFIGURE_BULKINT_SIZE, [], timeout=1000)

            # reclaim the interface, not sure why
            self.vendor.device.claimInterface(self.vendor.INTERFACE_ENDPOINTS)

            if (self.vendor_ep_interrupt_in and self.vendor_ep_interrupt_out):
                self.vendor.set_interface(
                    self.vendor.INTERFACE_ENDPOINTS, self.vendor_ep_interrupt_alt)
                for _ in range(self.VENDOR_LOOPBACK_REPEAT):
                    print("Run Interrupt endpoint loop back...")
                    self.init_buffers()
                    self.loop_back_interrupt()

            if (self.vendor_ep_bulk_in and self.vendor_ep_bulk_out):
                self.vendor.set_interface(
                    self.vendor.INTERFACE_ENDPOINTS, self.vendor_ep_bulk_alt)
                for _ in range(self.VENDOR_LOOPBACK_REPEAT):
                    print("Run Bulk endpoint loop back...")
                    self.init_buffers()
                    self.loop_back_bulk()

            if (self.vendor_ep_iso_in and self.vendor_ep_iso_out):
                self.vendor.set_interface(
                    self.vendor.INTERFACE_ENDPOINTS, self.vendor_ep_iso_alt)
                print("Run Isochronous endpoint loop back simple...")
                self.init_buffers()
                self.loop_back_isochronous_buffer()
                # not sure why we need to sleep between switching back to sending more data?
                sleep(1/3)
                print("Run Isochronous endpoint loop back extended...")
                self.init_buffers()
                self.loop_back_isochronous_packets()

            if (self.vendor_ep_iso2_in and self.vendor_ep_iso2_out):
                for _ in range(self.VENDOR_LOOPBACK_REPEAT):
                    print("Run Isochronous endpoint loop back in special iso 1023 way...")
                    self.init_buffers()
                    self.loop_back_isochronous_buffer_special_1023()

            print(f"\n------ Data Transfers Completed Successfully")
            return

        except ValueError as e:
            print(f"Error: {e}")
        except usb1.USBError:
            print(f"USB Error: Endpoints are not configured correctly.")
            print(f"Solution: Upload correct c-code to the AVR decive.")
        except Exception as e:
            print(f"Unexpected error: {e}")

        print(f"\n------ Data Transfers Failed")


    def init_buffers(self):
        # Reset buffer OUT
        self.vendor_buf_out = array.array('B', (0,)*self.VENDOR_LOOPBACK_SIZE)
        for i in range(0, self.VENDOR_LOOPBACK_SIZE, 4):
            self.vendor_buf_out[i+0] = (i >> 24) & 0xFF
            self.vendor_buf_out[i+1] = (i >> 16) & 0xFF
            self.vendor_buf_out[i+2] = (i >> 8) & 0xFF
            self.vendor_buf_out[i+3] = (i >> 0) & 0xFF
            if (i % 32) == 4:
                self.vendor_buf_out[i] = random.randrange(0xFF)

        # Reset buffer IN
        self.vendor_buf_in = array.array('B')

    def cmp_buffers(self):
        if self.vendor_buf_in != self.vendor_buf_out:
            raise ValueError("vendor_buf_in != vendor_buf_out")
        else:
            print("Successfully sent and received {} bytes!".format(
                len(self.vendor_buf_in)))

    def loop_back_control(self, bytes):
        try:
            self.vendor_buf_out = self.vendor_buf_out[:bytes]
            self.vendor.device.controlWrite(usb1.libusb1.LIBUSB_REQUEST_TYPE_VENDOR |
                                            usb1.libusb1.LIBUSB_RECIPIENT_INTERFACE, 0, self.vendor.REQUEST_VENDOR_TEST_CONTROL, 0, self.vendor_buf_out, timeout=1000)
            self.vendor_buf_in = self.vendor.device.controlRead(
                usb1.libusb1.LIBUSB_REQUEST_TYPE_VENDOR | usb1.libusb1.LIBUSB_RECIPIENT_INTERFACE, 0, self.vendor.REQUEST_VENDOR_TEST_CONTROL, 0, bytes, timeout=1000)
            self.cmp_buffers()
        except ValueError:
            raise ValueError("Error during control endpoint transfer")

    def loop_back_interrupt(self):
        try:
            self.vendor.device.interruptWrite(
                self.vendor_ep_interrupt_out, self.vendor_buf_out)
            if args.azlp:
                # send zlp
                self.vendor.device.interruptWrite(
                    self.vendor_ep_interrupt_out, [])
                # read back one more byte to get zlp
                self.vendor_buf_in = self.vendor.device.interruptRead(
                    self.vendor_ep_interrupt_in, self.VENDOR_LOOPBACK_SIZE+1)
            else:
                self.vendor_buf_in = self.vendor.device.interruptRead(
                    self.vendor_ep_interrupt_in, self.VENDOR_LOOPBACK_SIZE)

            self.cmp_buffers()
        except ValueError:
            raise ValueError("Error during interrupt endpoint transfer")

    def loop_back_bulk(self):
        try:
            self.vendor.device.bulkWrite(
                self.vendor_ep_bulk_out, self.vendor_buf_out)
            if args.azlp:
                # send zlp
                self.vendor.device.bulkWrite(self.vendor_ep_bulk_out, [])
                # read back one more byte to get zlp
                self.vendor_buf_in = self.vendor.device.bulkRead(
                    self.vendor_ep_bulk_in, self.VENDOR_LOOPBACK_SIZE+1)
            else:
                self.vendor_buf_in = self.vendor.device.bulkRead(
                    self.vendor_ep_bulk_in, self.VENDOR_LOOPBACK_SIZE)

            self.cmp_buffers()
        except ValueError:
            raise ValueError("Error during bulk endpoint transfer")

    def loop_back_isochronous_buffer(self):
        try:
            transfer = self.vendor.device.getTransfer(
                int(self.VENDOR_LOOPBACK_SIZE/self.vendor_ep_iso_size))

            transfer.setIsochronous(
                self.vendor_ep_iso_out, self.vendor_buf_out, timeout=5000)
            self.vendor.submit_wait_blocking(transfer)

            transfer.setIsochronous(
                self.vendor_ep_iso_in, self.VENDOR_LOOPBACK_SIZE, timeout=5000)
            self.vendor.submit_wait_blocking(transfer)

            self.vendor_buf_in = array.array('B')
            self.vendor_buf_in.frombytes(transfer.getBuffer())

            self.cmp_buffers()
        except ValueError:
            raise ValueError("Error during simple iso endpoint transfer")

    def loop_back_isochronous_packets(self):
        # Reset extra large buffer OUT
        self.vendor_buf_out = array.array('B', (0,)*self.ISO_NB_TRANS)
        for i in range(0, self.ISO_NB_TRANS, 4):
            self.vendor_buf_out[i+0] = ord('S')
            self.vendor_buf_out[i+1] = (i >> 16) & 0xFF
            self.vendor_buf_out[i+2] = (i >> 8) & 0xFF
            self.vendor_buf_out[i+3] = (i >> 0) & 0xFF

        # Reset buffer IN
        self.vendor_buf_in = array.array('B')
        iso_packet_in = array.array('B')

        packets = min(64, int(self.ISO_NB_TRANS/self.vendor_ep_iso_size))
        print("...using {0} packets of endpoint size {1}".format(packets,
                                                                 self.vendor_ep_iso_size))

        try:

            transfer = self.vendor.device.getTransfer(1)
            count = 0
            for i in range(packets):
                count += 1

                # Sending vendor_ep_iso_size bytes to device
                iso_packet_out = self.vendor_buf_out[i*int(
                    self.vendor_ep_iso_size):(i+1)*int(self.vendor_ep_iso_size)]
                transfer.setIsochronous(
                    self.vendor_ep_iso_out, iso_packet_out, timeout=5000)
                self.vendor.submit_wait_blocking(transfer)

                transfer.setIsochronous(
                    self.vendor_ep_iso_in, self.vendor_ep_iso_size, timeout=5000)
                self.vendor.submit_wait_blocking(transfer)

                iso_packet_in = array.array('B')
                iso_packet_in.frombytes(transfer.getBuffer())

                if iso_packet_in != iso_packet_out:
                    print("iso_packet %i/%i fail" % (count, packets))
                    print("iso_packet_out[%i] = " %
                          (len(iso_packet_out)), iso_packet_out)
                    print("iso_packet_in[%i] = " % (
                        len(iso_packet_in)), iso_packet_in, flush=True)
                else:
                    print("iso_packet %i/%i ok" % (count, packets), flush=True)
                self.vendor_buf_in += iso_packet_in

                # not sure why we need to sleep between switching back to sending more data?
                sleep(1/3)

            self.cmp_buffers()
        except ValueError:
            raise ValueError("Error during simple iso endpoint transfer")

    def loop_back_isochronous_buffer_special_1023(self):
        # This test is using iso with size 1023 as bulk/interrupt, with workarounds:
        # In and OUT endpoints in separate interfaces to avoid OS driver crashing
        # buffer size%1023==0 to avoid OS driver crashing
        ISO2_TIMEOUT = 0.3

        ISO_SIZE = int(self.VENDOR_LOOPBACK_SIZE /
                       self.vendor_ep_iso2_size)*self.vendor_ep_iso2_size
        self.vendor_buf_out = self.vendor_buf_out[:ISO_SIZE]

        try:
            self.vendor.set_interface(
                setting=self.vendor.INTERFACE_ENDPOINTS, alternate=self.vendor_ep_iso2_out_alt)
            sleep(ISO2_TIMEOUT)

            # Sending VENDOR_LOOPBACK_SIZE bytes to device
            transfer = self.vendor.device.getTransfer(
                int(ISO_SIZE/self.vendor_ep_iso2_size))

            transfer.setIsochronous(
                self.vendor_ep_iso2_out, self.vendor_buf_out, timeout=5000)
            self.vendor.submit_wait_blocking(transfer)

            self.vendor.set_interface(
                setting=self.vendor.INTERFACE_ENDPOINTS, alternate=self.vendor_ep_iso2_in_alt)
            sleep(ISO2_TIMEOUT)

            # Receive VENDOR_LOOPBACK_SIZE from device
            transfer.setIsochronous(
                self.vendor_ep_iso2_in, ISO_SIZE, timeout=5000)
            self.vendor.submit_wait_blocking(transfer)
            self.vendor_buf_in = array.array('B')
            self.vendor_buf_in.frombytes(transfer.getBuffer())

            self.cmp_buffers()

        except ValueError:
            raise ValueError("Error during iso {} endpoint transfer".format(
                self.vendor_ep_iso2_size))


if __name__ == '__main__':
    # Generate help and use messages
    parser = argparse.ArgumentParser(
        description='Python test application for transferring data on all data types',
        epilog='Example: python vendor_endpoints.py',
        formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument(
        '-z', '--azlp', help='Auto Zero-Lenght Package', action='store_true')
    args = parser.parse_args()

    print("\n------ Connecting to AVR device over USB...")

    test = usb_vendor_test_endpoints()

    if test.vendor.device is None:
        print(f"\n------ Failed to Connect Device")
        print(f"Device VID: {vendor_usb.VID:04x}, PID: {vendor_usb.PID:04x} was not found")
        print("1. Check that the curiosity nano is connected to the PC from the USB-C TARGET port (not debugger)")
        print("2. Check if the c-code is programmed correctly on the AVR device")
        print(f"3. Check that the USB driver is installed for the correct device")
        print(f"\t For Windows, download Zadig from: https://zadig.akeo.ie/") 
        print(f"\t Choose the libusbK installer after downloading")
        print(f"\t Select the device with VID: {vendor_usb.VID:04x} and PID: {vendor_usb.PID:04x}\n")
    else:
        print("\n------ Device Connected")
        test.main()
