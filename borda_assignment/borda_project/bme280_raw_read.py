import time
from smbus2 import SMBus

bus = SMBus(1)
BME280_ADDR = 0x76

# Kalibrasyon verilerini oku
def read_calibration_data():
    calib = {}

    # Sıcaklık kalibrasyon parametreleri
    calib['T1'] = bus.read_word_data(BME280_ADDR, 0x88)
    calib['T2'] = bus.read_word_data(BME280_ADDR, 0x8A)
    calib['T3'] = bus.read_word_data(BME280_ADDR, 0x8C)

    # İşaretli sayılara çevir
    if calib['T2'] > 32767:
        calib['T2'] -= 65536
    if calib['T3'] > 32767:
        calib['T3'] -= 65536

    return calib

def compensate_temperature(adc_T, calib):
    var1 = (((adc_T >> 3) - (calib['T1'] << 1)) * calib['T2']) >> 11
    var2 = (((((adc_T >> 4) - calib['T1']) * ((adc_T >> 4) - calib['T1'])) >> 12) * calib['T3']) >> 14
    t_fine = var1 + var2
    temp = (t_fine * 5 + 128) >> 8
    return temp / 100.0

# Sensörü başlat
def setup_sensor():
    bus.write_byte_data(BME280_ADDR, 0xF2, 0x01)  # ctrl_hum
    bus.write_byte_data(BME280_ADDR, 0xF4, 0x27)  # ctrl_meas
    bus.write_byte_data(BME280_ADDR, 0xF5, 0xA0)  # config

# Ana okuma döngüsü
def main():
    setup_sensor()
    calib = read_calibration_data()
    print("BME280 sıcaklık okuma başlatıldı...\n")

    while True:
        data = bus.read_i2c_block_data(BME280_ADDR, 0xF7, 8)
        adc_T = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4)

        temp_c = compensate_temperature(adc_T, calib)
        print(f"Sıcaklık: {temp_c:.2f} °C")

        time.sleep(1)

if __name__ == '__main__':
    main()
