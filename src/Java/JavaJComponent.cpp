#include "Stdafx.h"
#include "Java/JavaJComponent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaJComponent::setForeground = 0;

Java::JavaJComponent::JavaJComponent(jobject me) : JavaContainer(me)
{
}

Java::JavaJComponent::~JavaJComponent()
{
}

void Java::JavaJComponent::SetForeground(NN<JavaColor> fg)
{
	if (setForeground == 0)
		setForeground = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setForeground", "(Ljava/awt/Color;)V");
	jniEnv->CallVoidMethod(this->me, setForeground, fg->GetJObject());
}

jclass Java::JavaJComponent::GetClass()
{
	return jniEnv->FindClass("javax/swing/JComponent");
}
