#include "Stdafx.h"
#include "Java/JavaMyActionListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

JNIEXPORT void JNICALL Java_MyActionListener_actionPerformed(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::UIEvent hdlr = (UI::UIEvent)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	hdlr(userObj);
}

Java::JavaMyActionListener::JavaMyActionListener(UI::UIEvent hdlr, AnyType userObj) : JavaObject(NewObject(hdlr, userObj))
{
}

Java::JavaMyActionListener::~JavaMyActionListener()
{
}

jclass Java::JavaMyActionListener::GetClass()
{
	return jniEnv->FindClass("MyActionListener");
}

jobject Java::JavaMyActionListener::NewObject(UI::UIEvent hdlr, AnyType userObj)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(JJ)V");
	return jniEnv->NewObject(cls, mid, (Int64)hdlr, (Int64)userObj.GetOSInt());
}
