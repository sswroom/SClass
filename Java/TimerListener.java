public class TimerListener implements java.awt.event.ActionListener
{
	private long me;
	public TimerListener(long me)
	{
		this.me = me;
	}

	public long getMe()
	{
		return this.me;
	}

	public native void actionPerformed(java.awt.event.ActionEvent e);
}

