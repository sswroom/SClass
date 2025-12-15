#include "Stdafx.h"
#include "Java/JavaMyFocusListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;

JNIEXPORT void JNICALL Java_MyFocusListener_focusGained(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getFocusGainedHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::UIEvent hdlr = (UI::UIEvent)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr) hdlr(userObj);
}

JNIEXPORT void JNICALL Java_MyFocusListener_focusLost(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getFocusLostHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::UIEvent hdlr = (UI::UIEvent)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr) hdlr(userObj);
}
}

Java::JavaMyFocusListener::JavaMyFocusListener(AnyType userObj) : JavaObject(NewObject(userObj))
{
}

Java::JavaMyFocusListener::~JavaMyFocusListener()
{
}

void Java::JavaMyFocusListener::HandleFocusGain(UI::UIEvent hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setFocusGainedHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

void Java::JavaMyFocusListener::HandleFocusLost(UI::UIEvent hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setFocusLostHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

jclass Java::JavaMyFocusListener::GetClass()
{
	return jniEnv->FindClass("MyFocusListener");
}

jobject Java::JavaMyFocusListener::NewObject(AnyType userObj)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(J)V");
	return jniEnv->NewObject(cls, mid, (Int64)userObj.GetOSInt());
}
