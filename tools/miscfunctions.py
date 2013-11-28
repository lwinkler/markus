# -*- coding: utf-8 -*-
# Created on Wed Oct 16 11:10:51 2013
# @author: polchky

import numpy as np
import cv2
import os
import shutil
import string
import time
import datetime

def createbigframe(height,width,layers):
    bigframe = np.zeros((2*height+6, 2*width+6, layers),np.uint8)
    bigframe[:] = 255
    bigframe[height+4:-2,width+4:-2,:] = 0
    return bigframe

def erodeanddilate(fgmask,erosiond, dilatationd):
    element = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(erosiond, erosiond))
    element2 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(dilatationd / 2, dilatationd))
    fgmask = cv2.erode(fgmask,element)
    fgmask = cv2.dilate(fgmask, element2)
    ret,fgmask = cv2.threshold(fgmask, 10,255, cv2.THRESH_BINARY)
    return fgmask
            
def drawcircleandlines(frame,blobs):
    for blob in blobs:
        for index in range(len(blob[3])-1):
            cv2.line(frame,(blob[3][index][0],blob[3][index][1]),(blob[3][index+1][0],blob[3][index+1][1]),(255,0,0),1)
        cv2.circle(frame,(blob[3][-1][0],blob[3][-1][1]),5,(0,0,255),2)

def writevideo(folder, framename):
    if os.path.exists(str("args/" +framename)):
        os.remove(str("args/" + framename))
    shutil.move(framename, "args/")

def displaytimeinfo(end,mytime):
    minutes = int(end/60)
    seconds = end - 60 * minutes
    videoseconds = mytime / 12.
    videominutes = int(videoseconds / 60)
    videohours = int(videominutes / 60)
    videominutes = videominutes - 60 * videohours   
    print("duration: %d min %d sec, for a video of %d hr, %d min. Ratio: %.2f" % (minutes, seconds, videohours, videominutes, end / videoseconds))

def displaytotaltime(totaltime):
    seconds = int(totaltime)
    minutes = seconds / 60
    seconds = seconds - 60 * minutes
    hours = minutes / 60
    minutes = minutes - 60 * hours
    print("\nTotal duration: %d hr %d min % sec" % (hours, minutes, seconds))

def inttotime(duration):
    seconds = int(duration)
    minutes = seconds / 60
    seconds = seconds - 60 * minutes
    hours = minutes / 60
    minutes = minutes - 60 * hours
    return "Total time: %d hrs %d mins %d secs" % (hours, minutes, seconds)

def writeintrusion(textfile, start, mytime, intrusioncounter):
    seconds = start / 12
    cents = start - 12 * seconds
    cents = cents * 1000 / 12
    minutes = seconds / 60
    seconds = seconds - 60 * minutes
    hours = minutes / 60
    minutes = minutes - 60 * hours
    starttime = "%.2d:%.2d:%.2d,%.3d" % (hours, minutes, seconds, cents)
    seconds = mytime / 12
    cents = mytime - 12 * seconds
    cents = cents * 1000 / 12
    minutes = seconds / 60
    seconds = seconds - 60 * minutes
    hours = minutes / 60
    minutes = minutes - 60 * hours
    endtime = "%.2d:%.2d:%.2d,%.3d" % (hours, minutes, seconds, cents)
    mystring = "%d\n" + starttime + " --> " + endtime + "\nintrusion\n\n"
    textfile.write(mystring % (intrusioncounter))
    
def srttable(textfile):
    gt = open(textfile,"r")
    flist = []
    findex = 1
    tindex = 0
    while True:
        line = gt.readline()
        if line == "":
            break
        tindex += 1
        line = gt.readline().rstrip()
        fstart, fend = string.split(line," --> ")
        secs, cents = fstart.split(",")
        x = time.strptime(secs,"%H:%M:%S")
        fstart = datetime.timedelta(hours=x.tm_hour,minutes=x.tm_min,seconds=x.tm_sec).total_seconds() + float(cents)/1000
        secs, cents = fend.split(",")
        x = time.strptime(secs,"%H:%M:%S")
        fend = datetime.timedelta(hours=x.tm_hour,minutes=x.tm_min,seconds=x.tm_sec).total_seconds() + float(cents)/1000
        label = gt.readline().rstrip()
        if label == "intrusion":
            flist.append([findex,fstart,fend])
        elif label != "normal" and label != "reverse" and label != "nothing":
            print("Problem with label n. %d: %s" % (findex,label))
        gt.readline()
        findex += 1
    return flist, tindex
    
def readparams():
    params = open("params","r")
    params.readline()
    ed = int(params.readline().rstrip())
    params.readline()
    dd = int(params.readline().rstrip())
    params.readline()
    mh = int(params.readline().rstrip())
    params.readline()
    mt = int(params.readline().rstrip())
    params.readline()
    ms = int(params.readline().rstrip())
    params.close()
    rx = "%s_%s_%s_%s_%s/" % (ed,dd,mh,mt,ms)
    return ed, dd, mh, mt, ms, rx
    
def readconf():
    conf = open("conf","r")
    sr = conf.readline().rstrip()
    rr = conf.readline().rstrip()
    ar = conf.readline().rstrip()
    vt = conf.readline().rstrip()
    me = conf.readline().rstrip()
    seq = []
    for i in range(7):
        seq.append(conf.readline().rstrip())
    conf.close()
    return sr,rr,ar,vt,me,seq
    
    
    
    
