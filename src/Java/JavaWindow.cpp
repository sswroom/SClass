#include "Stdafx.h"
#include "Java/JavaWindow.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaWindow::addWindowListener = 0;
jmethodID Java::JavaWindow::setAlwaysOnTop = 0;
jmethodID Java::JavaWindow::setBounds = 0;
jmethodID Java::JavaWindow::setVisible = 0;

Java::JavaWindow::JavaWindow(jobject me) : JavaContainer(me)
{
}

Java::JavaWindow::~JavaWindow()
{
}

void Java::JavaWindow::AddWindowListener(NN<JavaWindowListener> listener)
{
	if (addWindowListener == 0)
		addWindowListener = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "addWindowListener", "(Ljava/awt/event/WindowListener;)V");
	jniEnv->CallVoidMethod(this->me, addWindowListener, listener->GetJObject());
}

void Java::JavaWindow::SetAlwaysOnTop(Bool alwaysOnTop)
{
	if (setAlwaysOnTop == 0)
		setAlwaysOnTop = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setAlwaysOnTop", "(Z)V");
	jniEnv->CallVoidMethod(this->me, setAlwaysOnTop, alwaysOnTop);
}

void Java::JavaWindow::SetBounds(Int32 x, Int32 y, Int32 width, Int32 height)
{
	if (setBounds == 0)
		setBounds = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setBounds", "(IIII)V");
	jniEnv->CallVoidMethod(this->me, setBounds, x, y, width, height);
}

void Java::JavaWindow::SetVisible(Bool b)
{
	if (setVisible == 0)
		setVisible = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setVisible", "(Z)V");
	jniEnv->CallVoidMethod(this->me, setVisible, b);
}

jclass Java::JavaWindow::GetClass()
{
	return jniEnv->FindClass("java/awt/Window");
}
