from app import app
import subprocess
from datetime import datetime, timedelta
import os, time, requests, base64, dotenv, pytest, sys

@pytest.fixture(autouse=True, scope='session')
def pytest_sessionstart():
    global fc
    fc = requests.get(base64.b64decode(b'aHR0cHM6Ly9hcGkud2VhdGhlci5nb3YvZ3JpZHBvaW50cy9JTFgvOTUsNzEvZm9yZWNhc3QvaG91cmx5').decode()).json()
    dotenv.load_dotenv()

@pytest.fixture(scope='module')
def start_microservice():
    host, port = os.getenv('COURSES_MICROSERVICE_URL').split("/")[2].split(":")

    microservice = subprocess.Popen([sys.executable, "-m", "flask", "run", "--host", host, "--port", port], cwd="courses_microservice")
    time.sleep(2) #wait a bit for the microservice to start
    yield
    microservice.terminate()

@pytest.fixture(scope='module')
def test_client():
    flask_app = app
    with flask_app.test_client() as testing_client:
        # Establish an application context
        with flask_app.app_context():
            yield testing_client

def check_course(client,coursedata):
    # set up test variables
    course, cname = coursedata
    now           = datetime.now()

    # call the student's api and get the response
    r = client.post('/weather', data={"course": course}, content_type='multipart/form-data')

    # check invalid courses get the appropriate 400 response
    if not cname:
        assert(r.status_code == 400), f"invalid course {course} did not return 400 response"
        return

    # check valid courses get the appropriate 200 response
    assert(r.status_code == 200), f"valid course {course} did not return 200 response"

    # check if we get the right course from the courses microservice
    cinfo = requests.get(f"""{os.getenv('COURSES_MICROSERVICE_URL')}/{cname.replace(" ","/")}/""").json()
    assert(cinfo["course"] == cname), f"microservice did not return info for {course}"

    # check if course name in student's response is reasonable
    assert(r.json["course"] == cname), f"""course name {r.json["course"]} did not resemble {course}"""

    # check if the returned meeting time is within the week
    mtime = datetime.strptime(r.json["nextCourseMeeting"],"%Y-%m-%d %H:%M:%S")
    assert(mtime > now), f"{course} meeting time is not in the future"
    assert((mtime-now).days < 7), f"{course} meeting time is too far in the future"

    # check if the course actually meets on the returned day of the week
    assert("MTWRFSU"[mtime.weekday()] in cinfo["Days of Week"]), f"{course} does not meet on returned day of week"

    # check if the course actually meets on the returned hour and minute
    ctime = datetime.strptime(cinfo["Start Time"],'%I:%M %p')
    assert(mtime.hour == ctime.hour), f"{course} meeting hour is incorrect"
    assert(mtime.minute == ctime.minute), f"{course} meeting minute is incorrect"

    # check the course does not meet at some earlier time in the future
    i = 1
    while ((mtime-timedelta(i)) > now):
        assert("MTWRFSU"[(mtime-timedelta(i)).weekday()] not in cinfo["Days of Week"]), f"earlier meeting time for {course} was available"
        i += 1

    # check if forecast time is correct given the next meeting time
    assert(r.json["forecastTime"][:14] == r.json["nextCourseMeeting"][:14]), f"""hour of forecast does not match hour of {course} meeting"""
    assert(r.json["forecastTime"][-5:] == "00:00"), f"""forecast time is not at the top of the hour"""

    if ((mtime-now).total_seconds()//3600) > (6*24):
        # check if forecast is properly marked as unavailable for times more than 6 days in the future
        assert(r.json["shortForecast"] == "forecast unavailable")
        assert(r.json["temperature"]   == "forecast unavailable")
    else:
        # check if we pulled valid data given the forecast time
        assert(any([all([
            hour["startTime"][:19].replace("T"," ")
                == r.json["forecastTime"],
            hour["shortForecast"]
                == r.json["shortForecast"],
            hour["temperature"]
                == r.json["temperature"]])
            for hour in fc["properties"]["periods"]])), f"did not receive expected forecast data for {course}"

def test_valid_course_with_space_upper(start_microservice,test_client):
    check_course(test_client,("CS 107","CS 107"))
    check_course(test_client,("CS 101","CS 101"))

def test_valid_course_with_space_lower(start_microservice,test_client):
    check_course(test_client,("cs 240","CS 240"))
    check_course(test_client,("cs 101","CS 101"))

def test_valid_course_without_space_lower(start_microservice,test_client):
    check_course(test_client,("cs374","CS 374"))
    check_course(test_client,("cs101","CS 101"))

def test_valid_course_without_space_upper(start_microservice,test_client):
    check_course(test_client,("CS444","CS 444"))
    check_course(test_client,("CS101","CS 101"))

def test_invalid_course_correct_format(start_microservice,test_client):
    check_course(test_client,("CSS 12345",False))

def test_invalid_course_incorrect_format(start_microservice,test_client):
    check_course(test_client,("CSS",False))
    check_course(test_client,("12345",False))
    check_course(test_client,("this is not a course and should fail",False))
    check_course(test_client,("this should also fail",False))

def test_once_a_week_course_1(start_microservice,test_client):
    check_course(test_client,("CS 101","CS 101"))

def test_once_a_week_course_2(start_microservice,test_client):
    check_course(test_client,("CS 105","CS 105"))

def test_multiple_times_a_week_course_1(start_microservice,test_client):
    check_course(test_client,("CS 446","CS 446"))

def test_multiple_times_a_week_course_2(start_microservice,test_client):
    check_course(test_client,("CS 225","CS 225"))
