#ifndef _SM_IO_FILEANALYSE_FRAMEDETAILHANDLER
#define _SM_IO_FILEANALYSE_FRAMEDETAILHANDLER

namespace IO
{
	namespace FileAnalyse
	{
		class FrameDetailHandler
		{
		public:
			virtual ~FrameDetailHandler() {};

			virtual void AddHeader(const UTF8Char *header) = 0;
			virtual void AddField(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value) = 0;
			virtual void AddSubfield(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value) = 0;
			virtual void AddFieldSeperstor(UOSInt ofst, const UTF8Char *name) = 0;
			virtual void AddText(UOSInt ofst, const UTF8Char *name) = 0;
			virtual void AddSubframe(UOSInt ofst, UOSInt size) = 0;

			void AddBool(UOSInt frameOfst, const Char *name, UInt8 v);
			void AddInt64(UOSInt frameOfst, const Char *name, Int64 v);
			void AddInt64V(UOSInt frameOfst, UOSInt size, const Char *name, Int64 v);
			void AddUInt64(UOSInt frameOfst, const Char *name, UInt64 v);
			void AddUInt64V(UOSInt frameOfst, UOSInt size, const Char *name, UInt64 v);
			void AddInt(UOSInt frameOfst, UOSInt size, const Char *name, OSInt v);
			void AddUInt(UOSInt frameOfst, UOSInt size, const Char *name, UOSInt v);
			void AddUIntName(UOSInt frameOfst, UOSInt size, const Char *name, UOSInt v, const UTF8Char *vName);
			void AddUInt64Name(UOSInt frameOfst, UOSInt size, const Char *name, UInt64 v, const UTF8Char *vName);
			void AddFloat(UOSInt frameOfst, UOSInt size, const Char *name, Double v);
			void AddHex8(UOSInt frameOfst, const Char *name, UInt8 v);
			void AddHex16(UOSInt frameOfst, const Char *name, UInt16 v);
			void AddHex24(UOSInt frameOfst, const Char *name, UInt32 v);
			void AddHex32(UOSInt frameOfst, const Char *name, UInt32 v);
			void AddHex64(UOSInt frameOfst, const Char *name, UInt64 v);
			void AddHex64V(UOSInt frameOfst, UOSInt size, const Char *name, UInt64 v);
			void AddHex8Name(UOSInt frameOfst, const Char *name, UInt8 v, const UTF8Char *vName);
			void AddHex16Name(UOSInt frameOfst, const Char *name, UInt16 v, const UTF8Char *vName);
			void AddStrC(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *vBuff);
			void AddStrS(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *vBuff);
			void AddHexBuff(UOSInt frameOfst, UOSInt size, const Char *name, const UInt8 *vBuff, Bool multiLine);
			void AddHexBuff(UOSInt frameOfst, UOSInt size, const Char *name, const UInt8 *vBuff, UTF8Char seperator, Bool multiLine);
			void AddIPv4(UOSInt frameOfst, const Char *name, const UInt8 *vBuff);
			void AddIPv6(UOSInt frameOfst, const Char *name, const UInt8 *vBuff);
			void AddMACAddr(UOSInt frameOfst, const Char *name, const UInt8 *macBuff, Bool showVendor);
			void AddNetBIOSName(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *nbName);

			void AddTextHexBuff(UOSInt frameOfst, UOSInt size, const UInt8 *vBuff, Bool multiLine);
		};
	}
}
#endif
