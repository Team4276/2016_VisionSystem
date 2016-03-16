
package test_cv_eclipse;


/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the IterativeRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class Robot  {
	
	public static int g_nSequenceLidar = 0;
	public static double g_lidarDistanceCentimeters = 0.0;
	
	public static int g_nSequenceVisionSystem = 0;
	public static boolean g_isVisionSystemGoalDetected = false;
	public static double g_visionSystemAngleRobotToGoal = -181.0;
	public static double g_visionSystemPixelX = -1.0;

	public static boolean g_isImuDataValid = false;
	public static double g_imuYawDegrees = -181.00;

    /**
     * This function is run when the robot is first started up and should be
     * used for any initialization code.
     */
    public static void robotInit() {

     	
    	
    	JVisionSystemReceiverRunnable visionSystemRunnable = new JVisionSystemReceiverRunnable();
        Thread visionSystemThread = new Thread(visionSystemRunnable);
        visionSystemThread.start();   
        
        while(true) {
        	try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        }
    }

    /**
     * This function is called periodically during autonomous
     */
    public void autonomousPeriodic() {
    	
  
    }

    /**
     * This function is called periodically during operator control
     */
    public void teleopPeriodic() {
    	
    	
         
    }
    
    /**
     * This function is called periodically during test mode
     */
    public void testPeriodic() {
    
    }
    
}
