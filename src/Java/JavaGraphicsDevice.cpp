#include "Stdafx.h"
#include "Java/JavaGraphicsDevice.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaGraphicsDevice::getDisplayMode = 0;

Java::JavaGraphicsDevice::JavaGraphicsDevice(jobject me) : JavaObject(me)
{
}

Java::JavaGraphicsDevice::~JavaGraphicsDevice()
{
}

Java::JavaDisplayMode Java::JavaGraphicsDevice::GetDisplayMode()
{
	if (getDisplayMode == 0)
		getDisplayMode = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getDisplayMode", "()Ljava/awt/DisplayMode;");
	return JavaDisplayMode(jniEnv->CallObjectMethod(this->me, getDisplayMode));
}

jclass Java::JavaGraphicsDevice::GetClass()
{
	return jniEnv->FindClass("java/awt/GraphicsDevice");
}
