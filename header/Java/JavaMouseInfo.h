#ifndef _SM_JAVA_JAVAMOUSEINFO
#define _SM_JAVA_JAVAMOUSEINFO
#include "Java/JavaPointerInfo.h"

namespace Java
{
	class JavaMouseInfo
	{
	public:
		static Int32 GetNumberOfButtons();
		static JavaPointerInfo GetPointerInfo();
		static jclass GetClass();
	};
}
#endif
