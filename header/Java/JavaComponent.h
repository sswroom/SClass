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
		static jmethodID requestFocus;
	public:
		JavaComponent(jobject me);
		virtual ~JavaComponent();

		Int32 GetHeight();
		Int32 GetWidth();
		void RequestFocus();

		static jclass GetClass();
		static Single HorizontalAlignmentGetInt(HorizontalAlignment halign);
		static Single VerticalAlignmentGetInt(VerticalAlignment valign);
	};
}
#endif
