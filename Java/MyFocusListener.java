public class MyFocusListener implements java.awt.event.FocusListener
{
	private long focusGainHdlr;
	private long focusLostHdlr;
	private long userObj;
	public MyFocusListener(long userObj)
	{
		this.focusGainHdlr = 0;
		this.focusLostHdlr = 0;
		this.userObj = userObj;
	}

	public long getFocusGainHdlr()
	{
		return this.focusGainHdlr;
	}
	
	public void setFocusGainHdlr(long hdlr)
	{
		this.focusGainHdlr = hdlr;
	}

	public long getFocusLostHdlr()
	{
		return this.focusLostHdlr;
	}

	public void setFocusLostHdlr(long hdlr)
	{
		this.focusLostHdlr = hdlr;
	}

	public long getUserObj()
	{
		return this.userObj;
	}

	public native void focusGained(java.awt.event.FocusEvent e);
	public native void focusLost(java.awt.event.FocusEvent e);
}

