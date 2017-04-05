#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace cv;
using namespace std;

//GLOBALS
VideoCapture vcap;
Mat frame;
pthread_mutex_t frameLocker;

//const string videoStreamAddress = "http://192.168.1.4:8080:video.mjpg";
const string videoStreamAddress = 0;

//
/** Global variables */
String face_cascade_name = "lbpcascade_frontalface.xml";
String profile_cascade_name = "lbpcascade_profileface.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier profile_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face  \n detection";
Mat frame_gray;
Mat currentFrame;
std::vector<Rect> faces1;
std::vector<Rect> faces2;
std::vector<Rect> faces3;
Point center, pt1, pt2;
void *UpdateFrame(void *arg)
{
	for (;;)
	{
		Mat tempFrame;
		vcap >> tempFrame;
		pthread_mutex_lock(&frameLocker);
		frame = tempFrame;
		cvtColor(frame, frame_gray, CV_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);
		pthread_mutex_unlock(&frameLocker);
		usleep(50);
	}
}
void *proc1(void *arg)
{
	for (;;)
	{
		if (!frame_gray.empty())
		{
			//faces2.clear();
			profile_cascade.detectMultiScale(frame_gray, faces2, 1.1, 3, 0 | 15, Size(5, 5));
		}
		usleep(100);
	}
}
void *proc2(void *arg)
{
	for (;;)
	{
		if (!frame_gray.empty())
		{
			//faces1.clear();
			Mat tFrame;
			flip(frame_gray,tFrame,1);
			face_cascade.detectMultiScale(tFrame, faces3, 1.1, 3, 0 | 15, Size(5, 5));
		}
		usleep(50);
	}
}
void *proc3(void *arg)
{
	for (;;)
	{
		if (!frame_gray.empty())
		{
			//faces3.clear();
			face_cascade.detectMultiScale(frame_gray, faces3, 1.1, 3, 0 | 15, Size(5, 5));
		}
		usleep(50);
	}
}

int main(int, char**) {
	//	vcap.open(videoStreamAddress);
	vcap.open(0);
	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading Ek \n"); return -1; };
	if (!profile_cascade.load(profile_cascade_name)){ printf("--(!)Error loading Do\n"); return -1; };

	pthread_mutex_init(&frameLocker, NULL);
	pthread_t UpdThread;
	pthread_create(&UpdThread, NULL, UpdateFrame, NULL);
	pthread_t thread1;
	pthread_create(&thread1, NULL, proc1, NULL);
	pthread_t thread2;
	pthread_create(&thread2, NULL, proc2, NULL);
	pthread_t thread3;
	pthread_create(&thread3, NULL, proc3, NULL);


//	int rc1 = pthread_create(&threadF, NULL, UPD, NULL);

	for (;;)
	{
		pthread_mutex_lock(&frameLocker);
		currentFrame = frame;
		pthread_mutex_unlock(&frameLocker);

		if (currentFrame.empty() && faces1.size() == 0 && faces2.size() == 0)
		{
			printf("recieved empty frame main \n");
			continue;
		}
		else
		{
			for (short i = 0; i < faces1.size(); i++)
			{
				double alpha = 0.2;
				
				center =Point(faces1[i].x + faces1[i].width*0.5, faces1[i].y + faces1[i].height*0.5);
				//Point pt1, pt2;
				pt1 = Point(faces1[i].x, faces1[i].y);
				pt2 = pt1 + Point(faces1[i].width, faces1[i].height);
				rectangle(currentFrame, pt1, pt2, Scalar::all(128));
				rectangle(currentFrame, pt1, pt1 + Point(faces1[i].width, -20), Scalar::all(128), CV_FILLED);

				cv::Mat roi = currentFrame(cv::Rect(pt1, pt2));
				cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 255));
				cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
				printf("Front\n");
				usleep(10);
			}
			for (short i = 0; i < faces2.size(); i++)
			{
				double alpha = 0.2;
				Point center(faces2[i].x + faces2[i].width*0.5, faces2[i].y + faces2[i].height*0.5);
				Point pt1, pt2;
				pt1 = Point(faces2[i].x, faces2[i].y);
				pt2 = pt1 + Point(faces2[i].width, faces2[i].height);
				rectangle(currentFrame, pt1, pt2, Scalar::all(128));
				rectangle(currentFrame, pt1, pt1 + Point(faces2[i].width, -20), Scalar::all(128), CV_FILLED);

				cv::Mat roi = currentFrame(cv::Rect(pt1, pt2));
				cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(255, 0, 0));
				cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
				printf("Left\n");
				usleep(10);
			}
			for (size_t i = 0; i < faces3.size(); i++)
			{
				double alpha = 0.2;
				Point center(faces3[i].x + faces3[i].width*0.5, faces3[i].y + faces3[i].height*0.5);
				Point pt1, pt2;
				pt1 = Point(faces3[i].x, faces3[i].y);
				pt2 = pt1 + Point(faces3[i].width, faces3[i].height);
				rectangle(currentFrame, pt1, pt2, Scalar::all(128));
				rectangle(currentFrame, pt1, pt1 + Point(faces3[i].width, -20), Scalar::all(128), CV_FILLED);

				cv::Mat roi = currentFrame(cv::Rect(pt1, pt2));
				cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(255, 0, 0));
				cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
				printf("Right\n");
				usleep(10);
			}
			imshow("FrontFrame", currentFrame);	
usleep(1000);	
		}
		waitKey(1);
	}

}

/*
void *processFront(void *arg)
{
	for (;;)
	{
		std::vector<Rect> faces;
		Mat frame_gray;

		cvtColor(frame, frame_gray, CV_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		//-- Detect front faces
		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | 1, Size(10, 10));

		for (size_t i = 0; i < faces.size(); i++)
		{

			double alpha = 0.2;

			Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
			Point pt1, pt2;
			pt1 = Point(faces[i].x, faces[i].y);
			pt2 = pt1 + Point(faces[i].width, faces[i].height);
			rectangle(frame, pt1, pt2, Scalar::all(128));
			rectangle(frame, pt1, pt1 + Point(faces[i].width, -20), Scalar::all(128), CV_FILLED);

			cv::Mat roi = frame(cv::Rect(pt1, pt2));
			cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(198, 98, 98));
			cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
		}
	}
	imshow("FrontFrame", frame);

}*/
