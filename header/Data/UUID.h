#ifndef _SM_DATA_UUID
#define _SM_DATA_UUID
#include "Text/StringBuilderUTF.h"

namespace Data
{
	class UUID
	{
	private:
		UInt8 data[16];

	public:
		UUID(const UInt8 *buff);
		~UUID();

		void ToString(Text::StringBuilderUTF *sb);
		UTF8Char *ToString(UTF8Char *sbuff);
	};
}
#endif
