from flask import Flask, render_template, request, jsonify, send_from_directory
import requests

ESP32_IP = "http://YOUR_ESP32_IP"  # Example: http://192.168.1.105

app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/public/<path:filename>")
def serve_audio(filename):
    return send_from_directory("public", filename)


@app.route("/on", methods=["POST"])
def on():

    requests.get(ESP32_IP+"/on")

    return jsonify({"status":"on"})


@app.route("/off", methods=["POST"])
def off():

    requests.get(ESP32_IP+"/off")

    return jsonify({"status":"off"})


@app.route("/setTimer", methods=["POST"])
def setTimer():

    data=request.json

    on=data["on"]

    off=data["off"]

    requests.get(f"{ESP32_IP}/setTimer?on={on}&off={off}")

    return jsonify({"status":"timer_set"})


@app.route("/status")
def status():

    try:
        r=requests.get(ESP32_IP+"/status",timeout=1)

        return r.json()

    except:
        return {"status":"offline"}


if __name__=="__main__":

    app.run(host="0.0.0.0",port=5000)
