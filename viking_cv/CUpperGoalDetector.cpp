/*******************************************************************************************/
/* The MIT License (MIT)                                                                   */
/*                                                                                         */
/* Copyright (c) 2014 - Marina High School FIRST Robotics Team 4276 (Huntington Beach, CA) */
/*                                                                                         */
/* Permission is hereby granted, free of charge, to any person obtaining a copy            */
/* of this software and associated documentation files (the "Software"), to deal           */
/* in the Software without restriction, including without limitation the rights            */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell               */
/* copies of the Software, and to permit persons to whom the Software is                   */
/* furnished to do so, subject to the following conditions:                                */
/*                                                                                         */
/* The above copyright notice and this permission notice shall be included in              */
/* all copies or substantial portions of the Software.                                     */
/*                                                                                         */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR              */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                */

/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE             */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                  */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,           */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN               */
/* THE SOFTWARE.                                                                           */
/*******************************************************************************************/

/*******************************************************************************************/
/* We are a high school robotics team and always in need of financial support.             */
/* If you use this software for commercial purposes please return the favor and donate     */
/* (tax free) to "Marina High School Educational Foundation"  (Huntington Beach, CA)       */
/*******************************************************************************************/

#include <stdio.h>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "CUpperGoalRectangle.h"
#include "CTargetInfo.h"
#include "CVideoFrame.h"
#include "CVideoFrameQueue.h"
#include "CConnectionServer.h"
#include "CGpioLed.h"
#include "CUpperGoalDetector.h"
#include "CTestMonitor.h"
#include "CFrameGrinder.h"
#include "dbgMsg.h"
#include "viking_cv_version.h"

CUpperGoalDetector::CUpperGoalDetector()
{
}

CUpperGoalDetector::CUpperGoalDetector(const CUpperGoalDetector& orig)
{
}

CUpperGoalDetector::~CUpperGoalDetector()
{
}

void CUpperGoalDetector::init()
{
    m_tolerancePercentForRadius = 0.20;
}

void CUpperGoalDetector::detectBlobs(CVideoFrame * pFrame, CFrameGrinder* pFrameGrinder)
{
    try
    {
        static struct timespec timeLastCameraFrame = {0};
        static struct timespec timeNow = {0};
        cv::Mat img_hsv, goal_blob, dstA, dstB;
        static int iCount = 0;
        
        // 2016 filter values from Team 3019 - TowerTracker.java
        cv::Scalar lowerBounds = cv::Scalar(58,0,109);
	cv::Scalar upperBounds = cv::Scalar(93,255,240);

        int timeSinceLastCameraFrameMilliseconds = (int) CTestMonitor::getDeltaTimeMilliseconds(
                timeLastCameraFrame,
                pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT]);
        timeLastCameraFrame = pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT];

        // RBG is flawed as a way to filter based on color because the brightness is combined 
        // with the color info. 
        // Not so with HSV, where Hue and Saturation are maintained separately
        // OpenCV has a handy conversion from RGB to HSV
        cv::cvtColor(pFrame->m_frame, img_hsv, CV_BGR2HSV);

        // Find the bright response from the rettro-reflective tape
        cv::inRange(img_hsv, lowerBounds, upperBounds, goal_blob);
            
        iCount++;
        if ((iCount % 17) == 0)
        {
            pFrameGrinder->m_testMonitor.saveFrameToJpeg(goal_blob);
        }


        //Find the contours. Use the contourOutput Mat so the original image doesn't get overwritten
        std::vector<std::vector<cv::Point> > goalContours;
        cv::findContours(goal_blob, goalContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
        
        CUpperGoalRectangle bestupperGoalRectangle;
        float angleToBlueUpperGoalDegrees = 0.0;
        float offsetFromCenterlineToUpperGoalCenterUpperGoalFeet = 0.0;
        bool isUpperGoalFound = false;
        isUpperGoalFound = filterContoursLikeTowerTracker(goalContours, pFrame->m_frame.rows, pFrame->m_frame.cols,
                bestupperGoalRectangle, angleToBlueUpperGoalDegrees, offsetFromCenterlineToUpperGoalCenterUpperGoalFeet);
       
        CTestMonitor::getTicks(&timeNow);
        int timeLatencyThisCameraFrameMilliseconds = (int) CTestMonitor::getDeltaTimeMilliseconds(
                pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT],
                timeNow);

        pFrame->m_targetInfo.updateTargetInfo(
                timeSinceLastCameraFrameMilliseconds, timeLatencyThisCameraFrameMilliseconds, 
                isUpperGoalFound, angleToBlueUpperGoalDegrees, offsetFromCenterlineToUpperGoalCenterUpperGoalFeet);

        pFrame->updateAnnotationInfo(bestupperGoalRectangle);

        m_gpioLed.setGreenLED(isUpperGoalFound, pFrame->m_timeRemovedFromQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT]);
    }
    catch (...)
    {
    }
}

