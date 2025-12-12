#include "Stdafx.h"
#include "Java/JavaMouseInfo.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Int32 Java::JavaMouseInfo::GetNumberOfButtons()
{
	jclass cls = GetClass();
	jmethodID meth = jniEnv->GetStaticMethodID(cls, " getNumberOfButtons", "()I");
	return jniEnv->CallStaticIntMethod(cls, meth);
}

Java::JavaPointerInfo Java::JavaMouseInfo::GetPointerInfo()
{
	jclass cls = GetClass();
	jmethodID meth = jniEnv->GetStaticMethodID(cls, "getPointerInfo", "()Ljava/awt/PointerInfo;");
	return JavaPointerInfo(jniEnv->CallStaticObjectMethod(cls, meth));
}

jclass Java::JavaMouseInfo::GetClass()
{
	return jniEnv->FindClass("java/awt/MouseInfo");
}
