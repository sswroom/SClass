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
	jstring s;
	jniEnv->CallVoidMethod(this->me, setText, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
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
	jstring s;
	jobject o = jniEnv->NewObject(cls, mid, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
	return o;
}
