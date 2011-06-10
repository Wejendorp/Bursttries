#! /usr/bin/python
import sys
h = {}
for line in sys.stdin.xreadlines():
  for term in line.split():
    h[term] = ""
print str(len(h)) + " words"

size = 0
for term in h:
    size += len(term)
print str(size) + " unique size"
