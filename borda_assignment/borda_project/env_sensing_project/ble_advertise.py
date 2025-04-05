#!/usr/bin/env python3

import dbus
import dbus.mainloop.glib
import dbus.service
from gi.repository import GLib
import signal

BLUEZ_SERVICE_NAME = 'org.bluez'
ADAPTER_IFACE = 'org.bluez.LEAdvertisingManager1'
PAYLOAD_FILE = 'payload.bin'
READ_INTERVAL_SEC = 30
EXPECTED_PAYLOAD_LEN = 27  # Counter(1) + Timestamp(2) + 3 sensors * 4 stats * 2 bytes = 27

class Advertisement(dbus.service.Object):
    def __init__(self, bus, index):
        self.bus = bus
        self.path = f"/org/bluez/example/advertisement{index}"
        self.payload = self.read_payload()
        super().__init__(bus, self.path)

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def read_payload(self):
        """
        Reads the payload.bin file and parses 27-byte BLE data.
        Displays decoded statistics in the terminal for verification.
        """
        try:
            with open(PAYLOAD_FILE, "rb") as f:
                data = f.read()
                if len(data) != EXPECTED_PAYLOAD_LEN:
                    raise ValueError(
                        f"Payload must be {EXPECTED_PAYLOAD_LEN} bytes! Got: {len(data)} bytes"
                    )

                counter = data[0]
                timestamp = data[1] + (data[2] << 8)

                print(f"\n🔄 New Payload (Counter={counter} | Timestamp={timestamp}s)")

                # Parse 3 sensors × 4 stats (each 2 bytes)
                for i in range(3):
                    base = 3 + i * 8
                    std   = data[base]   + (data[base+1] << 8)
                    max_v = data[base+2] + (data[base+3] << 8)
                    min_v = data[base+4] + (data[base+5] << 8)
                    med   = data[base+6] + (data[base+7] << 8)

                    if i == 0:
                        name = "🌡️  Temperature"
                        scale = 100.0
                    elif i == 1:
                        name = "💧 Humidity"
                        scale = 100.0
                    else:
                        name = "🫁 CO₂"
                        scale = 1.0

                    print(f"{name}: Std={std/scale:.2f}, "
                          f"Max={max_v/scale:.2f}, Min={min_v/scale:.2f}, "
                          f"Med={med/scale:.2f}")

                return list(data)

        except Exception as e:
            print(f"⚠️ Failed to read payload.bin: {e}")
            return [0] * EXPECTED_PAYLOAD_LEN

    def update_payload(self):
        """
        Called periodically to refresh BLE payload from file.
        """
        self.payload = self.read_payload()

    @dbus.service.method('org.freedesktop.DBus.Properties',
                         in_signature='s', out_signature='a{sv}')
    def GetAll(self, interface):
        """
        Method required by BlueZ to provide advertising data.
        Sets ManufacturerData field to our sensor payload.
        """
        print(f"[GetAll] ManufacturerData: {self.payload}")

        manuf_data = dbus.Dictionary({
            dbus.UInt16(0xFFFF): dbus.Array(self.payload, signature='y')
        }, signature='qv')

        return {
            'Type': dbus.String('peripheral'),
            'LocalName': dbus.String('RPi'),
            'ManufacturerData': manuf_data
        }

    @dbus.service.method('org.freedesktop.DBus.Introspectable',
                         in_signature='', out_signature='s')
    def Introspect(self):
        return ''

    @dbus.service.method('org.bluez.LEAdvertisement1', in_signature='', out_signature='')
    def Release(self):
        print("🔕 Advertisement released")

def register_advertisement(bus, adv):
    """
    Registers the BLE advertisement object with BlueZ.
    """
    adapter_path = '/org/bluez/hci0'
    adapter = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter_path),
                             ADAPTER_IFACE)

    adapter.RegisterAdvertisement(
        adv.get_path(),
        {},
        reply_handler=lambda: print("📡 BLE advertisement active!"),
        error_handler=lambda e: print(f"❌ Error: {e}")
    )
    return adapter

def main():
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SystemBus()

    adv = Advertisement(bus, 0)
    adapter = register_advertisement(bus, adv)

    loop = GLib.MainLoop()

    def refresh_advertisement():
        adv.update_payload()
        return True

    GLib.timeout_add_seconds(READ_INTERVAL_SEC, refresh_advertisement)

    def handle_exit(sig, frame):
        print("\n🔌 Shutting down BLE advertising...")
        try:
            adapter.UnregisterAdvertisement(adv.get_path())
            adv.Release()
        except Exception as e:
            print(f"⚠️ Shutdown error: {e}")
        loop.quit()

    signal.signal(signal.SIGINT, handle_exit)

    try:
        loop.run()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main()
