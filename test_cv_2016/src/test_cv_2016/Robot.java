/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package test_cv_2016;

/**
 *
 * @author viking
 */
public class Robot {
    
    // Global place for all sensor data...   
    public static int g_nSequenceVisionSystem = 0;
    public static boolean g_isVisionSystemGoalDetected = false;
    public static double g_visionSystemAngleRobotToGoal = -181.0;

    
    // Put the following in robotInit()
    public static void robotInit()
    {
        JVisionSystemReceiverRunnable visionSystemRunnable = new JVisionSystemReceiverRunnable();
        Thread visionSystemThread = new Thread(visionSystemRunnable);
        visionSystemThread.start();      
    }
    
}
