#include "Stdafx.h"
#include "Java/JavaAbstractButton.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaAbstractButton::addActionListener = 0;
jmethodID Java::JavaAbstractButton::setText = 0;

Java::JavaAbstractButton::JavaAbstractButton(jobject me) : JavaJComponent(me)
{
}

Java::JavaAbstractButton::~JavaAbstractButton()
{
}

void Java::JavaAbstractButton::AddActionListener(NN<JavaActionListener> l)
{
	if (addActionListener == 0)
		addActionListener = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "addActionListener", "(Ljava/awt/event/ActionListener;)V");
	jniEnv->CallVoidMethod(this->me, addActionListener, l->ToObject()->GetJObject());
}

void Java::JavaAbstractButton::SetText(Text::CStringNN text)
{
	if (setText == 0)
		setText = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setText", "(Ljava/lang/String;)V");
	jstring s;
	jniEnv->CallVoidMethod(this->me, setText, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
}

jclass Java::JavaAbstractButton::GetClass()
{
	return jniEnv->FindClass("javax/swing/AbstractButton");
}
