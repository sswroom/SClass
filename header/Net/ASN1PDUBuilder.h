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
		UInt8 *buff;
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
		void AppendBitString(UInt8 bitLeft, const UInt8 *buff, UOSInt len);
		void AppendOctetString(const UInt8 *buff, UOSInt len);
		void AppendOctetString(Text::String *s);
		void AppendOctetString(NotNullPtr<Text::String> s);
		void AppendOctetStringC(const UTF8Char *s, UOSInt len);
		void AppendNull();
		void AppendOID(const UInt8 *oid, UOSInt len);
		void AppendOIDString(const UTF8Char *oidStr, UOSInt oidStrLen);
		void AppendChoice(UInt32 v);
		void AppendPrintableString(Text::String *s);
		void AppendUTF8String(Text::String *s);
		void AppendIA5String(Text::String *s);
		void AppendUTCTime(Data::DateTime *t);
		void AppendOther(UInt8 type, const UInt8 *buff, UOSInt buffSize);
		void AppendContentSpecific(UInt8 n, const UInt8 *buff, UOSInt buffSize);
		void AppendSequence(const UInt8 *buff, UOSInt buffSize);
		void AppendInteger(const UInt8 *buff, UOSInt buffSize);

		const UInt8 *GetItemRAW(const Char *path, UOSInt *itemLen, UOSInt *itemOfst);
		const UInt8 *GetBuff(UOSInt *buffSize);
		UOSInt GetBuffSize();
		Data::ByteArrayR GetArray();
	};
}
#endif
