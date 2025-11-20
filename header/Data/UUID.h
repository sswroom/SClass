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
		UUID(UnsafeArray<const UInt8> buff);
		UUID(Text::CStringNN str);
		~UUID();

		void SetValue(UnsafeArray<const UInt8> buff);
		void SetValue(NN<UUID> uuid);
		void SetValue(Text::CStringNN str);
		UOSInt GetValue(UnsafeArray<UInt8> buff) const;
		OSInt CompareTo(NN<UUID> uuid) const;

		UInt32 GetTimeLow() const;
		UInt16 GetTimeMid() const;
		UInt16 GetTimeHiAndVersion() const;
		UInt8 GetClkSeqHiRes() const;
		UInt8 GetClkSeqLow() const;
		UInt64 GetNode() const;
		UnsafeArray<const UInt8> GetBytes() const;

		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> sbuff) const;
		NN<UUID> Clone() const;
		Bool Equals(NN<UUID> uuid) const;
		void GenerateV4();
	};
}
#endif
