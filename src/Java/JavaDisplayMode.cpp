#include "Stdafx.h"
#include "Java/JavaDisplayMode.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaDisplayMode::getWidth = 0;
jmethodID Java::JavaDisplayMode::getHeight = 0;
jmethodID Java::JavaDisplayMode::getRefreshRate = 0;
jmethodID Java::JavaDisplayMode::getBitDepth = 0;

Java::JavaDisplayMode::JavaDisplayMode(jobject me) : JavaObject(me)
{
}

Java::JavaDisplayMode::~JavaDisplayMode()
{
}

Int32 Java::JavaDisplayMode::GetWidth()
{
	if (getWidth == 0)
		getWidth = jniEnv->GetMethodID(jniEnv->GetObjectClass(me), "getWidth", "()I");
	return jniEnv->CallIntMethod(me, getWidth);
}

Int32 Java::JavaDisplayMode::GetHeight()
{
	if (getHeight == 0)
		getHeight = jniEnv->GetMethodID(jniEnv->GetObjectClass(me), "getHeight", "()I");
	return jniEnv->CallIntMethod(me, getHeight);
}

Int32 Java::JavaDisplayMode::GetBitDepth()
{
	if (getBitDepth == 0)
		getBitDepth = jniEnv->GetMethodID(jniEnv->GetObjectClass(me), "getBitDepth", "()I");
	return jniEnv->CallIntMethod(me, getBitDepth);
}
Int32 Java::JavaDisplayMode::GetRefreshRate()
{
	if (getRefreshRate == 0)
		getRefreshRate = jniEnv->GetMethodID(jniEnv->GetObjectClass(me), "getRefreshRate", "()I");
	return jniEnv->CallIntMethod(me, getRefreshRate);
}

jclass Java::JavaDisplayMode::GetClass()
{
	return jniEnv->FindClass("java/awt/DisplayMode");
}
