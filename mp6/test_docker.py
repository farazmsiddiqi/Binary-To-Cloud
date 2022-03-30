import pytest
from dotenv import load_dotenv
import json, io, os
import requests
import subprocess, time
import signal

load_dotenv()
base_url = os.environ['DOCKER_MICROSERVICE_URL']

@pytest.fixture(scope="session", autouse=True)
def pytest_sessionstart():
    os.system("make clean")
    print(os.environ['DOCKER_BUILD_COMMAND'])
    proc_build = subprocess.Popen(os.environ['DOCKER_BUILD_COMMAND'].split())
    proc_build.wait()
    assert(proc_build.returncode == 0)
    print(os.environ['DOCKER_RUN_COMMAND'])
    proc_run = subprocess.Popen(os.environ['DOCKER_RUN_COMMAND'].split())
    time.sleep(5)
    proc_run.wait()
    assert(proc_run.returncode == 0)

def test_no_hidden_gif():
    data = {'png': open('sample/no-uiuc-chunk.png', 'rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 500)

def test_hidden_gif_waf():
    data = {'png': open('sample/waf.png','rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 200)

def test_hidden_gif_natalia():
    data = {'png': open('sample/natalia.png','rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 200)

def pytest_sessionfinish(session, exitcode):
    ### Kill the docker process occupying local port.
    proc_kill = subprocess.Popen(os.environ['DOCKER_KILL_COMMAND'].split())
    proc_kill.wait()
    assert(proc_kill.returncode == 0)
