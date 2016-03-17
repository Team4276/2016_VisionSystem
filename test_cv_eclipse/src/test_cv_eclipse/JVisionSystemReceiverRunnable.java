package test_cv_eclipse;

/**
 *
 * @author viking
 */
public class JVisionSystemReceiverRunnable implements Runnable {
	boolean m_continueRunning;

	JReceiver m_visionSystemReceiver;
	JTargetInfo m_visionSystemTargetInfo;

	@Override
	public void run() {
		m_visionSystemReceiver = new JReceiver();
		m_visionSystemTargetInfo = new JTargetInfo();

		m_continueRunning = true;
		m_visionSystemReceiver.m_initOK = false;
		while (m_continueRunning) {
			while (m_visionSystemReceiver.m_initOK == false) {
				if (m_visionSystemReceiver.init()) {
					break;
				}
				System.out.println("Vision System connect failed - wait 2 sec and try again...");
				try {
					Thread.sleep(2000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}

			int count = 0;
			String textInput;
			try {
				textInput = m_visionSystemReceiver.getOneLineFromSocket();
				if (!textInput.isEmpty()) {
					Robot.g_nSequenceVisionSystem++;
					m_visionSystemTargetInfo.initTargetInfoFromText(textInput);
					// System.out.println(textInput);
					Robot.g_isVisionSystemGoalDetected = m_visionSystemTargetInfo.m_isUpperGoalFound;
					Robot.g_visionSystemAngleRobotToGoal = m_visionSystemTargetInfo.m_angleFromStraightAheadToUpperGoal;
					Robot.g_visionSystemPixelX = m_visionSystemTargetInfo.m_pixelX;

					String sMsg = count++ + ") ";
					if (Robot.g_isVisionSystemGoalDetected) {
						sMsg += "*** FOUND *** Vision system Pixel X = " + Robot.g_visionSystemPixelX; // Robot.g_visionSystemAngleRobotToGoal
					} else {
						sMsg += "              Upper Goal not found";
					}
					System.out.println(sMsg);

				} else {
					m_visionSystemReceiver.m_initOK = false;
					Robot.g_isVisionSystemGoalDetected = false;
				}
			} catch (Exception eGeneric) {
				System.out.println("Vision System processing exception: " + eGeneric.getMessage());
				try {
					Thread.sleep(100);
				} catch (InterruptedException eSleep) {
					eSleep.printStackTrace();
				}
				m_visionSystemReceiver.m_initOK = false;
			}
		}
	}
}
