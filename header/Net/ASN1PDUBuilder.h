#ifndef _SM_NET_ASN1PDUBUILDER
#define _SM_NET_ASN1PDUBUILDER

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
		void EndLevel();
		void EndAll();
		void AppendBool(Bool v);
		void AppendInt32(Int32 v);
		void AppendUInt32(UInt32 v);
		void AppendBitString(const UInt8 *buff, UOSInt len);
		void AppendOctetString(const UInt8 *buff, UOSInt len);
		void AppendOctetStringS(const UTF8Char *s);
		void AppendNull();
		void AppendOID(const UInt8 *oid, UOSInt len);
		void AppendOIDString(const Char *oidStr);
		void AppendChoice(UInt32 v);
		void AppendPrintableString(const UTF8Char *s);
		void AppendUTF8String(const UTF8Char *s);
		void AppendIA5String(const UTF8Char *s);
		void AppendOther(UInt8 type, const UInt8 *buff, UOSInt buffSize);

		const UInt8 *GetBuff(UOSInt *buffSize);
		UOSInt GetBuffSize();
	};
}
#endif
