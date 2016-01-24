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


#ifndef CTARGETINFO_H
#define	CTARGETINFO_H

class CTargetInfo
{
public:
    CTargetInfo();
    CTargetInfo(const CTargetInfo& orig);
    virtual ~CTargetInfo();

    void init();

    void updateTargetInfo(
            int timeSinceLastCameraFrameMilliseconds,
            int timeLatencyThisCameraFrameMilliseconds, 
            bool isUpperGoalFound,
            float upperGoalDirectionDegrees,
            float upperGoalAngleDegrees,
            float distanceToGoalCenterInches);

    void initTargetInfoFromText(const std::string& targetInfoText);

    /* Not absolute time -- use for relative time between samples */
    int getTimeSinceLastCameraFrameMilliseconds()
    {
        return m_timeSinceLastCameraFrameMilliseconds;
    }

    int getTimeLatencyThisCameraFrameMilliseconds()
    {
        return m_timeLatencyThisCameraFrameMilliseconds;
    }

    /* 0 == not found */
    int isUpperGoalFound() const
    {
        return m_isUpperGoalFound;
    }

    std::string initFormattedTextFromTargetInfo();

private:
    int m_timeSinceLastCameraFrameMilliseconds;
    int m_timeLatencyThisCameraFrameMilliseconds;
    int m_isUpperGoalFound;
    
    // If this is zero the robot is pointed at the upper goal center. 
    // Positive (+) means upper goal is to the right, and the robot needs to rotate clockwise to reduce the angle to zero
    float m_upperGoalDirectionDegrees;            
    
    // Rotation of the upper goal around its center, relative to a straight line from the robot to the upper goal 
    // Positive (+) means upper goal is rotated clockwise and the robot needs to move left to reduce the angle to zero
    float m_upperGoalAngleDegrees;                  
    
    // As measured in a straight line from the center of the robot to the center of the upper goal
    // For autonomous operation want to arrive at a position where both angles are zero and this distance is a fixed offset "X"
    // "X" is TBD, depending on ideal distance from the upper goal to begin autonomous upper goal pickup
    float m_distanceToGoalCenterInches;      
};

#endif	/* CTARGETINFO_H */

