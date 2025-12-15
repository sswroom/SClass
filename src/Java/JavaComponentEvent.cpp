#include "Stdafx.h"
#include "Java/JavaComponentEvent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaComponentEvent::JavaComponentEvent(jobject me) : JavaAWTEvent(me)
{
}

Java::JavaComponentEvent::~JavaComponentEvent()
{
}

jclass Java::JavaComponentEvent::GetClass()
{
	return jniEnv->FindClass("java/awt/event/ComponentEvent");
}
