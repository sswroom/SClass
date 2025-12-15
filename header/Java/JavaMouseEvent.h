#ifndef _SM_JAVA_JAVAMOUSEEVENT
#define _SM_JAVA_JAVAMOUSEEVENT
#include "Java/JavaInputEvent.h"
#include "UI/GUIControl.h"

namespace Java
{
	class JavaMouseEvent : public JavaInputEvent
	{
	public:
		enum class Button
		{
			NOBUTTON,
			BUTTON1,
			BUTTON2,
			BUTTON3
		};
	private:
		static jmethodID getButton;
		static jmethodID getX;
		static jmethodID getXOnScreen;
		static jmethodID getY;
		static jmethodID getYOnScreen;
	public:
		JavaMouseEvent(jobject me);
		virtual ~JavaMouseEvent();

		Button GetButton();
		Int32 GetX();
		Int32 GetXOnScreen();
		Int32 GetY();
		Int32 GetYOnScreen();
		
		static jclass GetClass();
		static UI::GUIControl::MouseButton Button2MouseButton(Button btn);
	};
}
#endif
