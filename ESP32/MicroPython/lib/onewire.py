# OneWire driver for pycom boards
# Source: https://github.com/pycom/pycom-libraries/blob/master/examples/DS18X20/onewire.py
# Licensed under GPLv3

import time
import machine

class OneWire:
    CMD_SEARCHROM = const(0xf0)
    CMD_READROM = const(0x33)
    CMD_MATCHROM = const(0x55)
    CMD_SKIPROM = const(0xcc)

    def __init__(self, pin):
        self.pin = pin
        self.pin.init(pin.OPEN_DRAIN, pin.PULL_UP)

    def reset(self):
        """
        Perform the onewire reset function.
        Returns True if a device asserted a presence pulse, False otherwise.
        """
        sleep_us = time.sleep_us
        disable_irq = machine.disable_irq
        enable_irq = machine.enable_irq
        pin = self.pin

        pin(0)
        sleep_us(480)
        i = disable_irq()
        pin(1)
        sleep_us(60)
        status = not pin()
        enable_irq(i)
        sleep_us(420)
        return status

    def read_bit(self):
        sleep_us = time.sleep_us
        enable_irq = machine.enable_irq
        pin = self.pin

        pin(1) # half of the devices don't match CRC without this line
        i = machine.disable_irq()
        pin(0)
        sleep_us(1)
        pin(1)
        sleep_us(1)
        value = pin()
        enable_irq(i)
        sleep_us(40)
        return value

    def read_byte(self):
        value = 0
        for i in range(8):
            value |= self.read_bit() << i
        return value

    def read_bytes(self, count):
        buf = bytearray(count)
        for i in range(count):
            buf[i] = self.read_byte()
        return buf

    def write_bit(self, value):
        sleep_us = time.sleep_us
        pin = self.pin

        i = machine.disable_irq()
        pin(0)
        sleep_us(1)
        pin(value)
        sleep_us(60)
        pin(1)
        sleep_us(1)
        machine.enable_irq(i)

    def write_byte(self, value):
        for i in range(8):
            self.write_bit(value & 1)
            value >>= 1

    def write_bytes(self, buf):
        for b in buf:
            self.write_byte(b)

    def select_rom(self, rom):
        """
        Select a specific device to talk to. Pass in rom as a bytearray (8 bytes).
        """
        self.reset()
        self.write_byte(CMD_MATCHROM)
        self.write_bytes(rom)

    def crc8(self, data):
        """
        Compute CRC
        """
        crc = 0
        for i in range(len(data)):
            byte = data[i]
            for b in range(8):
                fb_bit = (crc ^ byte) & 0x01
                if fb_bit == 0x01:
                    crc = crc ^ 0x18
                crc = (crc >> 1) & 0x7f
                if fb_bit == 0x01:
                    crc = crc | 0x80
                byte = byte >> 1
        return crc

    def scan(self):
        """
        Return a list of ROMs for all attached devices.
        Each ROM is returned as a bytes object of 8 bytes.
        """
        devices = []
        diff = 65
        rom = False
        for i in range(0xff):
            rom, diff = self._search_rom(rom, diff)
            if rom:
                devices += [rom]
            if diff == 0:
                break
        return devices

    def _search_rom(self, l_rom, diff):
        if not self.reset():
            return None, 0
        self.write_byte(CMD_SEARCHROM)
        if not l_rom:
            l_rom = bytearray(8)
        rom = bytearray(8)
        next_diff = 0
        i = 64
        for byte in range(8):
            r_b = 0
            for bit in range(8):
                b = self.read_bit()
                if self.read_bit():
                    if b: # there are no devices or there is an error on the bus
                        return None, 0
                else:
                    if not b: # collision, two devices with different bit meaning
                        if diff > i or ((l_rom[byte] & (1 << bit)) and diff != i):
                            b = 1
                            next_diff = i
                self.write_bit(b)
                if b:
                    r_b |= 1 << bit
                i -= 1
            rom[byte] = r_b
        return rom, next_diff
