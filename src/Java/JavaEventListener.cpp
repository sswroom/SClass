#include "Stdafx.h"
#include "Java/JavaEventListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jclass Java::JavaEventListener::GetClass()
{
	return jniEnv->FindClass("java/util/EventListener");
}
