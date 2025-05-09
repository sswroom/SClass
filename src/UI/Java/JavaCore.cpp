#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
//#include "Media/JavaDrawEngine.h"
#include "Media/DrawEngineFactory.h"
#include "UI/GUICore.h"
#include "UI/Java/JavaButton.h"
#include "UI/Java/JavaCheckBox.h"
#include "UI/Java/JavaCheckedListBox.h"
#include "UI/Java/JavaComboBox.h"
#include "UI/Java/JavaCore.h"
#include "UI/Java/JavaDateTimePicker.h"
#include "UI/Java/JavaFileDialog.h"
#include "UI/Java/JavaFolderDialog.h"
#include "UI/Java/JavaFontDialog.h"
#include "UI/Java/JavaGroupBox.h"
#include "UI/Java/JavaHScrollBar.h"
#include "UI/Java/JavaHSplitter.h"
#include "UI/Java/JavaLabel.h"
#include "UI/Java/JavaListBox.h"
#include "UI/Java/JavaMessageDialog.h"
#include "UI/Java/JavaPanelBase.h"
#include "UI/Java/JavaPictureBox.h"
#include "UI/Java/JavaPictureBoxSimple.h"
#include "UI/Java/JavaProgressBar.h"
#include "UI/Java/JavaRadioButton.h"
#include "UI/Java/JavaRealtimeLineChart.h"
#include "UI/Java/JavaTabControl.h"
#include "UI/Java/JavaTextBox.h"
#include "UI/Java/JavaTrackBar.h"
#include "UI/Java/JavaVSplitter.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern void *jniEnv;
}

UI::Java::JavaCore::JavaCore()
{
	this->monMgr = 0;
	this->toExit = false;
}

UI::Java::JavaCore::~JavaCore()
{
}

void UI::Java::JavaCore::Run()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass eqCls = env->FindClass("java/awt/EventQueue");
	jmethodID mid = env->GetStaticMethodID(eqCls, "isDispatchThread", "()Z");
	if (env->CallStaticBooleanMethod(eqCls, mid))
	{
		jclass cls = env->FindClass("java/awt/Toolkit");
		mid = env->GetStaticMethodID(cls, "getDefaultToolkit", "()Ljava/awt/Toolkit;");
		jobject tk = env->CallStaticObjectMethod(cls, mid);
		cls = env->GetObjectClass(tk);
		mid = env->GetMethodID(cls, "getSystemEventQueue", "()Ljava/awt/EventQueue;");
		jobject eq = env->CallObjectMethod(tk, mid);
		mid = env->GetMethodID(eqCls, "<init>", "()V");
		jobject eq2 = env->NewObject(eqCls, mid);
		mid = env->GetMethodID(eqCls, "push", "(Ljava/awt/EventQueue;)V");
		env->CallVoidMethod(eq, mid, eq2);
		printf("Run Dispatch Thread\r\n");
		while (!this->toExit)
		{
			this->waitEvt.Wait(1000);
		}
		printf("Run Dispatch Thread End\r\n");
		mid = env->GetMethodID(eqCls, "pop", "()V");
		env->CallVoidMethod(eq2, mid);
	}
	else
	{
		printf("Run\r\n");
		while (!this->toExit)
		{
			this->waitEvt.Wait(1000);
		}
		printf("Run End\r\n");
	}
}

void UI::Java::JavaCore::ProcessMessages()
{
}

void UI::Java::JavaCore::WaitForMessages()
{
}

void UI::Java::JavaCore::Exit()
{
	this->toExit = true;
	this->waitEvt.Set();
}

NN<Media::DrawEngine> UI::Java::JavaCore::CreateDrawEngine()
{
//	NN<Media::DrawEngine> deng = 0;
//	NEW_CLASS(deng, Media::GTKDrawEngine());
	return Media::DrawEngineFactory::CreateDrawEngine();
}

Double UI::Java::JavaCore::GetMagnifyRatio(MonitorHandle *hMonitor)
{
/*	Double v = gdk_screen_get_resolution(gdk_screen_get_default());
	if (v <= 0)
		v = 1.0;
	return v;*/
	return 1.0;
}

void UI::Java::JavaCore::UseDevice(Bool useSystem, Bool useDisplay)
{
}

void UI::Java::JavaCore::SetNoDisplayOff(Bool noDispOff)
{
}

void UI::Java::JavaCore::DisplayOff()
{
}

void UI::Java::JavaCore::Suspend()
{
}

