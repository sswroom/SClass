#include "Stdafx.h"
#include "Java/JavaEventListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaEventListener::JavaEventListener(jobject me) : JavaObject(me)
{
}

Java::JavaEventListener::~JavaEventListener()
{
}

jclass Java::JavaEventListener::GetClass()
{
	return jniEnv->FindClass("java/util/EventListener");
}
