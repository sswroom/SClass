#ifndef _SM_DATA_BINARYBUILDER
#define _SM_DATA_BINARYBUILDER
#include "Data/Timestamp.h"
#include "IO/MemoryStream.h"
#include "Net/SocketUtil.h"

namespace Data
{
	class BinaryBuilder
	{
	private:
		IO::MemoryStream mstm;
	public:
		BinaryBuilder();
		BinaryBuilder(UOSInt initSize);
		~BinaryBuilder();

		void AppendI32(Int32 val);
		void AppendU32(UInt32 val);
		void AppendNI32(NInt32 val);
		void AppendI64(Int64 val);
		void AppendF64(Double val);
		void AppendStr(Text::CString s);
		void AppendStrOpt(Optional<Text::String> s);
		void AppendChar(UTF8Char c);
		void AppendBool(Bool b);
		void AppendTS(Data::Timestamp ts);
		void AppendDate(Data::Date dat);
		void AppendIPAddr(NN<Net::SocketUtil::AddressInfo> addr);
		void AppendBArr(Data::ByteArrayR barr);

		Data::ByteArrayR Build();
	};
}
#endif
