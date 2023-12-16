#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/Java/JavaTimer.h"
#include <jni.h>

JNIEXPORT void JNICALL Java_TimerListener_actionPerformed(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMe", "()J");
	UI::GUITimer *me = (UI::GUITimer*)env->CallLongMethod(obj, mid);
	me->EventTick();
}

UI::Java::JavaTimer::JavaTimer(JNIEnv *env, UInt32 interval, UI::UIEvent handler, void *userObj) : UI::GUITimer(handler, userObj)
{
	this->interval = interval;
	this->env = env;
	jclass cls = env->FindClass("TimerListener");
	jmethodID mid = env->GetMethodID(cls, "<init>", "(J)V");
	jobject tmr = env->NewObject(cls, mid, (Int64)this);
	cls = env->FindClass("javax/swing/Timer");
	mid = env->GetMethodID(cls, "<init>", "(ILjava/awt/event/ActionListener;)V");
	this->tmr = env->NewObject(cls, mid, interval, tmr);
}

UI::Java::JavaTimer::~JavaTimer()
{
	jclass cls = env->GetObjectClass(this->tmr);
	jmethodID mid = env->GetMethodID(cls, "stop", "()V");
	env->CallVoidMethod(this->tmr, mid);
}
