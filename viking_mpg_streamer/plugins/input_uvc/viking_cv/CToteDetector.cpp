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
        cv::Mat img_hsv, gray_blob, binary_blob, binary_blob_8u;
        static int iCount = 0;

        int timeSinceLastCameraFrameMilliseconds = (int) CTestMonitor::getDeltaTimeMilliseconds(
                timeLastCameraFrame,
                pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT]);
        timeLastCameraFrame = pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT];
       

        // RBG is flawed as a way to filter based on color because the brightness is combined 
        // with the color info. 
        // Not so with HSV, where Hue and Saturation are maintained separately
        // OpenCV has a handy conversion from RGB to HSV
        cv::cvtColor(pFrame->m_frame, img_hsv, CV_BGR2HSV);

        // Filter out all but Gray hue
        
#ifdef COLORFILTER_ZOOKS_CLASSROOM_LIFECAM
        cv::inRange(img_hsv, cv::Scalar(30, 15, 32), cv::Scalar(80, 220, 220), gray_blob);
#endif
        
#ifdef COLORFILTER_MARINA_HALLWAY_LIFECAM
        cv::inRange(img_hsv, cv::Scalar(30, 0, 90), cv::Scalar(220, 80, 160), gray_blob);
#endif
                   
#ifdef COLORFILTER_MARINA_HALLWAY_LOGITECH
        cv::inRange(img_hsv, cv::Scalar(30, 0, 120), cv::Scalar(220, 80, 160), gray_blob);
#endif
      
#ifdef COLORFILTER_LA_SPORTS_ARENA_LOGITECH
        cv::inRange(img_hsv, cv::Scalar(15, 0, 40), cv::Scalar(55, 70, 200), gray_blob);        
#endif
        iCount++;
        if ((iCount % 17) == 0)
        {
            pFrameGrinder->m_testMonitor.saveFrameToJpeg(gray_blob);
        }
      

        // make a kernel of 4x4, all 1's
        cv::Mat kernel = cv::Mat::ones(cv::Size(2, 2), CV_8U);
        cv::erode(gray_blob, gray_blob, kernel);

         //Find the contours. Use the contourOutput Mat so the original image doesn't get overwritten
        std::vector<std::vector<cv::Point> > grayContours;
        try
        {
            cv::findContours(gray_blob, grayContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
        }
        catch( cv::Exception& e )
        {
            const char* err_msg = e.what();
            dbgMsg_s(err_msg);
        }
        
        //const cv::Scalar colorGreen = cv::Scalar(0, 255, 0);
        //for (int i = 0; i < grayContours.size(); i++){
        //    cv::drawContours(pFrame->m_frame, grayContours, i, colorGreen, 1, 8);
        //}
        //pFrameGrinder->m_testMonitor.saveFrameToJpeg(pFrame->m_frame);

        CUpperGoalRectangle bestToteRectangleGray;
        float upperGoalDirectionDegrees = 0.0;
        float upperGoalAngleDegrees = 0.0;
        float distanceToGoalCenterInches = 0.0;
        bool isUpperGoalFound = false;
#ifdef DETECT_LARGEST_BLOB_NO_FILTER_BASED_ON_SIZE
        isUpperGoalFound = filterContoursToFindLargestBlob(grayContours, bestToteRectangleGray,
                                                           upperGoalDirectionDegrees, upperGoalAngleDegrees, distanceToGoalCenterInches);

#ifdef DISPLAY_CALIBRATION_INFO
        printf("viking_cv version %d.%d.%d", VERSION_YEAR, VERSION_INTERFACE, VERSION_BUILD);
        if (isUpperGoalFound)
        {
            printf("   NearFar_X: %d    LeftRight_Y: %d    Radius  %02f\r",
                    (int) bestToteRectangleGray.m_ptCenter.x,
                    (int) bestToteRectangleGray.m_ptCenter.y,
                    bestToteRectangleGray.m_radius);
        }
        else
        {
            printf("   Gray rectangle *NOT* found\r");
        }
#endif

#else
        isUpperGoalFound = filterContoursToFindToteBySize(grayContours, bestToteRectangleGray, angleToBlueToteDegrees, offsetFromCenterlineToToteCenterToteFeet);
#endif

        CTestMonitor::getTicks(&timeNow);
        int timeLatencyThisCameraFrameMilliseconds = (int) CTestMonitor::getDeltaTimeMilliseconds(
                pFrame->m_timeAddedToQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT],
                timeNow);

        pFrame->m_targetInfo.updateTargetInfo(
                timeSinceLastCameraFrameMilliseconds, timeLatencyThisCameraFrameMilliseconds, 
                isUpperGoalFound, upperGoalDirectionDegrees, upperGoalAngleDegrees, distanceToGoalCenterInches);

        pFrame->updateAnnotationInfo(bestToteRectangleGray);

        m_gpioLed.setGreenLED(isUpperGoalFound, pFrame->m_timeRemovedFromQueue[(int) CVideoFrame::FRAME_QUEUE_WAIT_FOR_BLOB_DETECT]);
    }
    catch (...)
    {
    }
}

bool CUpperGoalDetector::filterContoursToFindLargestBlob(
        const std::vector<std::vector<cv::Point> >& listContours,
        CUpperGoalRectangle& bestToteRectangle,
        float& upperGoalDirectionDegrees,
        float& upperGoalAngleDegrees,
        float& distanceToGoalCenterInches)
{
    const cv::Point2f robotCenter(VIEW_PIXEL_X_WIDTH + (PIXEL_OFFSET_FROM_CENTERLINE_TO_CAMERA), 
            VIEW_PIXEL_Y_HEIGHT + PIXEL_OFFSET_FROM_ROBOT_CENTER_TO_BOTTOM_OF_VIEW);
    
    bool isToteFound = false;
    bestToteRectangle.init();
    upperGoalDirectionDegrees = -999.0;
    upperGoalAngleDegrees = -999.0;
    distanceToGoalCenterInches = -1.0;

    unsigned int i = 0;
    CUpperGoalRectangle tempToteRectangle;
    double aspectRatio, diff, area, predict, max;
    cv::RotatedRect tempRect, vertRect, horizRect;
    std::vector<CUpperGoalRectangle> listPossibleToteRectangle;
    std::vector<cv::Point> contours_poly;
    max = 0.0;
    for (i = 0; i < listContours.size(); i++)
    {
        tempRect = cv::minAreaRect(cv::Mat(listContours[i]));
        area = tempRect.size.width * tempRect.size.height;
        if ((area > 6500.0) && (area < 8500.0))
        {
            if(max < area)
            {
                max = area;
                isToteFound = true;
                bestToteRectangle = tempRect;
            }
        }
    }
    if (isToteFound)
    {
        cv::Point2f diff = robotCenter - bestToteRectangle.center;
        distanceToGoalCenterInches = cv::sqrt(diff.x*diff.x + diff.y*diff.y);
        distanceToGoalCenterInches *= INCHES_PER_PIXEL;

        upperGoalDirectionDegrees = atan(diff.x/diff.y) * DEGREES_PER_RADIAN;
        
        upperGoalAngleDegrees = bestToteRectangle.angle;
    }
    return isToteFound;
}
