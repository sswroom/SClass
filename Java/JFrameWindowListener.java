public class JFrameWindowListener implements java.awt.event.WindowListener
{
	private long me;
	public JFrameWindowListener(long me)
	{
		this.me = me;
	}

	public long getMe()
	{
		return this.me;
	}

	public native void windowActivated(java.awt.event.WindowEvent e);
	public native void windowClosed(java.awt.event.WindowEvent e);
	public native void windowClosing(java.awt.event.WindowEvent e);
	public native void windowDeactivated(java.awt.event.WindowEvent e);
	public native void windowDeiconified(java.awt.event.WindowEvent e);
	public native void windowIconified(java.awt.event.WindowEvent e);
	public native void windowOpened(java.awt.event.WindowEvent e);
}

