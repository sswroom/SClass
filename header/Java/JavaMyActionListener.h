#ifndef _SM_JAVA_JAVAMYACTIONLISTENER
#define _SM_JAVA_JAVAMYACTIONLISTENER
#include "Java/JavaActionListener.h"
#include "Java/JavaObject.h"
#include "UI/GUICore.h"

namespace Java
{
	class JavaMyActionListener : public JavaObject, public JavaActionListener
	{
	public:
		JavaMyActionListener(UI::UIEvent hdlr, AnyType userObj);
		virtual ~JavaMyActionListener();

		NN<const JavaObject> ToObject() const { return *this; }

		static jclass GetClass();
		static jobject NewObject(UI::UIEvent hdlr, AnyType userObj);
	};
}
#endif
