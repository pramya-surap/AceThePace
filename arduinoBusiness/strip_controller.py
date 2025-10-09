#Install pyserial in pip

import serial
import time
#Once .ino is uploaded, pyserial talks with the board through COM3

# "COMX" on Windows, "/dev/ttyACM0" on Mac
PORT = "COM5"
BAUD = 9600

try:
    with serial.Serial(PORT, BAUD, timeout=2) as ser:
        print(f"Connected to {PORT} at {BAUD} baud.")
        time.sleep(2)  # wait for Arduino to reset

        # Flush any initial text from Arduino
        ser.reset_input_buffer()

        while True:
            try:
                # Get all 3 values from the user
                v1 = input("Enter pace 1 (100-999 ms) or 'q' to quit: ").strip()
                if v1.lower() == 'q':
                    break

                v2 = input("Enter pace 2 (100-999 ms): ").strip()
                v3 = input("Enter pace 3 (100-999 ms): ").strip()

                # Convert to integers and validate
                vals = [int(v1), int(v2), int(v3)]
                if all(100 <= v <= 999 for v in vals):
                    # Format: send all three numbers separated by spaces
                    msg = f"{vals[0]} {vals[1]} {vals[2]}\n"
                    ser.write(msg.encode())
                    ser.flush()

                    # Read Arduino response
                    line = ser.readline().decode(errors='ignore').strip()
                    if line:
                        print("Arduino says:", line)
                else:
                    print("Each value must be between 100 and 999.")
            except ValueError:
                print("Invalid input — enter integers only.")
except serial.SerialException as e:
    print(f"Serial error: {e}")
