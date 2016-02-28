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
        cv::Mat img_hsv, goal_blob;
        static int iCount = 0;
 
        // Look for the green hue wee are emitting from the LED halo 
        cv::Scalar lowerBounds = cv::Scalar(86,0,120);
	cv::Scalar upperBounds = cv::Scalar(94,255,255);

        int timeSinceLastCameraFrameMilliseconds = (int) CTestMonitor::getDeltaTimeMilliseconds(
                timeLastCameraFrame,
                pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT]);
        timeLastCameraFrame = pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT];

        // RBG is flawed as a way to filter based on color because the brightness is combined 
        // with the color info. 
        // Not so with HSV, where Hue and Saturation are maintained separately
        // OpenCV has a handy conversion from RGB to HSV
        cv::cvtColor(pFrame->m_frame, img_hsv, CV_BGR2HSV);

        // Find the bright response from the retro-reflective tape
        cv::inRange(img_hsv, lowerBounds, upperBounds, goal_blob);
            
        iCount++;
        if ((iCount % 17) == 0)
        {
            pFrameGrinder->m_testMonitor.saveFrameToJpeg(goal_blob);
        }

        //Find the contours. Use the contourOutput Mat so the original image doesn't get overwritten
        cv::vector<std::vector<cv::Point> > goalContours;
        cv::findContours(goal_blob, goalContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
        
        CUpperGoalRectangle upperGoalRectangle;
        float upperGoalAzimuthDegrees = 0.0;
        float distanceToUpperGoalInches = 0.0;
        bool isUpperGoalFound = false;
        isUpperGoalFound = filterContours(goalContours, pFrame->m_frame.rows, pFrame->m_frame.cols,
                upperGoalRectangle, upperGoalAzimuthDegrees, distanceToUpperGoalInches);
       
        CTestMonitor::getTicks(&timeNow);
        int timeLatencyThisCameraFrameMilliseconds = (int) CTestMonitor::getDeltaTimeMilliseconds(
                pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT],
                timeNow);

        pFrame->m_targetInfo.updateTargetInfo(
                timeSinceLastCameraFrameMilliseconds, timeLatencyThisCameraFrameMilliseconds, 
                isUpperGoalFound, upperGoalAzimuthDegrees, distanceToUpperGoalInches);

        pFrame->updateAnnotationInfo(upperGoalRectangle);

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

bool CUpperGoalDetector::filterContours(
        const std::vector<std::vector<cv::Point> >& listContours,
        int originalMatHeight, int originalMatWidth,
        CUpperGoalRectangle& upperGoalRectangle,
        float& upperGoalAzimuthDegrees,
        float& distanceToUpperGoalInches)
{
// Constants for known variables
// the height to the top of the target in first stronghold is 97 inches	
    static int TOP_TARGET_HEIGHT = 97;
//	the physical height of the camera lens
    static int TOP_CAMERA_HEIGHT = 16;

//	camera details, can usually be found on the data sheets of the camera
    static double VERTICAL_FOV  = 44;
    static double HORIZONTAL_FOV  = 78;
    static double CAMERA_ANGLE = 0;    
    
    static double PI = 3.14159265358979323846264;
   
    bool isUpperGoalFound = false;
    upperGoalRectangle.init();
    upperGoalAzimuthDegrees = -999.0;
    distanceToUpperGoalInches = -1.0;
   
    cv::RotatedRect tempRect;
    std::vector<cv::RotatedRect> listFilteredRect;
    for(int i = 0; i < listContours.size(); i++)
    {
    if (listContours.at(i).size() > 4 )  // ==2 contour  is a straight line, == 3 triangle, == 4 '4 sided' (still can't be a "U" shape)
        {
            std::vector<cv::Vec4i>  convexityDefectsSet;
            std::vector<int> hull;
            cv::convexHull(listContours.at(i), hull, false );
            if (hull.size() > 4 )  // ==2 hull is a straight line, == 3 triangle, == 4 '4 sided' (still can't be a "U" shape)
            {
                cv::convexityDefects(listContours.at(i), hull, convexityDefectsSet);
                tempRect = cv::minAreaRect(cv::Mat(listContours.at(i)));
                float aspect = (float)tempRect.size.width/(float)tempRect.size.height;
                if(   (   (tempRect.size.height >= 10) 
                       && (tempRect.size.width >= 18)  )
                   && (aspect >= 1.0) )
                {
                    //printf("w,h,a = %f\t%f\t%f\n",tempRect.size.width, tempRect.size.height, aspect);
                    bool bFound = false;
                    for(int j=0; j<convexityDefectsSet.size(); j++)
                    {    
                        // defect(start_index, end_index, farthest_pt_index, fixpt_depth)
                        //     start_index, end_index, farthest_pt_index are 0-based indices in the original contour of the convexity defect beginning, end and the farthest point
                        //     fixpt_depth is fixed-point approximation (with 8 fractional bits) of the distance between the farthest contour point and the hull. That is, to get the floating-point value of the depth will be fixpt_depth/256.0
                        //
                        // So... we look for a dent in the surrounding countour that is more than half the height
                        double depth = convexityDefectsSet[j][3]/256.0;
                        //printf("convexityDefectsSet[%d] (depth) = [ %d %d %d %d ]  (%f) \n", j, convexityDefectsSet[j][0], convexityDefectsSet[j][1], convexityDefectsSet[j][2], convexityDefectsSet[j][3], depth);
                        if(depth > (tempRect.size.height/2))
                        {
                            // Contour is concave (hoping for a "U")
                            bFound = true;
                        }
                    }
                    if(bFound) 
                    {
                        printf(" *** FOUND ***   w,h,a = %f\t%f\t%f\n",tempRect.size.width, tempRect.size.height, aspect);
                        listFilteredRect.push_back(tempRect);                    
                    }
                }
            }
        }
    }
    isUpperGoalFound = (listFilteredRect.size() == 1);
    if(isUpperGoalFound)
    {
        upperGoalRectangle = CUpperGoalRectangle(listFilteredRect[0]);
        
        // "fun" math brought to you by miss daisy (team 341)!
        int y = upperGoalRectangle.boundingRect().br().y + upperGoalRectangle.boundingRect().height / 2;
        y= -((2 * (y / originalMatHeight)) - 1);
        distanceToUpperGoalInches = (TOP_TARGET_HEIGHT - TOP_CAMERA_HEIGHT) / 
                        tan((y * VERTICAL_FOV / 2.0 + CAMERA_ANGLE) * PI / 180);
        //				angle to target...would not rely on this
        int targetX = upperGoalRectangle.boundingRect().tl().x + upperGoalRectangle.boundingRect().width / 2;
        targetX = (2 * (targetX / originalMatWidth)) - 1;
        upperGoalAzimuthDegrees = normalize360(targetX*HORIZONTAL_FOV /2.0 + 0);
    }
    return isUpperGoalFound;
}
