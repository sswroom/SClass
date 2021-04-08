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
		void SequenceBegin(UInt8 type);
		void SequenceEnd();
		void AppendBool(Bool v);
		void AppendInt32(Int32 v);
		void AppendUInt32(UInt32 v);
		void AppendString(const UTF8Char *s);
		void AppendNull();
		void AppendOID(const UInt8 *oid, UOSInt len);
		void AppendChoice(UInt32 v);
		void AppendBuff(UInt8 type, const UInt8 *buff, UOSInt buffSize);

		const UInt8 *GetBuff(UOSInt *buffSize);
	};
}
#endif
