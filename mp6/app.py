from asyncio import subprocess
from distutils.command.upload import upload
from operator import contains
from posixpath import dirname
from urllib import response
from flask import Flask, flash, redirect
app = Flask(__name__)

# Route for "/" for a web-based interface to this micro-service:
@app.route('/')
def index():
  from flask import render_template
  return render_template("index.html")

# Extract a hidden "uiuc" GIF from a PNG image:
@app.route('/extract', methods=["POST"])
def extract_hidden_gif():
  from flask import send_file, request
  import os, imghdr, uuid

  uploaded_file = request.files['png']
  print(uploaded_file)

  if uploaded_file.filename == '':
    return "must upload a file", 500
  
  if imghdr.what(uploaded_file) != 'png':
    return "not a png file", 500

  dir_name = "temp"

  if not os.path.isdir(dir_name):
    os.makedirs(dir_name)
  
  filename = str(uuid.uuid1()) + uploaded_file.filename
  png_path = os.path.join(dir_name, filename)
  uploaded_file.save(png_path)
  gif_file = "return.gif"

  sys_cmd = "./png-extractGIF" + " " + "temp/" + filename + " " + gif_file

  print(sys_cmd)
  os_output = os.system(sys_cmd)
  print(f'\t\tos output: {os_output}')
  
  if os_output != 0:
    return "no gif file in png", 500
  
  return send_file(gif_file), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0')
  