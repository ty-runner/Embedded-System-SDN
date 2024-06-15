from flask import Flask, request, jsonify
import sqlite3

app = Flask(__name__)

def init_db():
   conn = sqlite3.connect('sensor_data.db')
   c = conn.cursor()
   c.execute('''
             CREATE TABLE IF NOT EXISTS sensor (
                 id INTEGER PRIMARY KEY AUTOINCREMENT,
                 temperature REAL,
                 humidity REAL,
                 timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
             )
             ''')
   conn.commit()
   conn.close()

@app.route('/data', methods=['POST'])
def data():
   temperature = request.form.get('temperature')
   humidity = request.form.get('humidity')
   
   if temperature and humidity:
       conn = sqlite3.connect('sensor_data.db')
       c = conn.cursor()
       c.execute('INSERT INTO sensor (temperature, humidity) VALUES (?, ?)', (temperature, humidity))
       conn.commit()
       conn.close()
       return 'Data received', 200
   else:
       return 'Invalid data', 400

@app.route('/display', methods=['GET'])
def display():
   conn = sqlite3.connect('sensor_data.db')
   c = conn.cursor()
   c.execute('SELECT * FROM sensor ORDER BY timestamp DESC LIMIT 10')
   data = c.fetchall()
   conn.close()
   return jsonify(data)

if __name__ == '__main__':
   init_db()
   app.run(host='0.0.0.0', port=5000)
