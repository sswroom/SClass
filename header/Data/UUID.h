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
		UUID();
		UUID(const UInt8 *buff);
		~UUID();

		void SetValue(const UInt8 *buff);
		void SetValue(UUID *uuid);
		OSInt CompareTo(UUID *uuid);
		
		void ToString(Text::StringBuilderUTF *sb);
		UTF8Char *ToString(UTF8Char *sbuff);
		UUID *Clone();
	};
}
#endif