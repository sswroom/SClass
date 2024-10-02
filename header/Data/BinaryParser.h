#ifndef _SM_DATA_BINARYPARSER
#define _SM_DATA_BINARYPARSER
#include "Data/Timestamp.h"
#include "Net/SocketUtil.h"
#include "Text/String.h"

namespace Data
{
	class BinaryParser
	{
	private:
		Data::ByteArrayR buff;
		UOSInt currOfst;
		Bool error;

	public:
		BinaryParser(Data::ByteArrayR buff);
		~BinaryParser();

		Int32 NextI32();
		UInt32 NextU32();
		NInt32 NextNI32();
		Double NextF64();
		Text::CStringNN NextStrNN();
		Text::CString NextStr();
		Optional<Text::String> NextStrOpt();
		UTF8Char NextChar();
		Bool NextBool();
		Data::Timestamp NextTS();
		Data::Date NextDate();
		Bool NextIPAddr(NN<Net::SocketUtil::AddressInfo> addr);

		Bool HasError();
	};
}
#endif