Math::Size2D<UOSInt> UI::Java::JavaCore::GetDesktopSize()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls;
	jmethodID mid;
	cls = env->FindClass("java/awt/Toolkit");
	mid = env->GetStaticMethodID(cls, "getDefaultToolkit", "()Ljava/awt/Toolkit;");
	jobject tk = env->CallStaticObjectMethod(cls, mid);
	cls = env->GetObjectClass(tk);
	mid = env->GetMethodID(cls, "getScreenSize", "()Ljava/awt/Dimension");
	jobject size = env->CallObjectMethod(tk, mid);
	UOSInt w;
	UOSInt h;
	mid = env->GetMethodID(cls, "getWidth", "()D");
	w = (UOSInt)Double2OSInt(env->CallDoubleMethod(size, mid));
	mid = env->GetMethodID(cls, "getHeight", "()D");
	h = (UOSInt)Double2OSInt(env->CallDoubleMethod(size, mid));
	return Math::Size2D<UOSInt>(w, h);
}

Math::Coord2D<OSInt> UI::Java::JavaCore::GetCursorPos()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls;
	jmethodID mid;
	cls = env->FindClass("java/awt/MouseInfo");
	mid = env->GetStaticMethodID(cls, "getPointerInfo", "()Ljava/awt/PointerInfo;");
	jobject pointer = env->CallStaticObjectMethod(cls, mid); //PointerInfo
	cls = env->GetObjectClass(pointer);
	mid = env->GetMethodID(cls, "getLocation", "()Ljava/awt/Point;");
	jobject loc = env->CallObjectMethod(pointer, mid);
	cls = env->GetObjectClass(loc);
	OSInt x;
	OSInt y;
	mid = env->GetMethodID(cls, "getX", "()D");
	x = Double2OSInt(env->CallDoubleMethod(loc, mid));
	mid = env->GetMethodID(cls, "getY", "()D");
	y = Double2OSInt(env->CallDoubleMethod(loc, mid));
	return Math::Coord2D<OSInt>(x, y);
}

void UI::Java::JavaCore::SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot)
{
}

void UI::Java::JavaCore::GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi)
{
	if (this->monMgr)
	{
		if (hdpi)
			*hdpi = this->monMgr->GetMonitorHDPI(hMonitor);
		if (ddpi)
			*ddpi = this->monMgr->GetMonitorDDPI(hMonitor);
	}
	else
	{
		if (hdpi)
			*hdpi = 96.0;
		if (ddpi)
			*ddpi = 96.0;
	}
}

void UI::Java::JavaCore::SetMonitorMgr(Media::MonitorMgr *monMgr)
{
	this->monMgr = monMgr;
}

Media::MonitorMgr *UI::Java::JavaCore::GetMonitorMgr()
{
	return this->monMgr;
}

Bool UI::Java::JavaCore::IsForwarded()
{
	return getenv("SSH_CLIENT") != 0;
}

Int32 UI::Java::JavaCore::GetScrollBarSize()
{
	return 16;
}

void UI::Java::JavaCore::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::Java::JavaMessageDialog::ShowOK((JNIEnv*)jniEnv, message, title, ctrl);
}

Bool UI::Java::JavaCore::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::Java::JavaMessageDialog::ShowYesNo((JNIEnv*)jniEnv, message, title, ctrl);
}

NN<UI::GUIButton> UI::Java::JavaCore::NewButton(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::Java::JavaButton> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaButton(*this, parent, text));
	return ctrl;
}

NN<UI::GUICheckBox> UI::Java::JavaCore::NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked)
{
	NN<UI::Java::JavaCheckBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaCheckBox(*this, parent, text, checked));
	return ctrl;
}

NN<UI::GUICheckedListBox> UI::Java::JavaCore::NewCheckedListBox(NN<GUIClientControl> parent)
{
	NN<UI::Java::JavaCheckedListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaCheckedListBox(*this, parent));
	return ctrl;
}

NN<UI::GUIComboBox> UI::Java::JavaCore::NewComboBox(NN<GUIClientControl> parent, Bool allowEdit)
{
	NN<UI::Java::JavaComboBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaComboBox(*this, parent, allowEdit));
	return ctrl;
}

NN<UI::GUIDateTimePicker> UI::Java::JavaCore::NewDateTimePicker(NN<GUIClientControl> parent, Bool calendarSelect)
{
	NN<UI::Java::JavaDateTimePicker> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaDateTimePicker(*this, parent));
	return ctrl;
}

