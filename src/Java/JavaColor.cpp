#include "Stdafx.h"
#include "Java/JavaColor.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaColor::JavaColor(jobject me) : JavaObject(me)
{
}

Java::JavaColor::JavaColor(Int32 rgba, Bool hasalpha) : JavaObject(NewObject(rgba, hasalpha))
{
}

Java::JavaColor::~JavaColor()
{
}

jclass Java::JavaColor::GetClass()
{
	return jniEnv->FindClass("java/awt/Color");
}

jobject Java::JavaColor::NewObject(Int32 rgba, Bool hasalpha)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(IZ)V");
	return jniEnv->NewObject(cls, mid, rgba, hasalpha);
}
