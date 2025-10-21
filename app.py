from flask import Flask, render_template, url_for, request, jsonify
from datetime import datetime
import pyrebase

app = Flask(__name__, static_folder="static", template_folder="templates")

config = {}
key = 0
@app.route("/")
def main():
    return render_template("main.html")

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
        db.child("users/" + userID, "/data/" + "/" + timeStamp).update({ 'testKey': 'testValue' })

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

if __name__ == "__main__":
    app.run(debug=False, host="10.197.40.151", port=5000)
    # app.run(debug=False, host="169.254.198.223", port=5000)