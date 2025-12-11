#include "Stdafx.h"
#include "Java/JavaObject.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaObject::JavaObject(jobject me)
{
	this->me = me;
}

Java::JavaObject::~JavaObject()
{

}

jobject Java::JavaObject::GetJObject() const
{
	return this->me;
}

jclass Java::JavaObject::GetClass()
{
	return jniEnv->FindClass("java/lang/Object");
}
