#ifndef _SM_IO_PROTOCOLBUFFERSMESSAGE
#define _SM_IO_PROTOCOLBUFFERSMESSAGE
#include "Data/ByteBuffer.h"
#include "Data/FastMapNN.hpp"
#include "IO/ProtocolBuffersUtil.h"
#include "IO/FileAnalyse/FileAnalyser.h"

namespace IO
{
	class ProtocolBuffersMessage
	{
	private:
		struct FieldInfo
		{
			UInt8 id;
			ProtocolBuffersUtil::FieldType fieldType;
			Bool required;
			Bool packed;
			Bool deltaCoded;
			NN<Text::String> name;
			UIntOS valCount;
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
				Optional<ProtocolBuffersMessage> subMsg;
			} val;
		};

		NN<Text::String> name;
		Data::UInt32FastMapNN<FieldInfo> fieldMap;

		static void __stdcall FreeFieldInfo(NN<FieldInfo> fieldInfo);
	public:
		ProtocolBuffersMessage(Text::CStringNN name);
		~ProtocolBuffersMessage();

		void AddInt64(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddSInt64(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddUInt64(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddInt32(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddSInt32(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddUInt32(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddBool(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddEnum(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddFixed64(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddSFixed64(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddFixed32(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddSFixed32(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddFloat(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddDouble(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddString(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddBytes(Bool required, Text::CStringNN name, UInt8 id, Bool packed, Bool deltaCoded = false);
		void AddSubMessage(Bool required, Text::CStringNN name, UInt8 id, NN<ProtocolBuffersMessage> subMsg);

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
		NN<ProtocolBuffersMessage> Clone() const;
		NN<Text::String> GetName() const;

		Bool ParseMsssage(NN<IO::FileAnalyse::FrameDetail> frame, UnsafeArray<const UInt8> buff, UIntOS buffOfst, UIntOS buffSize);
		void ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level);
	};
}
#endif
