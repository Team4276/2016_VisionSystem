/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package test_cv_2016;

/**
 *
 * @author Jesse Lozano
 */
public class JTargetInfo {

    boolean m_isUpperGoalFound;
    double m_angleFromStraightAheadToUpperGoal;
    double m_distanceToUpperGoal;
    
    int commaPos;
    String word;
    int num;
    double fnum;
    String s;

    public void JTargetInfo() {
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

        commaPos = 0;
        word = "";
        num = 0;
        s = "";
        double fnum = 0.0;
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
    
    public void initTargetInfoFromText(String txt) {
        String[] items = txt.split(",");
        int iTemp = Integer.getInteger(items[0]);
        setIsUpperGoalFound(iTemp != 0);
        
        iTemp = Integer.getInteger(items[1]);
        setAngleFromStraightAheadToUpperGoal(iTemp);
        
        iTemp = Integer.getInteger(items[2]);
        setDistanceToUpperGoal(iTemp);
    }
}
