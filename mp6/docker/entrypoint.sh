#!/bin/bash
export MP2_DIR=../mp2/
make png-extractGIF
flask run --host 0.0.0.0
