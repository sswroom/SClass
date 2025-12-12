#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
//#include "Media/JavaDrawEngine.h"
#include "Media/DrawEngineFactory.h"
#include "UI/GUICore.h"
#include "UI/JavaUI/JUIButton.h"
#include "UI/JavaUI/JUICheckBox.h"
#include "UI/JavaUI/JUICheckedListBox.h"
#include "UI/JavaUI/JUIComboBox.h"
#include "UI/JavaUI/JUICore.h"
#include "UI/JavaUI/JUIDateTimePicker.h"
#include "UI/JavaUI/JUIFileDialog.h"
#include "UI/JavaUI/JUIFolderDialog.h"
#include "UI/JavaUI/JUIFontDialog.h"
#include "UI/JavaUI/JUIGroupBox.h"
#include "UI/JavaUI/JUIHScrollBar.h"
#include "UI/JavaUI/JUIHSplitter.h"
#include "UI/JavaUI/JUILabel.h"
#include "UI/JavaUI/JUIListBox.h"
#include "UI/JavaUI/JUIMessageDialog.h"
#include "UI/JavaUI/JUIPanelBase.h"
#include "UI/JavaUI/JUIPictureBox.h"
#include "UI/JavaUI/JUIPictureBoxSimple.h"
#include "UI/JavaUI/JUIProgressBar.h"
#include "UI/JavaUI/JUIRadioButton.h"
#include "UI/JavaUI/JUIRealtimeLineChart.h"
#include "UI/JavaUI/JUITabControl.h"
#include "UI/JavaUI/JUITextBox.h"
#include "UI/JavaUI/JUITrackBar.h"
#include "UI/JavaUI/JUIVSplitter.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern JNIEnv *jniEnv;
}

UI::JavaUI::JUICore::JUICore()
{
	this->monMgr = 0;
	this->toExit = false;
}

UI::JavaUI::JUICore::~JUICore()
{
}

void UI::JavaUI::JUICore::Run()
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

void UI::JavaUI::JUICore::ProcessMessages()
{
}

void UI::JavaUI::JUICore::WaitForMessages()
{
}

void UI::JavaUI::JUICore::Exit()
{
	this->toExit = true;
	this->waitEvt.Set();
}

NN<Media::DrawEngine> UI::JavaUI::JUICore::CreateDrawEngine()
{
//	NN<Media::DrawEngine> deng = 0;
//	NEW_CLASS(deng, Media::GTKDrawEngine());
	return Media::DrawEngineFactory::CreateDrawEngine();
}

Double UI::JavaUI::JUICore::GetMagnifyRatio(Optional<MonitorHandle> hMonitor)
{
/*	Double v = gdk_screen_get_resolution(gdk_screen_get_default());
	if (v <= 0)
		v = 1.0;
	return v;*/
	return 1.0;
}

void UI::JavaUI::JUICore::UseDevice(Bool useSystem, Bool useDisplay)
{
}

void UI::JavaUI::JUICore::SetNoDisplayOff(Bool noDispOff)
{
}

void UI::JavaUI::JUICore::DisplayOff()
{
}

void UI::JavaUI::JUICore::Suspend()
{
}

Math::Size2D<UOSInt> UI::JavaUI::JUICore::GetDesktopSize()
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

Math::Coord2D<OSInt> UI::JavaUI::JUICore::GetCursorPos()
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

void UI::JavaUI::JUICore::SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot)
{
}

void UI::JavaUI::JUICore::GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi)
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

void UI::JavaUI::JUICore::SetMonitorMgr(Optional<Media::MonitorMgr> monMgr)
{
	this->monMgr = monMgr;
}

Optional<Media::MonitorMgr> UI::JavaUI::JUICore::GetMonitorMgr()
{
	return this->monMgr;
}

Bool UI::JavaUI::JUICore::IsForwarded()
{
	return getenv("SSH_CLIENT") != 0;
}

Int32 UI::JavaUI::JUICore::GetScrollBarSize()
{
	return 16;
}

void UI::JavaUI::JUICore::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::JavaUI::JUIMessageDialog::ShowOK(message, title, ctrl);
}

Bool UI::JavaUI::JUICore::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::JavaUI::JUIMessageDialog::ShowYesNo(message, title, ctrl);
}

NN<UI::GUIButton> UI::JavaUI::JUICore::NewButton(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::JavaUI::JUIButton> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIButton(*this, parent, text));
	return ctrl;
}

NN<UI::GUICheckBox> UI::JavaUI::JUICore::NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked)
{
	NN<UI::JavaUI::JUICheckBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUICheckBox(*this, parent, text, checked));
	return ctrl;
}

NN<UI::GUICheckedListBox> UI::JavaUI::JUICore::NewCheckedListBox(NN<GUIClientControl> parent)
{
	NN<UI::JavaUI::JUICheckedListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUICheckedListBox(*this, parent));
	return ctrl;
}

NN<UI::GUIComboBox> UI::JavaUI::JUICore::NewComboBox(NN<GUIClientControl> parent, Bool allowEdit)
{
	NN<UI::JavaUI::JUIComboBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIComboBox(*this, parent, allowEdit));
	return ctrl;
}