NN<UI::GUIGroupBox> UI::Java::JavaCore::NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::Java::JavaGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaGroupBox(*this, parent, text));
	return ctrl;
}

NN<UI::GUIHScrollBar> UI::Java::JavaCore::NewHScrollBar(NN<GUIClientControl> parent, Double width)
{
	NN<UI::Java::JavaHScrollBar> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaHScrollBar(*this, parent, width));
	return ctrl;
}

NN<UI::GUILabel> UI::Java::JavaCore::NewLabel(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::Java::JavaLabel> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaLabel(*this, parent, text));
	return ctrl;
}

NN<UI::GUIListBox> UI::Java::JavaCore::NewListBox(NN<GUIClientControl> parent, Bool multiSelect)
{
	NN<UI::Java::JavaListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaListBox(*this, parent, multiSelect));
	return ctrl;
}

NN<UI::GUIPictureBox> UI::Java::JavaCore::NewPictureBox(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize)
{
	NN<UI::Java::JavaPictureBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaPictureBox(*this, parent, eng, hasBorder, allowResize));
	return ctrl;
}

NN<UI::GUIPictureBoxSimple> UI::Java::JavaCore::NewPictureBoxSimple(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder)
{
	NN<UI::Java::JavaPictureBoxSimple> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaPictureBoxSimple(*this, parent, eng, hasBorder));
	return ctrl;
}

NN<UI::GUIProgressBar> UI::Java::JavaCore::NewProgressBar(NN<GUIClientControl> parent, UInt64 totalCnt)
{
	NN<UI::Java::JavaProgressBar> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaProgressBar(*this, parent, totalCnt));
	return ctrl;
}

NN<UI::GUIRadioButton> UI::Java::JavaCore::NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected)
{
	NN<UI::Java::JavaRadioButton> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaRadioButton(*this, parent, initText, selected));
	return ctrl;
}

NN<UI::GUIRealtimeLineChart> UI::Java::JavaCore::NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS)
{
	NN<UI::Java::JavaRealtimeLineChart> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaRealtimeLineChart(*this, parent, eng, lineCnt, sampleCnt, updateIntervalMS));
	return ctrl;
}

NN<UI::GUITabControl> UI::Java::JavaCore::NewTabControl(NN<GUIClientControl> parent)
{
	NN<UI::Java::JavaTabControl> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaTabControl(*this, parent));
	return ctrl;
}

NN<UI::GUITextBox> UI::Java::JavaCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText)
{
	NN<UI::Java::JavaTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaTextBox(*this, parent, initText, false));
	return ctrl;
}

NN<UI::GUITextBox> UI::Java::JavaCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine)
{
	NN<UI::Java::JavaTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaTextBox(*this, parent, initText, multiLine));
	return ctrl;
}

NN<UI::GUITrackBar> UI::Java::JavaCore::NewTrackBar(NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal)
{
	NN<UI::Java::JavaTrackBar> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaTrackBar(*this, parent, minVal, maxVal, currVal));
	return ctrl;
}

NN<UI::GUIHSplitter> UI::Java::JavaCore::NewHSplitter(NN<GUIClientControl> parent, Int32 width, Bool isRight)
{
	NN<UI::Java::JavaHSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaHSplitter(*this, parent, width, isRight));
	return ctrl;
}

NN<UI::GUIVSplitter> UI::Java::JavaCore::NewVSplitter(NN<GUIClientControl> parent, Int32 height, Bool isBottom)
{
	NN<UI::Java::JavaVSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaVSplitter(*this, parent, height, isBottom));
	return ctrl;
}

NN<UI::GUIFileDialog> UI::Java::JavaCore::NewFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave)
{
	NN<UI::Java::JavaFileDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaFileDialog(compName, appName, dialogName, isSave));
	return ctrl;
}

NN<UI::GUIFolderDialog> UI::Java::JavaCore::NewFolderDialog()
{
	NN<UI::Java::JavaFolderDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaFolderDialog());
	return ctrl;
}

NN<UI::GUIFontDialog> UI::Java::JavaCore::NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::Java::JavaFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIFontDialog> UI::Java::JavaCore::NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::Java::JavaFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::Java::JavaCore::NewPanelBase(NN<UI::GUIPanel> master, ControlHandle *parentHWnd)
{
	NN<UI::Java::JavaPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaPanelBase(master, *this, parentHWnd));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::Java::JavaCore::NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent)
{
	NN<UI::Java::JavaPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaPanelBase(master, *this, parent));
	return ctrl;
}
