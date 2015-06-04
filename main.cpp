#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame, int fd );
int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
int StrokeSize = 20;

/** @function main */
int main( )
{

    printf("Projet TX UTT - Lucquin Maxime et Bansais Alain\n");

    CvCapture* capture;
    Mat frame;

    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){
        printf("--(!)Error loading\nAjouter le fichier haarcascade_frontalface_alt.xml dans le dossier\n");
        return -1;
    };
    if( !eyes_cascade.load( eyes_cascade_name ) ){
        printf("--(!)Error loading\nAjouter le fichier haarcascade_eye_tree_eyeglasses.xml dans le dossier\n");
        return -1;
    };

    //-- 2. Param serial port for Arduino
    char *portname = "/dev/ttymxc3";


    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    //int fd=0;

    if (fd < 0)
    {
        printf("error %d opening %s: %s", errno, portname, strerror (errno));
        return -1;
    }
    set_interface_attribs (fd, B115200, 0); // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0); // set no blocking

    //-- 2. Read the video stream
    capture = cvCaptureFromCAM( -1 );
    if( capture )
    {
        while( true )
        {
            frame = cvQueryFrame( capture );

            //-- 3. Apply the classifier to the frame
            if( !frame.empty() )
            {
                detectAndDisplay( frame, fd );
            }
            else
            {
                printf(" --(!) No captured frame -- Break!"); break;
            }

            int c = waitKey(10);
            if( (char)c == 'c' ) {
                break;
            }
        }
    } else {
        printf("Webcam not detected");
    }
    return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame, int fd )
{
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //get size of the frame
    Size s = frame.size();
    int frHeight = s.height;
    int frWidht = s.width;
    //size of the srokes
    int verticalStroke = (frWidht*StrokeSize)/100;
    int horizontalStroke = (frHeight*StrokeSize)/100;
    printf("Frame x:%i  y:%i Stroke: vert%i  hor:%i \n",frWidht,frHeight,verticalStroke,horizontalStroke);

    Mat overlay=frame;
    // Draw stroke
    rectangle( frame, Point( 0, 0 ), Point( frWidht, horizontalStroke), Scalar( 255, 0,0 ), +2, 8 );//Top
    rectangle( frame, Point( 0, frHeight-horizontalStroke ), Point( frWidht, frHeight), Scalar( 255, 0,0 ), +2, 8 );//Bottom
    rectangle( frame, Point( 0, 0 ), Point( verticalStroke, frHeight), Scalar( 255, 0,0 ), +2, 8 );//Left
    rectangle( frame, Point( frWidht-verticalStroke, 0 ), Point( frWidht, frHeight), Scalar( 255, 0,0 ), +2, 8 );//Right

    char result[10]="";
    char chXaxis[10]="";
    char chYaxis[10]="";

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );


    //face[0] => select the first face
    if(faces.size()>0){
        Point center( faces[0].x + faces[0].width*0.5, faces[0].y + faces[0].height*0.5 );
        ellipse( frame, center, Size( faces[0].width*0.5, faces[0].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

        //get center of face
        int x = faces[0].x + faces[0].width*0.5;
        int y = faces[0].y + faces[0].height*0.5;
        int faceLeft = faces[0].x;
        int faceRight = faces[0].x + faces[0].width;
        int faceTop=faces[0].y;
        int faceBottom=faces[0].y + faces[0].height;
        int Xaxis=1;
        int Yaxis=1;

        printf("Face = x: %i   y:%i \n",x,y);

        //face at left part
        if(faceLeft<horizontalStroke){
            printf("Turn Left\n");
            Xaxis=0;
        }
        //face at right part
        if(faceRight>(frWidht-verticalStroke)){
            printf("Turn Right\n");
            Xaxis=2;

        }
        //face at top part
        if(faceTop<verticalStroke){
            printf("Turn Up\n");
            Yaxis=0;
        }
        //face at bottom part
        if(faceBottom>(frHeight-horizontalStroke)){
            printf("Turn Down\n");
            Yaxis=2;
        }


        //convert int to char
        sprintf(chXaxis, "%d", Xaxis);
        sprintf(chYaxis, "%d", Yaxis);
        //create char for serial
        strcat(result,  chXaxis);
        strcat(result, ",");
        strcat(result, chYaxis);
        strcat(result, ";");

        printf("Reuslt : %s\n", result);

        write(fd, result, 1);

        memset(&result[0], 0, sizeof(result));
        memset(&chXaxis[0], 0, sizeof(chXaxis));
        memset(&chYaxis[0], 0, sizeof(chYaxis));


        Mat faceROI = frame_gray( faces[0] );
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

        for( size_t j = 0; j < eyes.size(); j++ )
        {
            Point center( faces[0].x + eyes[j].x + eyes[j].width*0.5, faces[0].y + eyes[j].y + eyes[j].height*0.5 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        }
    }

    //-- Show what you got
    imshow( window_name, frame );
}

int set_interface_attribs (int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        printf ("error %d from tcgetattr", errno);
        return -1;
    }
    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK; // ignore break signal
    tty.c_lflag = 0; // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_cc[VMIN] = 0; // read doesn't block
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
        printf ("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

void set_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        printf ("error %d from tggetattr", errno);
        return;
    }
    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout
    if (tcsetattr (fd, TCSANOW, &tty) != 0)
        printf ("error %d setting term attributes", errno);
}
