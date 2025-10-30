# 🏃‍♂️ ESP32 LED Runner Controller - User Guide

## 📋 **Initial Setup**

### 1. **Configure WiFi Settings**
Before uploading the code, you MUST update the WiFi credentials in the Arduino code:

```cpp
const char* ssid = "YOUR_WIFI_SSID";        // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password
```

**Example:**
```cpp
const char* ssid = "MyHomeWiFi";
const char* password = "mypassword123";
```

### 2. **Required Libraries**
Make sure you have these libraries installed in Arduino IDE:
- **Adafruit NeoPixel** (install via Library Manager)
- **WiFi** (usually included with ESP32 board package)

### 3. **Hardware Connections**
- Connect your NeoPixel strip to **Pin 9** on the ESP32
- Ensure proper power supply for your LED strip
- Connect ESP32 to your computer via USB

### 4. **Upload Code**
- Select your ESP32 board in Arduino IDE
- Upload the `esp32_led_controller.ino` code
- Open Serial Monitor (115200 baud) to see connection status

---

## 🌐 **Accessing the Website**

### **Step 1: Get the IP Address**
1. Open **Serial Monitor** in Arduino IDE (115200 baud)
2. Look for this message after WiFi connection:
   ```
   WiFi connected!
   IP address: 192.168.1.100
   Web server started!
   ```
3. **Copy the IP address** (e.g., `192.168.1.100`)

### **Step 2: Open the Website**
1. Open any web browser on your computer, phone, or tablet
2. Type the IP address in the address bar: `http://192.168.1.100`
3. Press Enter

**The LED Runner Controller website will load!**

---

## 🎮 **Using the Web Interface**

### **Main Interface Elements**

#### **🔴 Red Runner Control**
- **Slider**: Drag to adjust red runner speed (100-999ms)
- **Display**: Shows current delay value in real-time
- **Range**: 100ms (fastest) to 999ms (slowest)

#### **🟢 Green Runner Control**
- **Slider**: Drag to adjust green runner speed (100-999ms)
- **Display**: Shows current delay value in real-time
- **Range**: 100ms (fastest) to 999ms (slowest)

#### **🔵 Blue Runner Control**
- **Slider**: Drag to adjust blue runner speed (100-999ms)
- **Display**: Shows current delay value in real-time
- **Range**: 100ms (fastest) to 999ms (slowest)

#### **🔄 Update Button**
- **Purpose**: Applies all slider changes to the LED runners
- **When to use**: After adjusting any sliders
- **Feedback**: Status message confirms successful update

---

## 📱 **Step-by-Step Usage Guide**

### **Basic Operation:**

1. **Open the website** using the IP address from Serial Monitor
2. **Adjust sliders** to set desired speeds for each runner:
   - Move slider **left** = faster runner (lower delay)
   - Move slider **right** = slower runner (higher delay)
3. **Click "Update All Runners"** to apply changes
4. **Watch the LEDs** respond to your settings!

### **Speed Guidelines:**
- **100-200ms**: Very fast runners (blazing speed!)
- **200-400ms**: Fast runners (quick movement)
- **400-600ms**: Medium speed (balanced)
- **600-800ms**: Slow runners (leisurely pace)
- **800-999ms**: Very slow runners (crawl speed)

### **Visual Feedback:**
- **Status messages** appear below the button:
  - "Updating runners..." (while processing)
  - "Runners updated successfully!" (confirmation)
  - "Ready to control LED runners!" (ready state)

---

## 🔧 **Troubleshooting**

### **Can't Connect to WiFi?**
- ✅ Check WiFi credentials are correct
- ✅ Ensure ESP32 is within WiFi range
- ✅ Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- ✅ Check Serial Monitor for connection errors

### **Can't Access Website?**
- ✅ Verify IP address from Serial Monitor
- ✅ Ensure device is on same WiFi network as ESP32
- ✅ Try refreshing the browser page
- ✅ Check if firewall is blocking the connection

### **LEDs Not Responding?**
- ✅ Check LED strip connections to Pin 9
- ✅ Verify power supply is adequate
- ✅ Ensure NeoPixel library is installed
- ✅ Check Serial Monitor for any error messages

### **Website Looks Broken?**
- ✅ Try refreshing the page
- ✅ Clear browser cache
- ✅ Try a different browser
- ✅ Check if JavaScript is enabled

---

## 📱 **Mobile Usage**

The website is **mobile-friendly** and works great on phones and tablets:

1. **Connect your phone** to the same WiFi network
2. **Open browser** and navigate to the ESP32 IP address
3. **Use touch sliders** to control runner speeds
4. **Tap "Update All Runners"** to apply changes

---

## 🎯 **Pro Tips**

### **Best Practices:**
- **Start with default values** (300, 500, 700ms) to see the effect
- **Make small adjustments** first to understand the speed differences
- **Use different speeds** for each runner to create interesting patterns
- **Bookmark the IP address** for quick access

### **Creative Ideas:**
- **Racing effect**: Set one runner much faster than others
- **Synchronized**: Set all runners to similar speeds
- **Chase pattern**: Create a sequence of speeds (fast → medium → slow)
- **Random mode**: Keep changing speeds for dynamic effects

### **Performance Notes:**
- **Lower delays** = more CPU usage but smoother animation
- **Higher delays** = less CPU usage but choppier movement
- **Range 100-999ms** is optimized for best visual effect

---

## 🔄 **Quick Reference**

| Action | How To |
|--------|--------|
| **Access website** | Type ESP32 IP address in browser |
| **Speed up runner** | Move slider left (lower ms value) |
| **Slow down runner** | Move slider right (higher ms value) |
| **Apply changes** | Click "Update All Runners" button |
| **Check status** | Look at status message below button |
| **Reset to defaults** | Refresh page (loads 300, 500, 700ms) |

---

## 🆘 **Need Help?**

If you encounter issues:
1. **Check Serial Monitor** for error messages
2. **Verify all connections** are secure
3. **Restart ESP32** by unplugging and reconnecting power
4. **Try different WiFi network** if connection issues persist

**Happy LED controlling!** 🎉
