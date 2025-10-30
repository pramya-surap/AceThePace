from flask import Flask, render_template, url_for, request, jsonify
from datetime import datetime
import pyrebase

# NEW imports
from strip_controller2 import StripController

app = Flask(__name__, static_folder="static", template_folder="templates")

config = {}
key = 0

# ---- NEW: controller singleton configuration ----
# update COM port to your actual device port
SERIAL_PORT = "/dev/tty.usbserial-110"
SERIAL_BAUD = 9600
MOCK_SERIAL = False  # set True to test without Arduino (prints what would be sent)

controller = StripController(port=SERIAL_PORT, baud=SERIAL_BAUD, mock=MOCK_SERIAL)
# -------------------------------------------------

@app.route("/")
def main():
    return render_template("main2.html")

# (your existing /test route remains unchanged)
@app.route("/test", methods=["GET", "POST"])
def test():
    global config, userID, db, timeStamp, key

    if request.method == "POST":
        timeStamp = datetime.now().strftime("%d-%m-%Y %H:%M:%S")
        config = request.get_json()
        userID = config.pop("userID")

        print("User ID: " + userID, flush=True)
        print(config, flush=True)

        firebase = pyrebase.initialize_app(config)
        db = firebase.database()
        db.child("users/" + userID, "/data/" + "/" + timeStamp).update({'testKey': 'testValue'})

        return 'Data Uploaded', 200

    else:
        if (bool(config) == False):
            print("FB config is empty")
        else:
            value = request.args.get('distance')
            print('Distance: ' + value, flush=True)
            db.child('users/' + userID, '/data/' + timeStamp).update({key: value})
            key += 1
        return "Success"

# ---- NEW: API endpoint to receive paces from UI ----
@app.route("/api/send", methods=["POST"])
def api_send():
    """
    Expected JSON payload:
    {
      "red_on": true/false,
      "green_on": true/false,
      "blue_on": true/false,
      "red_pace": "100",   # string or number
      "green_pace": "200",
      "blue_pace": "300",
      "event_distance": "400"
    }
    """
    try:
        data = request.get_json(force=True)
        # coerce booleans
        red_on = bool(data.get("red_on", False))
        green_on = bool(data.get("green_on", False))
        blue_on = bool(data.get("blue_on", False))

        def parse_pace(k):
            v = data.get(k, "")
            if v is None or v == "":
                return None
            try:
                return int(v)
            except:
                raise ValueError(f"{k} must be an integer between 100 and 999 or empty")

        red_pace = parse_pace("red_pace")
        green_pace = parse_pace("green_pace")
        blue_pace = parse_pace("blue_pace")

        # build list of 3 ints: use 0 for off
        paces = []
        for on, pace in ((red_on, red_pace), (green_on, green_pace), (blue_on, blue_pace)):
            if on:
                if pace is None:
                    return jsonify({"ok": False, "error": "Active light missing pace value"}), 400
                if not (100 <= pace <= 999):
                    return jsonify({"ok": False, "error": "Pace must be between 100 and 999"}), 400
                paces.append(pace)
            else:
                paces.append(0)

        # optional: capture event distance if you want to store/use it
        event_distance = data.get("event_distance", "")

        # send to controller
        res = controller.send_paces(paces)
        if res.get("ok"):
            return jsonify({"ok": True, "sent": res.get("sent"), "response": res.get("response", "")})
        else:
            return jsonify({"ok": False, "error": res.get("error", "unknown")}), 500

    except ValueError as ve:
        return jsonify({"ok": False, "error": str(ve)}), 400
    except Exception as e:
        return jsonify({"ok": False, "error": f"Server error: {e}"}), 500
# ----------------------------------------------------

if __name__ == "__main__":
    # keep your preferred host; if you get "requested address" error, either use 0.0.0.0 or pick an IP that exists on the machine
    app.run(debug=False, host="172.28.161.129", port=5000)
