#include "Stdafx.h"
#include "Java/JavaMouseListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jclass Java::JavaMouseListener::GetClass()
{
	return jniEnv->FindClass("java/awt/event/MouseListener");
}
