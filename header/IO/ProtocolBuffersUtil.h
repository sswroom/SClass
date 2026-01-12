#ifndef _SM_IO_PROTOCOLBUFFERSUTIL
#define _SM_IO_PROTOCOLBUFFERSUTIL
#include "Text/CString.h" 

namespace IO
{
	class ProtocolBuffersUtil
	{
	public:
		enum class FieldType
		{
			Int32,
			Int64,
			SInt32,
			SInt64,
			UInt32,
			UInt64,
			String,
			Bytes,
			Fixed32,
			Fixed64,
			SFixed32,
			SFixed64,
			Float,
			Double,
			Bool,
			Enum,
			SubMsg
		};

		static UOSInt ReadVarUInt(UnsafeArray<const UInt8> buff, UOSInt buffOfst, OutParam<UInt64> val);
		static UOSInt ReadVarInt(UnsafeArray<const UInt8> buff, UOSInt buffOfst, OutParam<Int64> val);
		static UOSInt ReadVarSInt(UnsafeArray<const UInt8> buff, UOSInt buffOfst, OutParam<Int64> val);
		static Int64 ToSInt64(UInt64 val);
		static Text::CStringNN WireTypeGetName(UInt8 wireType);
	};
}
#endif
