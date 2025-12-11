#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
//#include "Media/JavaDrawEngine.h"
#include "Media/DrawEngineFactory.h"
#include "UI/GUICore.h"
#include "UI/JavaUI/JavaButton.h"
#include "UI/JavaUI/JavaCheckBox.h"
#include "UI/JavaUI/JavaCheckedListBox.h"
#include "UI/JavaUI/JavaComboBox.h"
#include "UI/JavaUI/JavaCore.h"
#include "UI/JavaUI/JavaDateTimePicker.h"
#include "UI/JavaUI/JavaFileDialog.h"
#include "UI/JavaUI/JavaFolderDialog.h"
#include "UI/JavaUI/JavaFontDialog.h"
#include "UI/JavaUI/JavaGroupBox.h"
#include "UI/JavaUI/JavaHScrollBar.h"
#include "UI/JavaUI/JavaHSplitter.h"
#include "UI/JavaUI/JavaLabel.h"
#include "UI/JavaUI/JavaListBox.h"
#include "UI/JavaUI/JavaMessageDialog.h"
#include "UI/JavaUI/JavaPanelBase.h"
#include "UI/JavaUI/JavaPictureBox.h"
#include "UI/JavaUI/JavaPictureBoxSimple.h"
#include "UI/JavaUI/JavaProgressBar.h"
#include "UI/JavaUI/JavaRadioButton.h"
#include "UI/JavaUI/JavaRealtimeLineChart.h"
#include "UI/JavaUI/JavaTabControl.h"
#include "UI/JavaUI/JavaTextBox.h"
#include "UI/JavaUI/JavaTrackBar.h"
#include "UI/JavaUI/JavaVSplitter.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern JNIEnv *jniEnv;
}

UI::JavaUI::JavaCore::JavaCore()
{
	this->monMgr = 0;
	this->toExit = false;
}

UI::JavaUI::JavaCore::~JavaCore()
{
}

void UI::JavaUI::JavaCore::Run()
{
	JNIEnv *env = jniEnv;
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

void UI::JavaUI::JavaCore::ProcessMessages()
{
}

void UI::JavaUI::JavaCore::WaitForMessages()
{
}

void UI::JavaUI::JavaCore::Exit()
{
	this->toExit = true;
	this->waitEvt.Set();
}

NN<Media::DrawEngine> UI::JavaUI::JavaCore::CreateDrawEngine()
{
//	NN<Media::DrawEngine> deng = 0;
//	NEW_CLASS(deng, Media::GTKDrawEngine());
	return Media::DrawEngineFactory::CreateDrawEngine();
}

Double UI::JavaUI::JavaCore::GetMagnifyRatio(Optional<MonitorHandle> hMonitor)
{
/*	Double v = gdk_screen_get_resolution(gdk_screen_get_default());
	if (v <= 0)
		v = 1.0;
	return v;*/
	return 1.0;
}

void UI::JavaUI::JavaCore::UseDevice(Bool useSystem, Bool useDisplay)
{
}

void UI::JavaUI::JavaCore::SetNoDisplayOff(Bool noDispOff)
{
}

void UI::JavaUI::JavaCore::DisplayOff()
{
}

void UI::JavaUI::JavaCore::Suspend()
{
}

Math::Size2D<UOSInt> UI::JavaUI::JavaCore::GetDesktopSize()
{
	JNIEnv *env = jniEnv;
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

Math::Coord2D<OSInt> UI::JavaUI::JavaCore::GetCursorPos()
{
	JNIEnv *env = jniEnv;
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

void UI::JavaUI::JavaCore::SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot)
{
}

void UI::JavaUI::JavaCore::GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi)
{
	NN<Media::MonitorMgr> monMgr;
	if (this->monMgr.SetTo(monMgr))
	{
		hdpi.Set(monMgr->GetMonitorHDPI(hMonitor));
		ddpi.Set(monMgr->GetMonitorDDPI(hMonitor));
	}
	else
	{
		hdpi.Set(96.0);
		ddpi.Set(96.0);
	}
}

void UI::JavaUI::JavaCore::SetMonitorMgr(Optional<Media::MonitorMgr> monMgr)
{
	this->monMgr = monMgr;
}

Optional<Media::MonitorMgr> UI::JavaUI::JavaCore::GetMonitorMgr()
{
	return this->monMgr;
}

Bool UI::JavaUI::JavaCore::IsForwarded()
{
	return getenv("SSH_CLIENT") != 0;
}

Int32 UI::JavaUI::JavaCore::GetScrollBarSize()
{
	return 16;
}

void UI::JavaUI::JavaCore::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::JavaUI::JavaMessageDialog::ShowOK(jniEnv, message, title, ctrl);
}

Bool UI::JavaUI::JavaCore::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::JavaUI::JavaMessageDialog::ShowYesNo(jniEnv, message, title, ctrl);
}

NN<UI::GUIButton> UI::JavaUI::JavaCore::NewButton(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::JavaUI::JavaButton> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaButton(*this, parent, text));
	return ctrl;
}

NN<UI::GUICheckBox> UI::JavaUI::JavaCore::NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked)
{
	NN<UI::JavaUI::JavaCheckBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaCheckBox(*this, parent, text, checked));
	return ctrl;
}

NN<UI::GUICheckedListBox> UI::JavaUI::JavaCore::NewCheckedListBox(NN<GUIClientControl> parent)
{
	NN<UI::JavaUI::JavaCheckedListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaCheckedListBox(*this, parent));
	return ctrl;
}

