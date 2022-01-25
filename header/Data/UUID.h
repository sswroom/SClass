#ifndef _SM_DATA_UUID
#define _SM_DATA_UUID
#include "Text/StringBuilderUTF8.h"

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
		UOSInt GetValue(UInt8 *buff);
		OSInt CompareTo(UUID *uuid);
		
		void ToString(Text::StringBuilderUTF8 *sb);
		UTF8Char *ToString(UTF8Char *sbuff);
		UUID *Clone();
		Bool Equals(UUID *uuid);
	};
}
#endif
