import cv2
import sys
import logging as log
import datetime as dt

from time import sleep

#cascPath = sys.argv[1]
cascPath = "haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascPath)
log.basicConfig(filename='webcam.log',level=log.INFO)

video_capture = cv2.VideoCapture(0)
anterior = 0
i = 0
fx=0.5
fy=0.5
while True:
    if not video_capture.isOpened():
        print('Unable to load camera.')
        sleep(5)
        pass

    # Capture frame-by-frame
    ret, frame = video_capture.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.resize(gray, (320,240))
    ol=frame.copy();
    
    if(i%2==0):
	i=0
        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(10, 10)
        )
    i=i+1


    al=0.8

    # Draw a rectangle around the faces
    for (x, y, w, h) in faces:
	print x,y
	fx = int(fx * al + x * (1-al))
	fy = int(fy * al + y * (1-al))
        cv2.rectangle(ol, (fx*2, fy*2), (2*fx+w*2, 2*fy+h*2), (250, 250, 250), -1)
        cv2.rectangle(frame, (fx*2, fy*2), (2*fx+w*2, 2*fy+h*2), (255, 255, 255), 1)
        cv2.putText(frame,'Location : '+ str(160-(fx+w/2))+','+ str(120-(fy+h/2))  , (fx*2, -3+fy*2), cv2.FONT_HERSHEY_SIMPLEX,0.5, (255, 255, 255), 1)
        cv2.addWeighted(ol, 0.1, frame, 0.9,1, frame)

    if anterior != len(faces):
        anterior = len(faces)
        log.info("faces: "+str(len(faces))+" at "+str(dt.datetime.now()))

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    # Display the resulting frame
    cv2.imshow('Video', frame)

# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()
