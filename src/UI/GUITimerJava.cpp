#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUITimer.h"
#include <jni.h>

extern "C"
{
	extern void *jniEnv;
}

JNIEXPORT void JNICALL Java_TimerListener_actionPerformed(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMe", "()J");
	UI::GUITimer *me = (UI::GUITimer*)env->CallLongMethod(obj, mid);
	me->OnTick();
}

UI::GUITimer::GUITimer(UI::GUICore *ui, UI::GUIForm *parent, UOSInt id, UInt32 interval, UI::UIEvent handler, void *userObj)
{
	this->interval = interval;
	this->handler = handler;
	this->userObj = userObj;
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->FindClass("TimerListener");
	jmethodID mid = env->GetMethodID(cls, "<init>", "(J)V");
	jobject tmr = env->NewObject(cls, mid, (Int64)this);
	cls = env->FindClass("javax/swing/Timer");
	mid = env->GetMethodID(cls, "<init>", "(ILjava/awt/event/ActionListener;)V");
	this->id = (UOSInt)env->NewObject(cls, mid, interval, tmr);
}

UI::GUITimer::~GUITimer()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jobject tmr = (jobject)this->id;
	jclass cls = env->GetObjectClass(tmr);
	jmethodID mid = env->GetMethodID(cls, "stop", "()V");
	env->CallVoidMethod(tmr, mid);
}

void UI::GUITimer::OnTick()
{
	if (this->handler)
		this->handler(this->userObj);
}

UOSInt UI::GUITimer::GetId()
{
	return this->id;
}