NN<UI::GUIComboBox> UI::JavaUI::JavaCore::NewComboBox(NN<GUIClientControl> parent, Bool allowEdit)
{
	NN<UI::JavaUI::JavaComboBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaComboBox(*this, parent, allowEdit));
	return ctrl;
}

NN<UI::GUIDateTimePicker> UI::JavaUI::JavaCore::NewDateTimePicker(NN<GUIClientControl> parent, Bool calendarSelect)
{
	NN<UI::JavaUI::JavaDateTimePicker> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaDateTimePicker(*this, parent));
	return ctrl;
}

NN<UI::GUIGroupBox> UI::JavaUI::JavaCore::NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::JavaUI::JavaGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaGroupBox(*this, parent, text));
	return ctrl;
}

NN<UI::GUIHScrollBar> UI::JavaUI::JavaCore::NewHScrollBar(NN<GUIClientControl> parent, Double width)
{
	NN<UI::JavaUI::JavaHScrollBar> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaHScrollBar(*this, parent, width));
	return ctrl;
}

NN<UI::GUILabel> UI::JavaUI::JavaCore::NewLabel(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::JavaUI::JavaLabel> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaLabel(*this, parent, text));
	return ctrl;
}

NN<UI::GUIListBox> UI::JavaUI::JavaCore::NewListBox(NN<GUIClientControl> parent, Bool multiSelect)
{
	NN<UI::JavaUI::JavaListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaListBox(*this, parent, multiSelect));
	return ctrl;
}

NN<UI::GUIPictureBox> UI::JavaUI::JavaCore::NewPictureBox(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize)
{
	NN<UI::JavaUI::JavaPictureBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaPictureBox(*this, parent, eng, hasBorder, allowResize));
	return ctrl;
}

NN<UI::GUIPictureBoxSimple> UI::JavaUI::JavaCore::NewPictureBoxSimple(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder)
{
	NN<UI::JavaUI::JavaPictureBoxSimple> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaPictureBoxSimple(*this, parent, eng, hasBorder));
	return ctrl;
}

NN<UI::GUIProgressBar> UI::JavaUI::JavaCore::NewProgressBar(NN<GUIClientControl> parent, UInt64 totalCnt)
{
	NN<UI::JavaUI::JavaProgressBar> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaProgressBar(*this, parent, totalCnt));
	return ctrl;
}

NN<UI::GUIRadioButton> UI::JavaUI::JavaCore::NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected)
{
	NN<UI::JavaUI::JavaRadioButton> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaRadioButton(*this, parent, initText, selected));
	return ctrl;
}

NN<UI::GUIRealtimeLineChart> UI::JavaUI::JavaCore::NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess)
{
	NN<UI::JavaUI::JavaRealtimeLineChart> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaRealtimeLineChart(*this, parent, eng, lineCnt, sampleCnt, updateIntervalMS));
	return ctrl;
}

NN<UI::GUITabControl> UI::JavaUI::JavaCore::NewTabControl(NN<GUIClientControl> parent)
{
	NN<UI::JavaUI::JavaTabControl> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaTabControl(*this, parent));
	return ctrl;
}

NN<UI::GUITextBox> UI::JavaUI::JavaCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText)
{
	NN<UI::JavaUI::JavaTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaTextBox(*this, parent, initText, false));
	return ctrl;
}

NN<UI::GUITextBox> UI::JavaUI::JavaCore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine)
{
	NN<UI::JavaUI::JavaTextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaTextBox(*this, parent, initText, multiLine));
	return ctrl;
}

NN<UI::GUITrackBar> UI::JavaUI::JavaCore::NewTrackBar(NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal)
{
	NN<UI::JavaUI::JavaTrackBar> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaTrackBar(*this, parent, minVal, maxVal, currVal));
	return ctrl;
}

NN<UI::GUIHSplitter> UI::JavaUI::JavaCore::NewHSplitter(NN<GUIClientControl> parent, Int32 width, Bool isRight)
{
	NN<UI::JavaUI::JavaHSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaHSplitter(*this, parent, width, isRight));
	return ctrl;
}

NN<UI::GUIVSplitter> UI::JavaUI::JavaCore::NewVSplitter(NN<GUIClientControl> parent, Int32 height, Bool isBottom)
{
	NN<UI::JavaUI::JavaVSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaVSplitter(*this, parent, height, isBottom));
	return ctrl;
}

NN<UI::GUIFileDialog> UI::JavaUI::JavaCore::NewFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave)
{
	NN<UI::JavaUI::JavaFileDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaFileDialog(compName, appName, dialogName, isSave));
	return ctrl;
}

NN<UI::GUIFolderDialog> UI::JavaUI::JavaCore::NewFolderDialog()
{
	NN<UI::JavaUI::JavaFolderDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaFolderDialog());
	return ctrl;
}

NN<UI::GUIFontDialog> UI::JavaUI::JavaCore::NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::JavaUI::JavaFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIFontDialog> UI::JavaUI::JavaCore::NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::JavaUI::JavaFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::JavaUI::JavaCore::NewPanelBase(NN<UI::GUIPanel> master, Optional<ControlHandle> parentHWnd)
{
	NN<UI::JavaUI::JavaPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaPanelBase(master, *this, parentHWnd));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::JavaUI::JavaCore::NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent)
{
	NN<UI::JavaUI::JavaPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JavaPanelBase(master, *this, parent));
	return ctrl;
}
