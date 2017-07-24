#!/usr/bin/python3
import sys
import datetime
import humanize
from re import split as re_split
if "T" in sys.argv[1]:
    time = datetime.datetime.utcnow() - datetime.datetime(*map(int, re_split(r'[^\d]', sys.argv[1].replace('+00:00', ''))))
else:
    time = datetime.datetime.utcnow() - datetime.datetime.utcfromtimestamp(((int(sys.argv[1]) >> 22) + 1420070400000) / 1000)
print(humanize.naturaltime(time))