NN<UI::GUIDateTimePicker> UI::JavaUI::JUICore::NewDateTimePicker(NN<GUIClientControl> parent, Bool calendarSelect)
{
	NN<UI::JavaUI::JUIDateTimePicker> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIDateTimePicker(*this, parent));
	return ctrl;
}

NN<UI::GUIGroupBox> UI::JavaUI::JUICore::NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::JavaUI::JUIGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIGroupBox(*this, parent, text));
	return ctrl;
}

NN<UI::GUIHScrollBar> UI::JavaUI::JUICore::NewHScrollBar(NN<GUIClientControl> parent, Double width)
{
	NN<UI::JavaUI::JUIHScrollBar> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIHScrollBar(*this, parent, width));
	return ctrl;
}

NN<UI::GUILabel> UI::JavaUI::JUICore::NewLabel(NN<GUIClientControl> parent, Text::CStringNN text)
{
	NN<UI::JavaUI::JUILabel> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUILabel(*this, parent, text));
	return ctrl;
}

NN<UI::GUIListBox> UI::JavaUI::JUICore::NewListBox(NN<GUIClientControl> parent, Bool multiSelect)
{
	NN<UI::JavaUI::JUIListBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIListBox(*this, parent, multiSelect));
	return ctrl;
}

NN<UI::GUIPictureBox> UI::JavaUI::JUICore::NewPictureBox(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize)
{
	NN<UI::JavaUI::JUIPictureBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIPictureBox(*this, parent, eng, hasBorder, allowResize));
	return ctrl;
}

NN<UI::GUIPictureBoxSimple> UI::JavaUI::JUICore::NewPictureBoxSimple(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder)
{
	NN<UI::JavaUI::JUIPictureBoxSimple> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIPictureBoxSimple(*this, parent, eng, hasBorder));
	return ctrl;
}

NN<UI::GUIProgressBar> UI::JavaUI::JUICore::NewProgressBar(NN<GUIClientControl> parent, UInt64 totalCnt)
{
	NN<UI::JavaUI::JUIProgressBar> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIProgressBar(*this, parent, totalCnt));
	return ctrl;
}

NN<UI::GUIRadioButton> UI::JavaUI::JUICore::NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected)
{
	NN<UI::JavaUI::JUIRadioButton> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIRadioButton(*this, parent, initText, selected));
	return ctrl;
}

NN<UI::GUIRealtimeLineChart> UI::JavaUI::JUICore::NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess)
{
	NN<UI::JavaUI::JUIRealtimeLineChart> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIRealtimeLineChart(*this, parent, eng, lineCnt, sampleCnt, updateIntervalMS));
	return ctrl;
}

NN<UI::GUITabControl> UI::JavaUI::JUICore::NewTabControl(NN<GUIClientControl> parent)
{
	NN<UI::JavaUI::JUITabControl> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUITabControl(*this, parent));
	return ctrl;
}

NN<UI::GUITextBox> UI::JavaUI::JUICore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText)
{
	NN<UI::JavaUI::JUITextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUITextBox(*this, parent, initText, false));
	return ctrl;
}

NN<UI::GUITextBox> UI::JavaUI::JUICore::NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine)
{
	NN<UI::JavaUI::JUITextBox> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUITextBox(*this, parent, initText, multiLine));
	return ctrl;
}

NN<UI::GUITrackBar> UI::JavaUI::JUICore::NewTrackBar(NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal)
{
	NN<UI::JavaUI::JUITrackBar> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUITrackBar(*this, parent, minVal, maxVal, currVal));
	return ctrl;
}

NN<UI::GUIHSplitter> UI::JavaUI::JUICore::NewHSplitter(NN<GUIClientControl> parent, Int32 width, Bool isRight)
{
	NN<UI::JavaUI::JUIHSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIHSplitter(*this, parent, width, isRight));
	return ctrl;
}

NN<UI::GUIVSplitter> UI::JavaUI::JUICore::NewVSplitter(NN<GUIClientControl> parent, Int32 height, Bool isBottom)
{
	NN<UI::JavaUI::JUIVSplitter> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIVSplitter(*this, parent, height, isBottom));
	return ctrl;
}

NN<UI::GUIFileDialog> UI::JavaUI::JUICore::NewFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave)
{
	NN<UI::JavaUI::JUIFileDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIFileDialog(compName, appName, dialogName, isSave));
	return ctrl;
}

NN<UI::GUIFolderDialog> UI::JavaUI::JUICore::NewFolderDialog()
{
	NN<UI::JavaUI::JUIFolderDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIFolderDialog());
	return ctrl;
}

NN<UI::GUIFontDialog> UI::JavaUI::JUICore::NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::JavaUI::JUIFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIFontDialog> UI::JavaUI::JUICore::NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	NN<UI::JavaUI::JUIFontDialog> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIFontDialog(fontName, fontSizePt, isBold, isItalic));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::JavaUI::JUICore::NewPanelBase(NN<UI::GUIPanel> master, Optional<ControlHandle> parentHWnd)
{
	NN<UI::JavaUI::JUIPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIPanelBase(master, *this, parentHWnd));
	return ctrl;
}

NN<UI::GUIPanelBase> UI::JavaUI::JUICore::NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent)
{
	NN<UI::JavaUI::JUIPanelBase> ctrl;
	NEW_CLASSNN(ctrl, UI::JavaUI::JUIPanelBase(master, *this, parent));
	return ctrl;
}
