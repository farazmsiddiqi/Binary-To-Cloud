from termios import CSTART
from tracemalloc import start
from flask import Flask, render_template, request, jsonify
import os
import requests
import re
from datetime import datetime, timedelta, time
import json

app = Flask(__name__)

# Route for "/" (frontend):
@app.route('/')
def index():
  return render_template("index.html")


# Route for "/weather" (middleware):
@app.route('/weather', methods=["POST"])
def POST_weather():
  COORDS = (40.1125,-88.2284)
  SERVER_URL = os.getenv('COURSES_MICROSERVICE_URL')
  MAX_FORECAST_HRS = 144
  WEEKDAY_NUMS = {
    'M':0,
    'T':1,
    'W':2,
    'R':3,
    'F':4
  }

  subject, number = split_course(request.form['course'])

  r = requests.get(f'{SERVER_URL}/{subject}/{number}/')

  # ensure that course is valid
  if r.status_code == 404:
    return jsonify({'error': f'course: {subject} {number} not found'}), 400

  # capture course data, and use when calling the weather api
  course_data = r.json()

  # assign an int [0,6] to each weekday (weekends not included) the class is offered
  # Mon:0, Tues:1, ...
  course_weekdays = [x.upper() for x in list(course_data['Days of Week'])]
  course_weekday_nums = [WEEKDAY_NUMS[day] for day in course_weekdays]

  ## what time is is right now?
  now = datetime.now()

  # gets 'now' to class date (accuracy=day)
  num_days_to_class = calc_days_to_class(now, course_weekday_nums)

  ## when is the next class relative to my current time?
  course_start_time = datetime.strptime(course_data['Start Time'], '%I:%M %p').replace(year=datetime.today().year,
                                                                                       month=datetime.today().month,
                                                                                       day=datetime.today().day + num_days_to_class)

  # same day, class is over: find date of next class
  if datetime.now().day == course_start_time.day and datetime.now().hour > course_start_time.hour:
    num_days_to_class = 1
    now = now + timedelta(days=1)

    num_days_to_class += calc_days_to_class(now, course_weekday_nums)
    course_start_time += timedelta(days=num_days_to_class)
  
  ## what is the weather at that current time?
  weather_req = requests.get(f'https://api.weather.gov/points/{COORDS[0]},{COORDS[1]}')
  forecast_req = requests.get(weather_req.json()['properties']['forecastHourly'])
  # a list of dicts describing hourly weather
  periods = forecast_req.json()['properties']['periods']

  # go through the periods, find the time period that is closest to the course_start_time
  # after correct period is found, capture relevant metadata
  date_mask = '%Y-%m-%dT%H:%M'
  for period in periods:
    start_time = datetime.strptime(period['startTime'][:-9], date_mask)
    end_time = datetime.strptime(period['endTime'][:-9], date_mask)

    if course_start_time >= start_time and course_start_time < end_time:
      return build_response(subject, number, course_start_time, period, MAX_FORECAST_HRS)

  return jsonify({'error': f'no time period found for course: {subject} {number}'}), 400


def split_course(course):
  subject = re.split('\d+', course)[0].strip().upper()
  number = re.split('[A-Za-z]', course)[-1].strip()

  return subject, number


def calc_days_to_class(now, course_weekday_nums):
  counter = 0
  while now.weekday() not in course_weekday_nums:
    now = now + timedelta(days=1)
    counter += 1
  return counter


def is_forecast_avail(course_start_time, max_future_forecast_time):
  time_diff = datetime.now() - course_start_time
  return time_diff.seconds/3600 < max_future_forecast_time


def build_response(subject, number, course_start_time, period, max_future_forecast_time):
  # this is the correct period, build json response
  response_json = {}
  response_json["course"] = subject +  " " + number
  response_json['nextCourseMeeting'] = course_start_time.strftime('%Y-%m-%d %H:%M:%S')
  response_json['forecastTime'] = period['startTime'][:10] + " " + period['startTime'][11:19]

  # check to see if next class is within 6 days
  if is_forecast_avail(course_start_time, max_future_forecast_time):
    response_json['temperature'] = period['temperature']
    response_json['shortForecast'] = period['shortForecast']
  else:
    response_json['temperature'] = "forecast unavailable"
    response_json['shortForecast'] = "forecast unavailable"
  
  return jsonify(response_json), 200