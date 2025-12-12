#include "Stdafx.h"
#include "Java/JavaObject.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaObject::JavaObject(const JavaObject& o)
{
	this->me = o.me;
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

Bool Java::JavaObject::IsNull() const
{
	return this->me == 0;
}

jclass Java::JavaObject::GetClass()
{
	return jniEnv->FindClass("java/lang/Object");
}
