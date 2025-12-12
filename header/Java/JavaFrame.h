#ifndef _SM_JAVA_JAVAFRAME
#define _SM_JAVA_JAVAFRAME
#include "Java/JavaWindow.h"
#include "Text/CString.h"

namespace Java
{
	class JavaFrame : public JavaWindow
	{
	public:
		enum ExtendedState
		{
			NORMAL = 0,
			ICONIFIED = 1,
			MAXIMIZED_HORIZ = 2,
			MAXIMIZED_VERT = 4,
			MAXIMIZED_BOTH = 6
		};
	private:
		static jmethodID setExtendedState;
		static jmethodID setResizable;
		static jmethodID setTitle;
	public:
		JavaFrame(jobject me);
		virtual ~JavaFrame();

		void SetExtendedState(ExtendedState state);
		void SetResizable(Bool resizable);
		void SetTitle(Text::CStringNN title);

		static jclass GetClass();
	};
}
#endif
