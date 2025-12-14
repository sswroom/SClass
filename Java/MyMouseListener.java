public class MyMouseListener implements java.awt.event.MouseListener
{
	private long mouseClickedHdlr;
	private long mouseEnteredHdlr;
	private long mouseExitedHdlr;
	private long mousePressedHdlr;
	private long mouseReleasedHdlr;
	private long userObj;
	public MyMouseListener(long userObj)
	{
		this.mouseClickedHdlr = 0;
		this.mouseEnteredHdlr = 0;
		this.mouseExitedHdlr = 0;
		this.mousePressedHdlr = 0;
		this.mouseReleasedHdlr = 0;
		this.userObj = userObj;
	}

	public long getMouseClickedHdlr()
	{
		return this.mouseClickedHdlr;
	}
	
	public void setMouseClickedHdlr(long hdlr)
	{
		this.mouseClickedHdlr = hdlr;
	}

	public long getMouseEnteredHdlr()
	{
		return this.mouseEnteredHdlr;
	}
	
	public void setMouseEnteredHdlr(long hdlr)
	{
		this.mouseEnteredHdlr = hdlr;
	}

	public long getMouseExitedHdlr()
	{
		return this.mouseExitedHdlr;
	}
	
	public void setMouseExitedHdlr(long hdlr)
	{
		this.mouseExitedHdlr = hdlr;
	}

	public long getMousePressedHdlr()
	{
		return this.mousePressedHdlr;
	}
	
	public void setMousePressedHdlr(long hdlr)
	{
		this.mousePressedHdlr = hdlr;
	}

	public long getMouseReleasedHdlr()
	{
		return this.mouseReleasedHdlr;
	}

	public void setMouseReleasedHdlr(long hdlr)
	{
		this.mouseReleasedHdlr = hdlr;
	}

	public long getUserObj()
	{
		return this.userObj;
	}

	public native void mouseClicked(java.awt.event.MouseEvent e);
	public native void mouseEntered(java.awt.event.MouseEvent e);
	public native void mouseExited(java.awt.event.MouseEvent e);
	public native void mousePressed(java.awt.event.MouseEvent e);
	public native void mouseReleased(java.awt.event.MouseEvent e);
}
