#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
PASS="${GREEN}[Pass]${NC}\t"
FAIL="${RED}[Fail]${NC}\t"
FAILED=0
LIMIT=0.500

if [ ! -e "nginx" ]; then
	printf "nginx is not installed in this directory. Install with \"make install-pattern\" or \"make install-trie\"\n"
	exit
fi

printf "\n---------------------\n| Starting new nginx process.\n---------------------\n"
killall nginx
RUNNING='1'
SLEEPCOUNT='0'
while [ $RUNNING = '1' ]; do
	if [ "$(pidof process_name)" ]; then
		SLEEPCOUNT=$(($SLEPCOUNT + 1))
	else
		RUNNING='0'
	fi
	if [ $SLEEPCOUNT -gt 5 ]; then
		printf "Failed to stop running nginx process\n"
		exit
	fi
	sleep 1
done
./nginx

printf "\n---------------------\n| Testing Server response.\n--------------------\n"

DESKTOPUA="Ubuntu"
MOBILEUA="Samsung Galaxy"
TRUE='test: True'
FALSE='test: False'

RESPONSE=`curl -I localhost:8888 | grep -c "200 OK"`
if [ $RESPONSE -gt '0' ]; then
	printf "${PASS}Server Response: Server response 200 recieved.\n"
else
	printf "${FAIL}Server Response: Server response 200 not recieved.\n"
	FAILED=$(($FAILED + 1))
fi

printf "\n---------------------\n| Testing mobile/non-mobile User-Agents.\n--------------------\n"

CORRECTHEADER=`curl -A "$DESKTOPUA" -I localhost:8888 | grep -c "$FALSE"`
if [ $CORRECTHEADER -gt '0' ];
then
	printf "${PASS}Desktop User-Agent matched correctly\n"
else
	printf "${FAIL}Desktop User-Agent matched incorrectly\n"
	FAILED=$(($FAILED + 1))
fi

CORRECTHEADER=`curl -A "$MOBILEUA" -I localhost:8888/ | grep -c "$TRUE"`
if [ $CORRECTHEADER -gt '0' ]; then
	printf "${PASS}Mobile User-Agent matched correctly\n"
else
	printf "${FAIL}Mobile User-Agent matched incorrectly\n"
	FAILED=$(($FAILED + 1))
fi

printf "\n---------------------\n| Testing ApacheBench stress with no cache.\n--------------------\n"
TIME=`../ApacheBench/ab -c 10 -n 10000 -q -U ../data/20000\ User\ Agents.csv localhost:8888/ | grep "(mean, across all concurrent requests)" | sed -r 's/.*_([0-9]*)\..*/\1/g' | grep -E -o '\<[0-9]{1,2}\.[0-9]{2,5}\>'`
if [ $TIME ]; then
	if [ $(echo "$TIME < $LIMIT" | bc -l) -eq 1 ]; then
		printf "${PASS}Stress test: $TIME ms per detection is within limit of $LIMIT.\n"
	else
		printf "${FAIL}Stress test: $TIME ms per detection excedes limit of $LIMIT.\n"
		FAILED=$(($FAILED + 1))
	fi
else
	printf "${FAIL}Stress test reload: ApacheBench could not complete tests.\n"
	FAILED=$(($FAILED + 1))
fi


if [ -e "build/pattern.conf" ]; then
	printf "\n---------------------\n| Testing ApacheBench stress with cache.\n--------------------\n"
	TIME=`../ApacheBench/ab -c 10 -n 10000 -q -U ../data/20000\ User\ Agents.csv localhost:8888/ | grep "(mean, across all concurrent requests)" | sed -r 's/.*_([0-9]*)\..*/\1/g' | grep -E -o '\<[0-9]{1,2}\.[0-9]{2,5}\>'`
	if [ $TIME ]; then
		if [ $(echo "$TIME < $LIMIT" | bc -l) -eq 1 ]; then
			printf "${PASS}Stress test: $TIME ms per detection is within limit of $LIMIT.\n"
		else
			printf "${FAIL}Stress test: $TIME ms per detection excedes limit of $LIMIT.\n"
		FAILED=$(($FAILED + 1))
		fi
	else
		printf "${FAIL}Stress test reload: ApacheBench could not complete tests.\n"
		FAILED=$(($FAILED + 1))
	fi
fi

printf "\n---------------------\n| Testing ApacheBench stress with reload.\n--------------------\n"
TIME="UNSET"
RELOADCOUNT=0

while [ $RELOADCOUNT -lt 5 ]; do
	sleep 1	
	./nginx -s reload
	RELOADCOUNT=$(($RELOADCOUNT + 1))
	printf "Doing nginx reload number ${RELOADCOUNT}\n"
done &
TIME=`../ApacheBench/ab -c 10 -n 200000 -q -U ../data/20000\ User\ Agents.csv localhost:8888/ | grep "(mean, across all concurrent requests)" | sed -r 's/.*_([0-9]*)\..*/\1/g' | grep -E -o '\<[0-9]{1,2}\.[0-9]{2,5}\>'`
wait
if [ $TIME ]; then
	if [ $(echo "$TIME < $LIMIT" | bc -l) -eq 1 ]; then
		printf "${PASS}Stress test reload: $TIME ms per detection is within limit of $LIMIT.\n"
	else
		printf "${FAIL}Stress test reload: $TIME ms per detection excedes limit of $LIMIT.\n"
		FAILED=$(($FAILED + 1))
	fi
else
	printf "${FAIL}Stress test reload: ApacheBench could not complete tests.\n"
	FAILED=$(($FAILED + 1))
fi

if [ $FAILED -gt '0' ]; then
	TESTCOLOUR=${RED}
else
	TESTCOLOUR=${GREEN}
fi
printf "${TESTCOLOUR}Finished tests with ${FAILED} failures.${NC}\n"
