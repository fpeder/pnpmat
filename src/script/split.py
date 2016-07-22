#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys

def split(I):
    if len(I) == 1:
        return
    else:
        print I
        split(I[0:len(I)/2])
        split(I[len(I)/2:])


def split2(s, e):
    print '2', s, e
    sys.stdin.read(1)
    if e - s <= 1:
        return
    else:
        split2(s, s + (e-s)/2)
        split3(s + (e-s)/2+1, e)

def split3(s, e):
    print '3', s, e
    sys.stdin.read(1)
    if e - s <= 1:
        return
    else:
        split3(s + (e-s)/2+1, e)
        split2(s, s + (e-s)/2)

#split(range(16))
split2(0, 16)
