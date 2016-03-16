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
    Boolean m_initOK = false;

    void init()
    {
        try
        {
            m_currentTargetInfo = new JTargetInfo();
            m_initOK = false;
            m_bbbTextSocket = new Socket(m_host, 5809);
            m_out = new PrintWriter(m_bbbTextSocket.getOutputStream(), true);
            m_in = new BufferedReader(new InputStreamReader(m_bbbTextSocket.getInputStream()));
        } catch (UnknownHostException e)
        {
            System.err.println("Don't know about host: " + m_host);
            m_initOK = false;

        } catch (IOException e)
        {
            System.err.println("Couldn't get I/O for the connection to: " + m_host);
            m_initOK = false;
        }
        if (m_out == null)
        {
            System.err.println("OUT == null");
        } else if (m_in == null)
        {
            System.err.println("IN == null");
            m_initOK = false;
        }

        m_initOK = true;
            
        m_out.println("GET");   // Tells the BeagleBone to start sending text
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
