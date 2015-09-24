package ls.client;

import android.util.Log;
/**
 * 
 * @author Ls Mouse
 *
 */
public class TimeLog {
	static int m_i = 0;
	static public  void Log(){
		
		Log.i("Time" +m_i, System.currentTimeMillis()+"");
		m_i++;
	}
}
