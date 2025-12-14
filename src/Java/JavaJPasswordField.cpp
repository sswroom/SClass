#include "Stdafx.h"
#include "Java/JavaJPasswordField.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaJPasswordField::setEchoChar = 0;

Java::JavaJPasswordField::JavaJPasswordField(Text::CStringNN text) : JavaJTextField(NewObject(text))
{
	this->SetEchoChar(0);
}

Java::JavaJPasswordField::~JavaJPasswordField()
{
}

void Java::JavaJPasswordField::SetEchoChar(UTF16Char c)
{
	if (setEchoChar == 0)
		setEchoChar = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setEchoChar", "(C)V");
	jniEnv->CallVoidMethod(this->me, setEchoChar, c);
}

jclass Java::JavaJPasswordField::GetClass()
{
	return jniEnv->FindClass("javax/swing/JPasswordField");
}

jobject Java::JavaJPasswordField::NewObject(Text::CStringNN text)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
	jstring s;
	jobject o = jniEnv->NewObject(cls, mid, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
	return o;
}
