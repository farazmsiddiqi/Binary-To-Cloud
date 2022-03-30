python3 -m pip install pandas
python3 -m pip install requests
python3 -m pip install python-dotenv
python3 -m pip install pytest
python3 -m pip install flask
echo "Setting Timezone"
export TZ='America/Chicago'
TZ='America/Chicago'
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
echo "Printing Date Information"
date
