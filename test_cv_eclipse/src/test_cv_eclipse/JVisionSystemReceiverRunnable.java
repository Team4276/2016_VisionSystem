package test_cv_eclipse;


/**
 *
 * @author viking
 */
public class JVisionSystemReceiverRunnable implements Runnable 
{
    boolean m_continueRunning;

    JReceiver m_visionSystemReceiver;
    JTargetInfo m_visionSystemTargetInfo;

    @Override
    public void run() {
        m_visionSystemReceiver = new JReceiver();
        m_visionSystemTargetInfo = new JTargetInfo();
        m_visionSystemReceiver.init();

        int count = 0;
        String textInput;
        m_continueRunning = true;
        while(m_continueRunning) 
        {
            textInput = m_visionSystemReceiver.getOneLineFromSocket();
            if(!textInput.isEmpty())
            {
                Robot.g_nSequenceVisionSystem++;
                m_visionSystemTargetInfo.initTargetInfoFromText(textInput);
                //System.out.println(textInput);
                Robot.g_isVisionSystemGoalDetected = m_visionSystemTargetInfo.m_isUpperGoalFound;
                Robot.g_visionSystemAngleRobotToGoal = m_visionSystemTargetInfo.m_angleFromStraightAheadToUpperGoal;
                Robot.g_visionSystemPixelX = m_visionSystemTargetInfo.m_pixelX;
                
                String sMsg = count++ + ") ";
                if(Robot.g_isVisionSystemGoalDetected) {
                	sMsg += "*** FOUND *** Vision system Pixel X = " + Robot.g_visionSystemPixelX;    // Robot.g_visionSystemAngleRobotToGoal + "deg,   " + Robot.g_visionSystemPixelX + " inches";
                } 
                else
                {
                	sMsg += "              Upper Goal not found";
                }
                System.out.println(sMsg);
                
            }
            else
            {
                Robot.g_isVisionSystemGoalDetected = false;
            }
        }
    }
}