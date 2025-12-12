#ifndef _SM_JAVA_JAVADISPLAYMODE
#define _SM_JAVA_JAVADISPLAYMODE
#include "Java/JavaObject.h"

namespace Java
{
	class JavaDisplayMode : public JavaObject
	{
	private:
		static jmethodID getWidth;
		static jmethodID getHeight;
		static jmethodID getRefreshRate;
		static jmethodID getBitDepth;
	public:
		JavaDisplayMode(jobject me);
		virtual ~JavaDisplayMode();

		Int32 GetWidth();
		Int32 GetHeight();
		Int32 GetBitDepth();
		Int32 GetRefreshRate();

		static jclass GetClass();
	};
}
#endif
