#ifndef _SM_JAVA_JAVACOMPONENT
#define _SM_JAVA_JAVACOMPONENT
#include "Java/JavaObject.h"

namespace Java
{
	class JavaComponent : public JavaObject
	{
	public:
		enum class HorizontalAlignment
		{
			Left,
			Center,
			Right
		};

		enum class VerticalAlignment
		{
			Top,
			Center,
			Bottom
		};
	private:
		static jmethodID getHeight;
		static jmethodID getWidth;
		static jmethodID isVisible;
		static jmethodID requestFocus;
		static jmethodID setLocation;
		static jmethodID setSize;
		static jmethodID setVisible;
	public:
		JavaComponent(jobject me);
		virtual ~JavaComponent();

		Int32 GetHeight();
		Int32 GetWidth();
		Bool IsVisible();
		void RequestFocus();
		void SetLocation(Int32 x, Int32 y);
		void SetSize(Int32 width, Int32 height);
		void SetVisible(Bool b);

		static jclass GetClass();
		static Single HorizontalAlignmentGetInt(HorizontalAlignment halign);
		static Single VerticalAlignmentGetInt(VerticalAlignment valign);
	};
}
#endif
