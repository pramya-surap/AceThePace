
import serial
import time
import threading

class StripController:
    def __init__(self, port="COM5", baud=9600, timeout=2, mock=False):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.mock = mock
        self.lock = threading.Lock()
        self.ser = None

        if self.mock:
            print("[StripController] MOCK mode - not opening serial port")
            return

        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=self.timeout)
            time.sleep(2)  # allow Arduino reset
            self.ser.reset_input_buffer()
            print(f"[StripController] Connected to {self.port} @ {self.baud}")
        except Exception as e:
            print(f"[StripController] Failed to open serial port: {e}")
            self.ser = None

    def send_paces(self, paces):
        """
        paces: iterable of 3 integers (100-999 or 0 to mean 'off')
        Returns dict {ok: True/False, sent: '...'}
        """
        if len(paces) != 3:
            raise ValueError("Expecting exactly 3 pace values")

        if not all(isinstance(x, int) for x in paces):
            raise ValueError("All pace values must be integers")

        if not all((x == 0) or (100 <= x <= 999) for x in paces):
            raise ValueError("Every pace must be 0 (off) or between 100 and 999")

        msg = f"{paces[0]} {paces[1]} {paces[2]}\n"

        if self.mock:
            print("[StripController][MOCK] would send:", msg.strip())
            return {"ok": True, "sent": msg.strip(), "mock": True}

        if not self.ser:
            return {"ok": False, "error": "Serial port not open"}

        with self.lock:
            try:
                self.ser.write(msg.encode())
                self.ser.flush()
                # option: read single response line (timeout set on serial)
                try:
                    resp = self.ser.readline().decode(errors="ignore").strip()
                except Exception:
                    resp = ""
                return {"ok": True, "sent": msg.strip(), "response": resp}
            except Exception as e:
                return {"ok": False, "error": str(e)}

    def close(self):
        if self.ser:
            try:
                self.ser.close()
                self.ser = None
            except Exception:
                pass
