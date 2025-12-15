#include "Stdafx.h"
#include "Java/JavaInputEvent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaInputEvent::JavaInputEvent(jobject me) : JavaComponentEvent(me)
{
}

Java::JavaInputEvent::~JavaInputEvent()
{
}

jclass Java::JavaInputEvent::GetClass()
{
	return jniEnv->FindClass("java/awt/event/InputEvent");
}
