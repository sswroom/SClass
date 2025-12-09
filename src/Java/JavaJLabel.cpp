#include "Stdafx.h"
#include "Java/JavaJLabel.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jclass Java::JavaJLabel::GetClass()
{
	return jniEnv->FindClass("javax/swing/JLabel");
}
