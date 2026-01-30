#ifndef _SM_JAVA_JAVAMYMOUSELISTENER
#define _SM_JAVA_JAVAMYMOUSELISTENER
#include "Java/JavaMouseListener.h"
#include "Java/JavaObject.h"
#include "UI/GUIControl.h"
#include "UI/GUICore.h"

namespace Java
{
	class JavaMyMouseListener : public JavaObject, public JavaMouseListener
	{
	public:
		JavaMyMouseListener(AnyType userObj);
		virtual ~JavaMyMouseListener();

		void HandleMouseClicked(UI::GUIControl::MouseEventHandler hdlr);
		void HandleMouseEntered(UI::GUIControl::MouseEventHandler hdlr);
		void HandleMouseExited(UI::GUIControl::MouseEventHandler hdlr);
		void HandleMousePressed(UI::GUIControl::MouseEventHandler hdlr);
		void HandleMouseReleased(UI::GUIControl::MouseEventHandler hdlr);
		NN<const JavaObject> ToObject() const { return NNTHIS; }

		static jclass GetClass();
		static jobject NewObject(AnyType userObj);
	};
}
#endif
