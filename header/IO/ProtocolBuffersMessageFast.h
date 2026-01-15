#ifndef _SM_IO_PROTOCOLBUFFERSMESSAGEFAST
#define _SM_IO_PROTOCOLBUFFERSMESSAGEFAST
#include "Data/ByteBuffer.h"
#include "Data/FastMapNN.hpp"
#include "IO/ProtocolBuffersUtil.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class ProtocolBuffersMessageFast
	{
	private:
		struct FieldInfo
		{
			UInt8 id;
			ProtocolBuffersUtil::FieldType fieldType;
			Bool required;
			Bool valExist;
			union
			{
				Int32 int32Val;
				Int64 int64Val;
				UInt32 uint32Val;
				UInt64 uint64Val;
				Single floatVal;
				Double doubleVal;
				Bool boolVal;
				Optional<Text::String> strVal;
				Optional<Data::ByteBuffer> bytesVal;
				Optional<ProtocolBuffersMessageFast> subMsgVal;
			} val;
		};

		Data::UInt32FastMapNN<FieldInfo> fieldMap;
		static void __stdcall FreeFieldInfo(NN<FieldInfo> fieldInfo);
	public:
		ProtocolBuffersMessageFast();
		~ProtocolBuffersMessageFast();

		void AddInt32(Bool required, UInt8 id);
		void AddInt64(Bool required, UInt8 id);
		void AddSInt32(Bool required, UInt8 id);
		void AddSInt64(Bool required, UInt8 id);
		void AddUInt32(Bool required, UInt8 id);
		void AddUInt64(Bool required, UInt8 id);
		void AddString(Bool required, UInt8 id);
		void AddBytes(Bool required, UInt8 id);
		void AddFixed32(Bool required, UInt8 id);
		void AddFixed64(Bool required, UInt8 id);
		void AddSFixed32(Bool required, UInt8 id);
		void AddSFixed64(Bool required, UInt8 id);
		void AddFloat(Bool required, UInt8 id);
		void AddDouble(Bool required, UInt8 id);
		void AddBool(Bool required, UInt8 id);
		void AddEnum(Bool required, UInt8 id);

		void ClearValues();
		Bool GetInt32(UInt8 id, OutParam<Int32> val) const;
		Bool GetInt64(UInt8 id, OutParam<Int64> val) const;
		Bool GetUInt32(UInt8 id, OutParam<UInt32> val) const;
		Bool GetUInt64(UInt8 id, OutParam<UInt64> val) const;
		Bool GetString(UInt8 id, OutParam<NN<Text::String>> val) const;
		Bool GetBytes(UInt8 id, OutParam<NN<Data::ByteBuffer>> val) const;
		Bool GetFixed32(UInt8 id, OutParam<UInt32> val) const;
		Bool GetFixed64(UInt8 id, OutParam<UInt64> val) const;
		Bool GetSFixed32(UInt8 id, OutParam<Int32> val) const;
		Bool GetSFixed64(UInt8 id, OutParam<Int64> val) const;
		Bool GetFloat(UInt8 id, OutParam<Single> val) const;
		Bool GetDouble(UInt8 id, OutParam<Double> val) const;
		Bool GetBool(UInt8 id, OutParam<Bool> val) const;
		Bool GetEnum(UInt8 id, OutParam<Int32> val) const;

		Bool ParseMsssage(UnsafeArray<const UInt8> buff, UIntOS buffSize);
	};
}
#endif
