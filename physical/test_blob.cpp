// TrackColour.cpp : Defines the entry point for the console application.
//
#include "cv.h"
#include "highgui.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PACPORT 1034
#define GHOSTPORT 1035

typedef struct msg_s
{
    double x;
    double y;
    double yaw;
    double other_x;
    double other_y;
    double other_yaw;
}msg_t;

IplImage* GetThresholdedImage(IplImage* img)
{
	// Convert the image into an HSV image
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);

	IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

	// Values 20,100,100 to 30,255,255 working perfect for yellow at around 6pm
	cvInRangeS(imgHSV, cvScalar(130, 100, 100), cvScalar(170, 255, 255), imgThreshed);

	cvReleaseImage(&imgHSV);

	return imgThreshed;
}

int main()
{
	// Initialize capturing live feed from the camera
	CvCapture* capture = 0;
	capture = cvCaptureFromFile("http://admin:camera@192.168.0.20/image.jpg");
	//capture = cvCaptureFromCAM(0);

	// Couldn't get a device? Throw an error and quit
	if(!capture)
    {
        printf("Could not initialize capturing...\n");
        return -1;
    }

    // added for socket connections
    struct sockaddr_in pacCli, ghostCli;
    int n,sdPac, sdGhost;
    msg_t msgPac, msgGhost;

    if((sdPac =socket(PF_INET,SOCK_STREAM,0))==-1)
        {
        perror("socket");
        return(1);
        }

    pacCli.sin_family=PF_INET;
    pacCli.sin_port=htons(PACPORT);
    pacCli.sin_addr.s_addr=inet_addr("127.0.0.1");

    if(connect(sdPac,(struct sockaddr *)&pacCli,sizeof(pacCli))==-1)
        {
        perror("connect");
        return(1);
        }

	// The two windows we'll be using
    cvNamedWindow("video");
	cvNamedWindow("thresh");

	// This image holds the "scribble" data...
	// the tracked positions of the ball
	IplImage* imgScribble = NULL;

	msgPac.other_x = 0.0;
    msgPac.other_y = 0.0;
    msgPac.other_yaw = 0.0;

	// An infinite loop
	while(true)
    {
		// Will hold a frame captured from the camera
		IplImage* frame = 0;
		capture = cvCaptureFromFile("http://admin:camera@192.168.0.20/image.jpg");
		frame = cvQueryFrame(capture);

		// If we couldn't grab a frame... quit
        if(!frame)
            break;

		// If this is the first frame, we need to initialize it
		if(imgScribble == NULL)
		{
			imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);
		}

		// Holds the yellow thresholded image (yellow = white, rest = black)
		IplImage* imgYellowThresh = GetThresholdedImage(frame);

		// Calculate the moments to estimate the position of the ball
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		cvMoments(imgYellowThresh, moments, 1);

		// The actual moment values
		double moment10 = cvGetSpatialMoment(moments, 1, 0);
		double moment01 = cvGetSpatialMoment(moments, 0, 1);
		double area = cvGetCentralMoment(moments, 0, 0);

		// Holding the last and current ball positions
		static int posX = 0;
		static int posY = 0;

		int lastX = posX;
		int lastY = posY;

		posX = moment10/area;
		posY = moment01/area;

		// Print it out for debugging purposes
		printf("position (%d,%d)\n", posX, posY);

		msgPac.x = posX;
        msgPac.y = posY;
        msgPac.yaw = atan2(posX,posY);

		// We want to draw a line only if its a valid position
		if(lastX>0 && lastY>0 && posX>0 && posY>0)
		{
			// Draw a yellow line from the previous point to the current point
			cvLine(imgScribble, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,255,255), 5);
		}

		// Add the scribbling image and the frame... and we get a combination of the two
		cvAdd(frame, imgScribble, frame);
		cvShowImage("thresh", imgYellowThresh);
		cvShowImage("video", frame);

		// Wait for a keypress
		int c = cvWaitKey(10);
		if(c!=-1)
		{
			// If pressed, break out of the loop
            break;
		}

		send(sdPac,(const void*)&msgPac,sizeof(msgPac),0);

		// Release the thresholded image
		cvReleaseImage(&imgYellowThresh);

		delete moments;

		cvShowImage("video", frame);
    }

	cvReleaseCapture(&capture);
    return 0;
}
