#include "Stdafx.h"
#include "Java/JavaActionListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jclass Java::JavaActionListener::GetClass()
{
	return jniEnv->FindClass("java/awt/event/ActionListener");
}
