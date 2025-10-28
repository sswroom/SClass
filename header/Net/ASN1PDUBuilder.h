#ifndef _SM_NET_ASN1PDUBUILDER
#define _SM_NET_ASN1PDUBUILDER
#include "Data/ByteArray.h"
#include "Data/DateTime.h"
#include "Text/String.h"

namespace Net
{
	class ASN1PDUBuilder
	{
	private:
		UOSInt seqOffset[16];
		UOSInt currLev;
		UOSInt buffSize;
		UnsafeArray<UInt8> buff;
		UOSInt currOffset;

	public:
		ASN1PDUBuilder();
		~ASN1PDUBuilder();

		void AllocateSize(UOSInt size);
		void BeginOther(UInt8 type);
		void BeginSequence();
		void BeginSet();
		void BeginContentSpecific(UInt8 n);
		void EndLevel();
		void EndAll();
		void AppendBool(Bool v);
		void AppendInt32(Int32 v);
		void AppendUInt32(UInt32 v);
		void AppendBitString(UInt8 bitLeft, UnsafeArray<const UInt8> buff, UOSInt buffLen);
		void AppendBitString(UInt8 bitLeft, Data::ByteArrayR buff);
		void AppendOctetString(UnsafeArray<const UInt8> buff, UOSInt len);
		void AppendOctetString(Optional<Text::String> s);
		void AppendOctetString(NN<Text::String> s);
		void AppendOctetStringC(Text::CStringNN s);
		void AppendNull();
		void AppendOID(UnsafeArray<const UInt8> oid, UOSInt len);
		void AppendOID(Data::ByteArrayR oid);
		void AppendOIDString(Text::CStringNN oidStr);
		void AppendChoice(UInt32 v);
		void AppendPrintableString(NN<Text::String> s);
		void AppendUTF8String(NN<Text::String> s);
		void AppendIA5String(NN<Text::String> s);
		void AppendUTCTime(NN<Data::DateTime> t);
		void AppendOther(UInt8 type, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		void AppendContentSpecific(UInt8 n, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		void AppendSequence(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		void AppendInteger(UnsafeArray<const UInt8> buff, UOSInt buffSize);

		UnsafeArrayOpt<const UInt8> GetItemRAW(UnsafeArrayOpt<const Char> path, OptOut<UOSInt> itemLen, OutParam<UOSInt> itemOfst);
		UnsafeArray<const UInt8> GetBuff(OutParam<UOSInt> buffSize);
		UnsafeArray<const UInt8> GetBuff();
		UOSInt GetBuffSize();
		Data::ByteArrayR GetArray();
	};
}
#endif
