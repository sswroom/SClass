#ifndef _SM_JAVA_JAVAWINDOW
#define _SM_JAVA_JAVAWINDOW
#include "Java/JavaContainer.h"
#include "Java/JavaWindowListener.h"

namespace Java
{
	class JavaWindow : public JavaContainer
	{
	private:
		static jmethodID addWindowListener;
		static jmethodID removeWindowListener;
		static jmethodID setAlwaysOnTop;
		static jmethodID setBounds;
		static jmethodID setVisible;
	public:
		JavaWindow(jobject me);
		virtual ~JavaWindow();

		void AddWindowListener(NN<JavaWindowListener> l);
		void RemoveWindowListener(NN<JavaWindowListener> l);
		void SetAlwaysOnTop(Bool alwaysOnTop);
		void SetBounds(Int32 x, Int32 y, Int32 width, Int32 height);
		void SetVisible(Bool b);

		static jclass GetClass();
	};
}
#endif
