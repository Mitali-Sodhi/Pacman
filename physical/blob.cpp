// TrackColour.cpp : Defines the entry point for the console application.
//

// g++ -ggdb `pkg-config --cflags opencv` -o `basename blob.cpp .cpp` blob.cpp `pkg-config --libs opencv`

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

IplImage* GetThresholdedImage(IplImage* img, int color)
{
    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);


    if (color == 1) // yellow
        cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);


    if (color == 2) //blue
        cvInRangeS(imgHSV, cvScalar(90, 100, 100), cvScalar(120, 255, 255),
                imgThreshed);

    cvReleaseImage(&imgHSV);

    return imgThreshed;
}



int main()
{
    // Initialize capturing live feed from the camera
    CvCapture* capture = 0;

    //capture = cvCaptureFromFile("http://admin:camera@192.168.0.20/image.jpg");
    capture = cvCaptureFromCAM(0);
    assert(capture);


    if(!capture)
    {
        printf("Could not initialize capturing...\n");
        return -1;
    }

    // added for socket connections
    struct sockaddr_in pacCli, ghostCli;
    int n,sdPac, sdGhost;
    msg_t msgPac, msgGhost;
    
    // if((sdPac =socket(PF_INET,SOCK_STREAM,0))==-1) 
    //     {
    //     perror("socket");
    //     return(1);
    //     }

    // pacCli.sin_family=PF_INET;
    // pacCli.sin_port=htons(PACPORT);
    // pacCli.sin_addr.s_addr=inet_addr("127.0.0.1");

    // if(connect(sdPac,(struct sockaddr *)&pacCli,sizeof(pacCli))==-1) 
    //     {
    //     perror("connect");
    //     return(1);
    //     }   


    cvNamedWindow("video");
    cvNamedWindow("thresh yellow");
    cvNamedWindow("thresh blue");

    IplImage* imgScribble = NULL;

    while(true)
    {

        IplImage* frame = 0;
        //capture = cvCaptureFromFile("http://admin:camera@192.168.0.20/image.jpg");
        frame = cvQueryFrame(capture);

        if(!frame)
            break;
        
        if(imgScribble == NULL)
        {
            imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);
        }


        // Holds the thresholded image (color = white, rest = black)
        IplImage* imgYellowThresh = GetThresholdedImage(frame, 1);
        IplImage* imgBlueThresh = GetThresholdedImage(frame, 2);


        // Calculate the moments to estimate the position of the ball
        CvMoments *moments_yellow = (CvMoments*)malloc(sizeof(CvMoments));
        CvMoments *moments_blue = (CvMoments*) malloc(sizeof(CvMoments));

        cvMoments(imgYellowThresh, moments_yellow, 1);
        cvMoments(imgBlueThresh, moments_blue, 1);

        // The actual moment values
        double moment10y = cvGetSpatialMoment(moments_yellow, 1, 0);
        double moment01y = cvGetSpatialMoment(moments_yellow, 0, 1);
        double areay = cvGetCentralMoment(moments_yellow, 0, 0);

        double moment10b = cvGetSpatialMoment(moments_blue, 1, 0);
        double moment01b = cvGetSpatialMoment(moments_blue, 0, 1);
        double areab = cvGetCentralMoment(moments_blue, 0, 0);

        //* Yellow processing    *//
       
        static int posXy = 0;
        static int posYy = 0;

        int lastXy = posXy;
        int lastYy = posYy;

        int tempXy = moment10y / areay;
        int tempYy = moment01y / areay;

        if ( tempXy >= 0 && tempYy >= 0  && tempXy < 700 && tempYy < 700 && areay>1000 )
        {
            posXy = moment10y / areay;
            posYy = moment01y / areay;
        }
        std::cout << std::endl;
        printf("position yellow (%d,%d)\n", posXy, posYy);
        msgPac.x = posXy;
        msgPac.y = posYy;
        msgPac.yaw = atan2(posXy,posYy);

        

        //* Blue Processing  *//

        static int posXb = 0;
        static int posYb = 0;

        int lastXb = posXb;
        int lastYb = posYb;

        int tempXb = moment10b / areab;
        int tempYb = moment01b / areab;

        if ( tempXb >= 0 && tempYb >= 0  && tempXb < 700 && tempYb < 700 && areab>700 )
        {
            posXb = moment10b / areab;
            posYb = moment01b / areab;
        }

        printf("position blue (%d,%d)\n", posXb, posYb);
        msgPac.other_x = posXb;
        msgPac.other_y = posYb;
        msgPac.other_yaw = atan2(posXb,posYb);



        // Add the scribbling image and the frame... and we get a combination of the two colors
        cvAdd(frame, imgScribble, frame);
        cvShowImage("thresh yellow", imgYellowThresh);
        cvShowImage("thresh blue", imgBlueThresh);
        cvShowImage("video", frame);

        //send(sdPac,(const void*)&msgPac,sizeof(msgPac),0); 

        // Release the thresholded image
        cvReleaseImage(&imgYellowThresh);
        cvReleaseImage(&imgBlueThresh);

        delete moments_yellow;
        delete moments_blue;

    }

    cvReleaseCapture(&capture);
    return 0;
}