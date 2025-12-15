#include "Stdafx.h"
#include "Java/JavaAWTEvent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaAWTEvent::JavaAWTEvent(jobject me) : JavaEventObject(me)
{
}

Java::JavaAWTEvent::~JavaAWTEvent()
{
}

jclass Java::JavaAWTEvent::GetClass()
{
	return jniEnv->FindClass("java/awt/AWTEvent");
}
