import board
import busio
from adafruit_bme280 import basic as adafruit_bme280

i2c = busio.I2C(board.SCL, board.SDA)
sensor = adafruit_bme280.Adafruit_BME280_I2C(i2c, address=0x76)

print(f"Temperature: {sensor.temperature:.2f} °C")
print(f"Humidity: {sensor.humidity:.2f} %")
print(f"Pressure: {sensor.pressure:.2f} hPa")

