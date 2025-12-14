#include "Stdafx.h"
#include "Java/JavaJTextField.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaJTextField::JavaJTextField(Text::CStringNN text) : JavaJTextComponent(NewObject(text))
{
}

Java::JavaJTextField::JavaJTextField(jobject me) : JavaJTextComponent(me)
{
}

Java::JavaJTextField::~JavaJTextField()
{
}

jclass Java::JavaJTextField::GetClass()
{
	return jniEnv->FindClass("javax/swing/JTextField");
}

jobject Java::JavaJTextField::NewObject(Text::CStringNN text)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
	jstring s;
	jobject o = jniEnv->NewObject(cls, mid, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
	return o;
}
