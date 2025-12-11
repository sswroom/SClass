#include "Stdafx.h"
#include "Java/JavaJLabel.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaJLabel::JavaJLabel()
{
	this->cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(this->cls, "<init>", "()V");
	this->me = jniEnv->NewObject(this->cls, mid);
}

Java::JavaJLabel::JavaJLabel(Text::CStringNN text)
{
	this->cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(this->cls, "<init>", "(Ljava/lang/String;)V");
	this->me = jniEnv->NewObject(this->cls, mid, jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
}

Java::JavaJLabel::~JavaJLabel()
{

}

jclass Java::JavaJLabel::GetClass()
{
	return jniEnv->FindClass("javax/swing/JLabel");
}
