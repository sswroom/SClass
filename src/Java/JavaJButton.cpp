#include "Stdafx.h"
#include "Java/JavaJButton.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaJButton::JavaJButton(Text::CStringNN text) : JavaAbstractButton(NewObject(text))
{
}

Java::JavaJButton::~JavaJButton()
{
}

jclass Java::JavaJButton::GetClass()
{
	return jniEnv->FindClass("javax/swing/JButton");
}

jobject Java::JavaJButton::NewObject(Text::CStringNN text)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
	jstring s;
	jobject o = jniEnv->NewObject(cls, mid, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
	return o;
}
