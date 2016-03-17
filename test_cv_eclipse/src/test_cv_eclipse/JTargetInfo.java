/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package test_cv_eclipse;

/**
 *
 * @author Jesse Lozano
 */
public class JTargetInfo {

    boolean m_isUpperGoalFound;
    double m_angleFromStraightAheadToUpperGoal;
	double m_distanceToUpperGoal;
    double m_pixelX;
    
    int commaPos;
    String word;
    int num;
    double fnum;
    String s;

    public JTargetInfo() {
        init();
    }

    /*
     public void JTargetInfo(const JTargetInfo & orig)
     {
     init();
     }
     */
    public void init() {
        m_isUpperGoalFound = false;
        m_angleFromStraightAheadToUpperGoal = 0.0;
        m_distanceToUpperGoal = 0.0;
        m_pixelX = -1.0;

        commaPos = 0;
        word = "";
        num = 0;
        s = "";
    }

    public void setIsUpperGoalFound(boolean num) {
        m_isUpperGoalFound = num;
    }

    public boolean getIsUpperGoalFound() {
        return m_isUpperGoalFound;
    }
    
    public void setAngleFromStraightAheadToUpperGoal(double fnum) {
        m_angleFromStraightAheadToUpperGoal = fnum;
    }

    public double getAngleFromStraightAheadToUpperGoal() {
        return m_angleFromStraightAheadToUpperGoal;
    }

    public void setDistanceToUpperGoal(double fnum) {
        m_distanceToUpperGoal = fnum;
    }

    public double getDistanceToUpperGoal() {
        return m_distanceToUpperGoal;
    }

    public void setPixelX(double fnum) {
        m_pixelX = fnum;
    }

    public double getPixelX() {
        return m_pixelX;
    }
    
    public void initTargetInfoFromText(String txt) {
        String[] items = txt.split(",");
        if(items.length < 4) {
        	// Ignore initial "HTTP/1.0 200 OK" return and any other malformed packet data
        } else {
	        Integer iTemp = Integer.parseInt(items[0]);
	        setIsUpperGoalFound(iTemp != 0);
	        
	        iTemp = Integer.parseInt(items[1]);
	        setAngleFromStraightAheadToUpperGoal(iTemp);
	        
	        iTemp = Integer.parseInt(items[2]);
			setDistanceToUpperGoal(iTemp);
	        
	        iTemp = Integer.parseInt(items[3]);
	        setPixelX(iTemp);
        }
    }
}
