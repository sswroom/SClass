public class MyActionListener implements java.awt.event.ActionListener
{
	private long hdlr;
	private long userObj;
	public MyActionListener(long hdlr, long userObj)
	{
		this.hdlr = hdlr;
		this.userObj = userObj;
	}

	public long getHdlr()
	{
		return this.hdlr;
	}

	public long getUserObj()
	{
		return this.userObj;
	}

	public native void actionPerformed(java.awt.event.ActionEvent e);
}

