#include "Stdafx.h"
#include "Java/JavaJLabel.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaJLabel::setText = 0;

Java::JavaJLabel::JavaJLabel() : JavaJComponent(NewObject())
{
}

Java::JavaJLabel::JavaJLabel(Text::CStringNN text) : JavaJComponent(NewObject(text))
{
}

Java::JavaJLabel::~JavaJLabel()
{

}

void Java::JavaJLabel::SetText(Text::CStringNN text)
{
	if (setText == 0)
		setText = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setText", "(Ljava/lang/String;)V");
	jniEnv->CallVoidMethod(this->me, setText, jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
}

jclass Java::JavaJLabel::GetClass()
{
	return jniEnv->FindClass("javax/swing/JLabel");
}

jobject Java::JavaJLabel::NewObject()
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "()V");
	return jniEnv->NewObject(cls, mid);
}

jobject Java::JavaJLabel::NewObject(Text::CStringNN text)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
	return jniEnv->NewObject(cls, mid, jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
}
