import cv2
import numpy
import serial
from time import sleep

_serial = serial.Serial("COM3", 19200)
_serial.flushInput()
sleep(2)
print("Connection to arduino...")

targetMultiplier = .1

_video = cv2.VideoCapture(0)
ret, _frame = _video.read()
frameSizeX = _frame.shape[1]
frameSizeY = _frame.shape[0]
frameCenterX = int(frameSizeX/2)
frameCenterY = int(frameSizeY/2)

startX = int(frameCenterX - (frameSizeX * targetMultiplier)/2)
startY = int(frameCenterY - (frameSizeY * targetMultiplier)/2)
endX = int(frameCenterX + (frameSizeX * targetMultiplier)/2)
endY = int(frameCenterY + (frameSizeY * targetMultiplier)/2)
# Webcam Resolution is 640x480
# Center of Image is 320x240

print(frameSizeX, frameSizeY, frameCenterX, frameCenterY, "\n")
print(startX, endX, startY, endY, "\n")

while(True):
    ret, _frame = _video.read()
    # _frame = cv2.medianBlur(_frame, 5)
    grayFrame = cv2.cvtColor(_frame, cv2.COLOR_BGR2GRAY)
    grayFrame = cv2.medianBlur(grayFrame, 5)
    ret2, frameThresh = cv2.threshold(grayFrame, 127, 255, 0)
    # ret2, frameThresh = cv2.threshold(grayFrame, 127, 255, cv2.THRESH_BINARY)

    # ret2, frameThresh = cv2.adaptiveThreshold(grayFrame, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)

    frameMoment = cv2.moments(frameThresh)

    if frameMoment["m00"] != 0:
        centerX = int(frameMoment['m10'] / frameMoment['m00'])
        centerY = int(frameMoment['m01'] / frameMoment['m00'])
    else:
        centerX, centerY = 0, 0

    cv2.circle(_frame, (centerX, centerY), 5, (0, 0, 255), -1)
    cv2.putText(_frame, "Centroid", (centerX - 50, centerY - 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
    centroid = "({}, {})".format(centerX, centerY)
    cv2.putText(_frame, centroid, (centerX - 25, centerY - 25), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
    cv2.circle(_frame, (frameCenterX, frameCenterY), 2, (255, 255, 255), -1)
    cv2.rectangle(_frame, (startX, startY), (endX, endY), (255, 255, 255), 2)
    cv2.imshow('Frame-GrayScale', _frame)

    cv2.imshow('Binary-Threshold', frameThresh)

    centroidWrite = "X{0:d}Y{1:d}Z".format(centerX, centerY)
    _serial.write(centroidWrite.encode())

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

_serial.close()
_video.release()
cv2.destroyAllWindows()
exit()