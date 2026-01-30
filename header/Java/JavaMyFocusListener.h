#ifndef _SM_JAVA_JAVAMYFOCUSLISTENER
#define _SM_JAVA_JAVAMYFOCUSLISTENER
#include "Java/JavaFocusListener.h"
#include "Java/JavaObject.h"
#include "UI/GUICore.h"

namespace Java
{
	class JavaMyFocusListener : public JavaObject, public JavaFocusListener
	{
	public:
		JavaMyFocusListener(AnyType userObj);
		virtual ~JavaMyFocusListener();

		void HandleFocusGain(UI::UIEvent hdlr);
		void HandleFocusLost(UI::UIEvent hdlr);
		NN<const JavaObject> ToObject() const { return NNTHIS; }

		static jclass GetClass();
		static jobject NewObject(AnyType userObj);
	};
}
#endif
