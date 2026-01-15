#ifndef _SM_DATA_CIRCULARBYTEBUFF
#define _SM_DATA_CIRCULARBYTEBUFF
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class CircularByteBuff
	{
	private:
		UnsafeArray<UInt8> buff;
		UIntOS buffSize;
		UIntOS indexBegin;
		UIntOS indexEnd;

	public:
		CircularByteBuff(UIntOS maxSize);
		~CircularByteBuff();

		void Clear();
		void AppendBytes(UnsafeArray<const UInt8> buff, UIntOS buffSize);
		UIntOS GetBytes(UnsafeArray<UInt8> buff);
		void ToString(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
