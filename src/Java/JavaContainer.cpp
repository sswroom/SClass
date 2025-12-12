#include "Stdafx.h"
#include "Java/JavaContainer.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaContainer::add = 0;

Java::JavaContainer::JavaContainer(jobject me) : JavaComponent(me)
{
}

Java::JavaContainer::~JavaContainer()
{
}

Java::JavaComponent Java::JavaContainer::Add(NN<JavaComponent> comp)
{
	if (add == 0)
		add = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "add", "(Ljava/awt/Component;)Ljava/awt/Component;");
	return JavaComponent(jniEnv->CallObjectMethod(this->me, add, comp->GetJObject()));
}

jclass Java::JavaContainer::GetClass()
{
	return jniEnv->FindClass("java/awt/Container");
}
