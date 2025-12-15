#include "Stdafx.h"
#include "Java/JavaEventObject.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaEventObject::JavaEventObject(jobject me) : JavaObject(me)
{
}

Java::JavaEventObject::~JavaEventObject()
{
}

jclass Java::JavaEventObject::GetClass()
{
	return jniEnv->FindClass("java/util/EventObject");
}
