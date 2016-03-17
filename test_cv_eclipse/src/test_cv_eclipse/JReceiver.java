/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package test_cv_eclipse;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author acappon
 */
public class JReceiver
{

    JTargetInfo m_currentTargetInfo = null;
    String m_host = "10.42.76.12";
    Socket m_bbbTextSocket = null;
    PrintWriter m_out = null;
    BufferedReader m_in = null;
    Boolean m_initOK = true;

    boolean init()
    {
        try
        {
            m_currentTargetInfo = new JTargetInfo();
            m_initOK = true;
			m_bbbTextSocket = new Socket(m_host, 5809);
            m_out = new PrintWriter(m_bbbTextSocket.getOutputStream(), true);
            m_in = new BufferedReader(new InputStreamReader(m_bbbTextSocket.getInputStream()));
        } catch (UnknownHostException e)
        {
            System.err.println("UnknownHostException Exception while creating connection to host: " + m_host + "\n" + e.getMessage());
            m_initOK = false;

        } catch (IOException e)        
        {
            System.err.println("I/O Exception while creating connection to host: " + m_host + "\n" + e.getMessage());
            m_initOK = false;
        } catch (Exception e)        
	    {
	        System.err.println("Generic Exception while creating connection to host: " + m_host + "\n" + e.getMessage());
	        m_initOK = false;
	    } 
        if (m_out == null)
        {
        	System.err.println("Couldn't get PrintWriter for the connection to: " + m_host);
        } else if (m_in == null)
        {
            System.err.println("Couldn't get BufferedReader for the connection to: " + m_host);
            m_initOK = false;
        }
        if(m_initOK) {
        	m_out.println("GET");   // Tells the BeagleBone to start sending text
        }  else {
            if (m_currentTargetInfo != null)
            {
            	m_currentTargetInfo = null;
            }
            if (m_bbbTextSocket != null)
            {
            	m_bbbTextSocket = null;
            }
            if (m_out != null)
            {
            	m_out = null;
            }
            if (m_in != null)
            {
            	m_in = null;
            }
        }
        return m_initOK;
    }
    
	String getOneLineFromSocket()
    {
        String textInput;
        try {
            textInput = m_in.readLine();
        } catch (IOException ex) {
            Logger.getLogger(JReceiver.class.getName()).log(Level.SEVERE, null, ex);
			textInput = "";
        }
       return textInput;
    }
}
