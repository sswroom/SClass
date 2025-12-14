#include "Stdafx.h"
#include "Java/JavaContainer.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaContainer::add = 0;
jmethodID Java::JavaContainer::setLayout = 0;

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

void Java::JavaContainer::SetLayout(Optional<JavaLayoutManager> mgr)
{
	if (setLayout == 0)
		setLayout = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setLayout", "(Ljava/awt/LayoutManager;)V");
	NN<JavaLayoutManager> nnmgr;
	jobject o;
	if (mgr.SetTo(nnmgr))
		o = nnmgr->GetJObject();
	else
		o = 0;
	jniEnv->CallVoidMethod(this->me, setLayout, o);
}

jclass Java::JavaContainer::GetClass()
{
	return jniEnv->FindClass("java/awt/Container");
}
