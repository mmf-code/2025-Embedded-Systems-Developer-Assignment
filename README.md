# 🌱 Environmental Sensing System with BLE Broadcasting (Raspberry Pi 5)

This project is part of the **Borda Academy 2025 Embedded Systems Developer Assignment**. It demonstrates an environmental monitoring system using multiple I²C sensors on a Raspberry Pi 5. The system periodically collects environmental data, applies filtering, performs statistical analysis, and advertises results over Bluetooth Low Energy (BLE).

---

## 📆 Project Structure

```
env_sensing_project/
├── include/                # Header files
├── src/                    # C source code
├── report/                 # Datasheets and docs
├── test/                   # (Optional) Unit tests
├── ble_advertise.py        # BLE Advertisement via D-Bus
├── env_sensor              # Compiled C binary
├── payload.bin             # Binary payload for BLE
├── Makefile                # Build instructions
├── flowchart.png           # System architecture
└── README.md               # You're reading it!

bonus_part/
├── rtos_bonus.c            # Producer/Consumer threads with sync
├── slow_consumer.c         # Overflow simulation
├── circular_buffer.c/h     # Shared buffer logic
├── buffer_overflow.log     # Overflow logs
├── Makefile
```

---

## ✅ Features

- ✅ I²C communication with 3 environmental sensors  
- ✅ Median filter (moving window) for noise reduction  
- ✅ Circular buffer to store filtered data  
- ✅ Computation of mean, standard deviation, min, max, median  
- ✅ BLE advertisement every 30 seconds using `BlueZ` D-Bus API  
- ✅ Real-time terminal outputs  
- ✅ Python script for BLE broadcasting via `payload.bin`  
- ✅ Bonus: Thread synchronization + overflow logging  

---

## 📊 Sensors Used

| Sensor        | Type              | I²C Address | Status         |
|---------------|-------------------|-------------|----------------|
| BME280        | Temperature        | `0x76`      | Physical       |
| Simulated Hum | Humidity           | `0x76`      | Simulated      |
| Simulated CO₂ | CO2 Concentration  | `0x5A`      | Simulated      |

📌 Sensor datasheets are provided in `report/datasheets/`.

---

## 🚀 How It Works

### 1. Measurement Loop (C - `main.c`)
- Every **1 second**, temperature is read from the real BME280 sensor.
- Humidity and CO₂ are simulated using random values.
- Median filter is applied (window size: 5).
- Filtered values are pushed into circular buffers (size: 50).
- Every **30 seconds**, statistics are computed and written to `payload.bin`.

### 2. BLE Advertisement (Python - `ble_advertise.py`)
- Reads `payload.bin` every 30 seconds.
- Parses statistics for each sensor.
- Sends BLE advertisement using `BlueZ` D-Bus API.
- Displays parsed data in the terminal.

---

## 🔧 Setup & Build

```bash
# Clone project and enter directory
cd env_sensing_project

# Build the C project
make

# Run the sensor logic
./env_sensor
```

In a second terminal:

```bash
# Make BLE script executable
chmod +x ble_advertise.py

# Run BLE advertisement
sudo ./ble_advertise.py
```

---

## 📱 BLE Payload Format (27 bytes)

| Byte Index | Description                            |
|------------|----------------------------------------|
| 0          | Counter (1 byte)                       |
| 1–2        | Timestamp (2 bytes, seconds)           |
| 3–26       | 3 Sensors × 4 stats (std, max, min, med), each 2 bytes |

---

## 📁 Bonus Part (Located in `/bonus_part`)

### 🧅 2.a – RTOS-like Producer/Consumer Threads

| Subtask     | Description                                                                 | Code File       |
|-------------|-----------------------------------------------------------------------------|-----------------|
| **2.a.i**   | Producer waits for buffer space, writes data, signals consumer             | `rtos_bonus.c`  |
| **2.a.ii**  | Consumer waits for data, reads from buffer, signals producer                | `rtos_bonus.c`  |
| **2.a.iii** | Simulated data filtering via delay (usleep)                                 | `rtos_bonus.c`  |

### 🧠 2.b – Race Condition Prevention
- **Technique**: Mutex and Condition Variables (POSIX threads)
- **Result**: No data corruption or race conditions observed.

### ⚠️ 2.c – Overflow Case (Consumer Slower Than Producer)

| Condition        | Behavior                                                           |
|------------------|--------------------------------------------------------------------|
| Consumer delayed | Buffer fills up                                                    |
| Overflow         | New data dropped, logged to `buffer_overflow.log`                 |

```bash
# Compile and run overflow simulation
cd bonus_part
make slow_consumer
./slow_consumer

# View log of dropped data
cat buffer_overflow.log
```

📝 *This simulates a real-world scenario where incoming data frequency exceeds processing speed.*

---

## 📷 Screenshots

> Include terminal outputs and BLE scanner screenshots here (e.g., nRF Connect)

---

## 🧠 Skills Demonstrated

- RTOS-style multithreading with mutex/condition variable (Bonus Part)
- Embedded C design principles
- Sensor communication (I²C)
- Data filtering and statistics
- BLE packet structuring and D-Bus advertisement
- Python/C integration for embedded systems

---

## 📌 Requirements Met

✔ 1 Hz sampling  
✔ Circular buffer handling with overflow protection  
✔ BLE packets every 30 sec  
✔ Sensor data aggregation and structured advertisement  
✔ Bonus: Overflow handling & RTOS-style architecture

---

## 👤 Author

**Atakan Yaman**  
Istanbul Technical University  
Embedded Systems & IoT Enthusiast

---

## 📄 License

MIT License

Copyright (c) 2025 Atakan Yaman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

