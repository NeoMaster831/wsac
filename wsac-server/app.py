from config import PAYLOAD_SIGNATURE
from flask import Flask

app = Flask(__name__)

@app.route('/ping')
def ping():
    return 'Pong!'

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=True)