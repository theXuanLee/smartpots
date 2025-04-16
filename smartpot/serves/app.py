# server.py
from flask import Flask, request, jsonify, render_template
import json
import os

app = Flask(__name__)

data_store = []  # 存储传感器数据

@app.route('/')
def index():
    return render_template('index.html', data=data_store)

@app.route('/data', methods=['POST'])
def receive_data():
    try:
        data = request.get_json()
        if data:
            data_store.append(data)
            return jsonify({"message": "Data received", "data": data}), 200
        else:
            return jsonify({"error": "No data received"}), 400
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/data', methods=['GET'])
def get_data():
    return jsonify(data_store)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
