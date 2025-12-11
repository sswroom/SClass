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
	public:
		static jclass GetClass();
		static Single HorizontalAlignmentGetInt(HorizontalAlignment halign);
		static Single VerticalAlignmentGetInt(VerticalAlignment valign);
	};
}
#endif