double CUpperGoalDetector::normalize360(double angle)
{
    while(angle >= 360.0)
    {
        angle -= 360.0;
    }
    while(angle < 0.0)
    {
        angle += 360.0;
    }
    return angle;
}

bool CUpperGoalDetector::filterContoursLikeTowerTracker(
        std::vector<std::vector<cv::Point> >& listContours,
        int originalMatHeight, int originalMatWidth,
        CUpperGoalRectangle& bestUpperGoalRectangle,
        float& angleToUpperGoalDegrees,
        float& distanceToUpperGoalFeet)
{
// Constants for known variables
// the height to the top of the target in first stronghold is 97 inches	
    static int TOP_TARGET_HEIGHT = 97;
//	the physical height of the camera lens
    static int TOP_CAMERA_HEIGHT = 32;

//	camera details, can usually be found on the datasheets of the camera
    static double VERTICAL_FOV  = 51;
    static double HORIZONTAL_FOV  = 67;
    static double CAMERA_ANGLE = 10;    
    
    static double PI = 3.14159265358979323846264;
    
    
    bool isUpperGoalFound = false;
    bestUpperGoalRectangle.init();
    angleToUpperGoalDegrees = -999.0;
    distanceToUpperGoalFeet = -1.0;

    unsigned int i = 0;
    CUpperGoalRectangle tempUpperGoalRectangle;
    double aspectRatio, diff, area, predict;
    cv::RotatedRect tempRect, vertRect, horizRect;
    std::vector<CUpperGoalRectangle> listPossibleUpperGoalRectangle;
    std::vector<cv::Point> contours_poly;
    for (std::vector<std::vector<cv::Point> >::iterator iter = listContours.begin() ; iter != listContours.end(); ++iter)
    {
        tempRect = cv::minAreaRect(cv::Mat(listContours[i]));
        if(tempRect.size.height < 25 || tempRect.size.width < 25)
        {
            listContours.erase(iter);
            continue;
        }
        float aspect = (float)tempRect.size.width/(float)tempRect.size.height;
        if(aspect < 1.0)
        {
            listContours.erase(iter);
            continue;
        }
    }
    isUpperGoalFound = (listContours.size() == 1);
    if(isUpperGoalFound)
    {
        tempRect = cv::minAreaRect(cv::Mat(listContours[i]));
        // "fun" math brought to you by miss daisy (team 341)!
        int y = tempRect.boundingRect().br().y + tempRect.boundingRect().height / 2;
        y= -((2 * (y / originalMatHeight)) - 1);
        distanceToUpperGoalFeet = (TOP_TARGET_HEIGHT - TOP_CAMERA_HEIGHT) / 
                        tan((y * VERTICAL_FOV / 2.0 + CAMERA_ANGLE) * PI / 180);
        //				angle to target...would not rely on this
        int targetX = tempRect.boundingRect().tl().x + tempRect.boundingRect().width / 2;
        targetX = (2 * (targetX / originalMatWidth)) - 1;
        angleToUpperGoalDegrees = normalize360(targetX*HORIZONTAL_FOV /2.0 + 0);
    }
    return isUpperGoalFound;
}
