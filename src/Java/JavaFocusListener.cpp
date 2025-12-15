#include "Stdafx.h"
#include "Java/JavaFocusListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jclass Java::JavaFocusListener::GetClass()
{
	return jniEnv->FindClass("java/awt/event/FocusListener");
}
