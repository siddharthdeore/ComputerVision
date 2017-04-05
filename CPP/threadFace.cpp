#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <pthread.h>

using namespace cv;
using namespace std;

//GLOBALS
VideoCapture vcap;
Mat frame;
pthread_mutex_t frameLocker;

const string videoStreamAddress = "http://192.168.2.114:8080:video.mjpg";
////////////////////////

/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables */
String face_cascade_name = "lbpcascade_frontalface.xml";
String profile_cascade_name = "lbpcascade_profileface.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier profile_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face  \n detection";

float fx=320, fy=240, alp = 0.7;
void *UpdateFrame(void *arg)
{
	for (;;)
	{
		Mat tempFrame;
		vcap >> tempFrame;

		pthread_mutex_lock(&frameLocker);
		frame = tempFrame;
		pthread_mutex_unlock(&frameLocker);
	}
}

int main(int, char**) {

	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading\n"); return -1; };
	if (!profile_cascade.load(profile_cascade_name)){ printf("--(!)Error loading\n"); return -1; };


//	vcap.open(videoStreamAddress);
	vcap.open(0);

	pthread_mutex_init(&frameLocker, NULL);
	pthread_t UpdThread;
	pthread_create(&UpdThread, NULL, UpdateFrame, NULL);

	long int i=0;
	for (;;)
	{
		i++;
		Mat currentFrame;
		pthread_mutex_lock(&frameLocker);
		currentFrame = frame;
		if (!frame.empty())
		{
			detectAndDisplay(frame);
		}
		pthread_mutex_unlock(&frameLocker);
		if (currentFrame.empty()){
			printf("recieved empty frame %ld\n",i);
			continue;
		}
		circle(currentFrame, Point(fx, fy), 5, Scalar(0, 0, 255));

		cvNamedWindow("Name", CV_WINDOW_NORMAL);
		//cvSetWindowProperty("Name", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		//resize(frame, frame, Size(640, 480));
		imshow("Name", currentFrame);
		if (waitKey(10) == 27)
		{
			exit(0);
		}

		waitKey(1);
	}
}

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect front faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | 1, Size(10, 10));

	//-- Detect left faces
	if (!faces.size())
	{
		profile_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | 1, Size(10, 10));
	}
	//-- Detect right faces
	if (!faces.size())
	{
		flip(frame, frame, 1);
		profile_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | 1, Size(10, 10));
		flip(frame, frame, 1);
	}

	for (size_t i = 0; i < faces.size(); i++)
	{

		double alpha = 0.2;

		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		Point pt1, pt2;
		pt1 = Point(faces[i].x, faces[i].y);
		pt2 = pt1 + Point(faces[i].width, faces[i].height);
		rectangle(frame, pt1, pt2, Scalar::all(255));
		rectangle(frame, pt1, pt1 + Point(faces[i].width, -20), Scalar::all(255), CV_FILLED);

		//cv::Mat roi = frame(cv::Rect(pt1, pt2));
		//cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(198, 98, 98));
		//cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);

		fx = fx*alp + center.x*(1 - alp);
		fy = fy*alp + center.y*(1 - alp);
			/*
		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;
		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		for (size_t j = 0; j < eyes.size(); j++)
		{
		Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
		int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
		circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
		}
		*/
	}
	//-- Show what you got
	//imshow(window_name, frame);
}
