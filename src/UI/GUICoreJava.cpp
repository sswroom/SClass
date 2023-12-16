#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
//#include "Media/JavaDrawEngine.h"
#include "Media/DrawEngineFactory.h"
#include "UI/GUICore.h"
#include "UI/GUICoreJava.h"
#include "UI/Java/JavaGroupBox.h"
#include "UI/Java/JavaMessageDialog.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern void *jniEnv;
}

UI::GUICoreJava::GUICoreJava()
{
	this->monMgr = 0;
	this->toExit = false;
	NEW_CLASS(this->waitEvt, Sync::Event(true));
}

UI::GUICoreJava::~GUICoreJava()
{
	DEL_CLASS(this->waitEvt);
}

void UI::GUICoreJava::Run()
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
			this->waitEvt->Wait(1000);
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
			this->waitEvt->Wait(1000);
		}
		printf("Run End\r\n");
	}
}

void UI::GUICoreJava::ProcessMessages()
{
}

void UI::GUICoreJava::WaitForMessages()
{
}

void UI::GUICoreJava::Exit()
{
	this->toExit = true;
	this->waitEvt->Set();
}

NotNullPtr<Media::DrawEngine> UI::GUICoreJava::CreateDrawEngine()
{
//	NotNullPtr<Media::DrawEngine> deng = 0;
//	NEW_CLASS(deng, Media::GTKDrawEngine());
	return Media::DrawEngineFactory::CreateDrawEngine();
}

Double UI::GUICoreJava::GetMagnifyRatio(MonitorHandle *hMonitor)
{
/*	Double v = gdk_screen_get_resolution(gdk_screen_get_default());
	if (v <= 0)
		v = 1.0;
	return v;*/
	return 1.0;
}

void UI::GUICoreJava::UseDevice(Bool useSystem, Bool useDisplay)
{
}

void UI::GUICoreJava::SetNoDisplayOff(Bool noDispOff)
{
}

void UI::GUICoreJava::DisplayOff()
{
}

void UI::GUICoreJava::Suspend()
{
}

Math::Size2D<UOSInt> UI::GUICoreJava::GetDesktopSize()
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

Math::Coord2D<OSInt> UI::GUICoreJava::GetCursorPos()
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

void UI::GUICoreJava::SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot)
{
}

void UI::GUICoreJava::GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi)
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

void UI::GUICoreJava::SetMonitorMgr(Media::MonitorMgr *monMgr)
{
	this->monMgr = monMgr;
}

Media::MonitorMgr *UI::GUICoreJava::GetMonitorMgr()
{
	return this->monMgr;
}

Bool UI::GUICoreJava::IsForwarded()
{
	return getenv("SSH_CLIENT") != 0;
}

void UI::GUICoreJava::ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	UI::Java::JavaMessageDialog::ShowOK((JNIEnv*)jniEnv, message, title, ctrl);
}

Bool UI::GUICoreJava::ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return UI::Java::JavaMessageDialog::ShowYesNo((JNIEnv*)jniEnv, message, title, ctrl);
}

NotNullPtr<UI::GUIGroupBox> UI::GUICoreJava::NewGroupBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text)
{
	NotNullPtr<UI::Java::JavaGroupBox> ctrl;
	NEW_CLASSNN(ctrl, UI::Java::JavaGroupBox(*this, parent, text));
	return ctrl;
}
