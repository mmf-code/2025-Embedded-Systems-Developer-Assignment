import time
from smbus2 import SMBus

bus = SMBus(1)
BME280_ADDR = 0x76

# Kalibrasyon verilerini ve register adreslerini okuma gibi işlemler burada yapılmalı
# Fakat test için sadece "başlat" ve "raw data" okuma yapacağız

# Sensörü başlat (ctrl_hum / ctrl_meas / config register'larını yaz)
bus.write_byte_data(BME280_ADDR, 0xF2, 0x01)  # ctrl_hum (1x oversampling)
bus.write_byte_data(BME280_ADDR, 0xF4, 0x27)  # ctrl_meas (1x oversampling, normal mode)
bus.write_byte_data(BME280_ADDR, 0xF5, 0xA0)  # config (standby 1000ms)

time.sleep(1)

try:
    # Chip ID
    chip_id = bus.read_byte_data(BME280_ADDR, 0xD0)
    print(f"Chip ID: 0x{chip_id:X}")

    # Basit test: temp MSB oku
    temp_msb = bus.read_byte_data(BME280_ADDR, 0xFA)
    print(f"Raw Temp MSB: {temp_msb}")

except Exception as e:
    print(f"Hata: {e}")
