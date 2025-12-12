#include "Stdafx.h"
#include "Java/JavaWindowListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaWindowListener::JavaWindowListener(jobject me) : JavaEventListener(me)
{
}

Java::JavaWindowListener::~JavaWindowListener()
{
}

jclass Java::JavaWindowListener::GetClass()
{
	return jniEnv->FindClass("java/awt/event/WindowListener");
}
