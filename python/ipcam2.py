import numpy as np
import cv2
import time
import requests
import threading
from threading import Thread, Event, ThreadError

class Cam():

  def __init__(self, url):
    
    self.stream = requests.get(url, stream=True)
    self.thread_cancelled = False
    self.thread = Thread(target=self.run)
    print "camera initialised"

    
  def start(self):
    self.thread.start()
    print "camera stream started"
    
  def run(self):
    bytes=''
    cascPath = "haarcascade_frontalface_default.xml"
    faceCascade = cv2.CascadeClassifier(cascPath)
    while not self.thread_cancelled:
      try:
        bytes+=self.stream.raw.read(1024)
        a = bytes.find('\xff\xd8')
        b = bytes.find('\xff\xd9')
        if a!=-1 and b!=-1:
          jpg = bytes[a:b+2]
          bytes= bytes[b+2:]
          frame = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8),cv2.IMREAD_COLOR)
    	  #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

          faces = faceCascade.detectMultiScale(
              frame,
              scaleFactor=1.1,
              minNeighbors=5,
              minSize=(10, 10),
              flags=cv2.cv.CV_HAAR_SCALE_IMAGE
          )

          # Draw a rectangle around the faces
	  for (x, y, w, h) in faces:
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

          # Display the resulting frame
          cv2.imshow('Video', frame)

          if cv2.waitKey(1) & 0xFF == ord('q'):
            break
      except ThreadError:
        self.thread_cancelled = True
        
        
  def is_running(self):
    return self.thread.isAlive()
      
    
  def shut_down(self):
    self.thread_cancelled = True
    #block while waiting for thread to terminate
    while self.thread.isAlive():
      time.sleep(1)
    return True

  
    
if __name__ == "__main__":
  url = 'http://192.168.2.117:8080/video?action=stream'
  cam = Cam(url)
  cam.start()
