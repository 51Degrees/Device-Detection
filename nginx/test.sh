#!/bin/bash

REQUESTS=100000
LIMIT=0.500
RELOADLIMIT=1000
RELOADSTODO=5

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
PASS="${GREEN}[Pass]${NC}\t"
FAIL="${RED}[Fail]${NC}\t"
FAILED=0
if [ ! -e "nginx" ]; then
	printf "nginx is not installed in this directory. Install with \"make install-pattern\" or \"make install-trie\"\n"
	exit
fi

printf "\n---------------------\n| Starting new nginx process.\n---------------------\n"
SLEEPCOUNT='0'
while [ "$(pidof nginx)" ]; do
	killall nginx
	SLEEPCOUNT=$(($SLEEPCOUNT + 1))	
	if [ $SLEEPCOUNT -gt 5 ]; then
		printf "Failed to stop currently running nginx process\n"
		exit
	fi
	sleep 1
done
./nginx

printf "\n---------------------\n| Testing Server response.\n--------------------\n"

DESKTOPUA="Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0"
MOBILEUA="Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53"
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
TIME=`../ApacheBench/ab -c 10 -i -n $REQUESTS -U ../data/20000\ User\ Agents.csv localhost:8888/ | grep "(mean, across all concurrent requests)" | sed -r 's/.*_([0-9]*)\..*/\1/g' | grep -E -o '\<[0-9]{1,2}\.[0-9]{2,5}\>'`
if [ $TIME ]; then
	if [ $(echo "$TIME < $LIMIT" | bc -l) -eq 1 ]; then
		printf "${PASS}Stress test: $TIME ms per detection is within limit of $LIMIT.\n"
	else
		printf "${FAIL}Stress test: $TIME ms per detection excedes limit of $LIMIT.\n"
		FAILED=$(($FAILED + 1))
	fi
BASETIME=$TIME
else
	printf "${FAIL}Stress test reload: ApacheBench could not complete tests.\n"
	FAILED=$(($FAILED + 1))
fi


if [ -e "build/pattern.conf" ]; then
	printf "\n---------------------\n| Testing ApacheBench stress with cache.\n--------------------\n"
	TIME=`../ApacheBench/ab -c 10 -i -n $REQUESTS -U ../data/20000\ User\ Agents.csv localhost:8888/ | grep "(mean, across all concurrent requests)" | sed -r 's/.*_([0-9]*)\..*/\1/g' | grep -E -o '\<[0-9]{1,2}\.[0-9]{2,5}\>'`
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
RELOADCOUNT=0
RELOADPASS=1
TOTALTIME=0
while [ $RELOADCOUNT -lt $RELOADSTODO ]; do
	printf "Doing nginx reload test $((RELOADCOUNT + 1)) of ${RELOADSTODO}\n"
	./nginx -s reload
	RELOADCOUNT=$(($RELOADCOUNT + 1))
	TIME=`../ApacheBench/ab -c 10 -i -n $(echo "$REQUESTS / $RELOADSTODO" | bc -l) -q -U ../data/20000\ User\ Agents.csv localhost:8888/ | grep "(mean, across all concurrent requests)" | sed -r 's/.*_([0-9]*)\..*/\1/g' | grep -E -o '\<[0-9]{1,2}\.[0-9]{2,5}\>'`
	if [ ! "$TIME" ]; then
		break
	fi
	TOTALTIME=$(echo "$TOTALTIME + $TIME" | bc -l)
done

if [ "$TIME" ]; then
	TOTALTIME=0$(echo "scale=3; $TOTALTIME / $RELOADSTODO" | bc -l)
	RELOADTIME=$(echo "scale=0; ( $TOTALTIME - $BASETIME ) * $REQUESTS" | bc -l)
	if [ $(echo "$RELOADTIME < $RELOADLIMIT" | bc -l) -eq 1 ]; then
		printf "${PASS}Reload penalty: $RELOADTIME ms penalty on reloading is within limit of $RELOADLIMIT.\n"
	else
		printf "${FAIL}Reload penalty: $RELOADTIME ms penalty on reloading excedes limit of $RELOADLIMIT.\n"
		FAILED=$(($FAILED + 1))
	fi
	if [ $(echo "$TOTALTIME < $LIMIT" | bc -l) -eq 1 ]; then
		printf "${PASS}Stress test reload: $TOTALTIME ms per detection is within limit of $LIMIT.\n"
	else
		printf "${FAIL}Stress test reload: $TOTALTIME ms per detection excedes limit of $LIMIT.\n"
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
printf "${TESTCOLOUR}Finished tests with ${FAILED} failure(s).${NC}\n"
