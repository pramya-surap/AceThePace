#Install pyserial in pip

import serial
import time

#Once .ino is uploaded, pyserial talks with the board through COM3

# "COMX" on Windows, "/dev/ttyACM0" on Mac
PORT = "COM4"
BAUD = 9600

try:
    with serial.Serial(PORT, BAUD, timeout=2) as ser:
        print(f"Connected to {PORT} at {BAUD} baud.")
        time.sleep(2)  # wait for Arduino to reset

        # Flush any initial text from Arduino
        ser.reset_input_buffer()

        while True:
            value = input("Enter brightness (0–255, or 'q' to quit): ").strip()
            if value.lower() == 'q':
                break
            try:
                num = int(value)
                if 0 <= num <= 255:
                    ser.write(f"{num}\n".encode())
                    ser.flush()
                    # read Arduino response
                    line = ser.readline().decode(errors='ignore').strip()
                    if line:
                        print("Arduino says:", line)
                else:
                    print("Please enter a number between 0 and 255.")
            except ValueError:
                print("Invalid input. Enter an integer 0–255 or 'q'.")
except serial.SerialException as e:
    print(f"Serial error: {e}")
