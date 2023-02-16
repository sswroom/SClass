#ifndef _SM_DATA_CIRCULARBYTEBUFF
#define _SM_DATA_CIRCULARBYTEBUFF
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class CircularByteBuff
	{
	private:
		UInt8 *buff;
		UOSInt buffSize;
		UOSInt indexBegin;
		UOSInt indexEnd;

	public:
		CircularByteBuff(UOSInt maxSize);
		~CircularByteBuff();

		void AppendBytes(const UInt8 *buff, UOSInt buffSize);
		UOSInt GetBytes(UInt8 *buff);
		void ToString(Text::StringBuilderUTF8 *sb);
	};
}
#endif
