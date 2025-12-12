#include "Stdafx.h"
#include "Java/JavaPointerInfo.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaPointerInfo::getDevice = 0;
jmethodID Java::JavaPointerInfo::getLocation = 0;

Java::JavaPointerInfo::JavaPointerInfo(jobject me) : JavaObject(me)
{
}

Java::JavaPointerInfo::~JavaPointerInfo()
{
}

Java::JavaGraphicsDevice Java::JavaPointerInfo::GetDevice()
{
	if (getDevice == 0)
		getDevice = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getDevice", "()Ljava/awt/GraphicsDevice;");
	return JavaGraphicsDevice(jniEnv->CallObjectMethod(this->me, getDevice));
}

jclass Java::JavaPointerInfo::GetClass()
{
	return jniEnv->FindClass("java/awt/PointerInfo");
}
