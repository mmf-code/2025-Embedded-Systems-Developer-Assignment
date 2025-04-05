from smbus2 import SMBus

address = 0x76

try:
    with SMBus(1) as bus:
        chip_id = bus.read_byte_data(address, 0xD0)
        print(f"Chip ID: 0x{chip_id:X}")
except Exception as e:
    print(f"I2C read error: {e}")
