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
			virtual void AddField(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value) = 0;
			virtual void AddSubfield(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value) = 0;
			virtual void AddFieldSeperstor(UInt32 ofst, const UTF8Char *name) = 0;
			virtual void AddText(UInt32 ofst, const UTF8Char *name) = 0;

			void AddBool(UInt32 frameOfst, const Char *name, UInt8 v);
			void AddInt64(UInt32 frameOfst, const Char *name, Int64 v);
			void AddInt(UInt32 frameOfst, UInt32 size, const Char *name, OSInt v);
			void AddUInt(UInt32 frameOfst, UInt32 size, const Char *name, UOSInt v);
			void AddUIntName(UInt32 frameOfst, UInt32 size, const Char *name, UOSInt v, const UTF8Char *vName);
			void AddHex8(UInt32 frameOfst, const Char *name, UInt8 v);
			void AddHex16(UInt32 frameOfst, const Char *name, UInt16 v);
			void AddHex24(UInt32 frameOfst, const Char *name, UInt32 v);
			void AddHex32(UInt32 frameOfst, const Char *name, UInt32 v);
			void AddHex8Name(UInt32 frameOfst, const Char *name, UInt8 v, const UTF8Char *vName);
			void AddHex16Name(UInt32 frameOfst, const Char *name, UInt16 v, const UTF8Char *vName);
			void AddStrC(UInt32 frameOfst, UInt32 size, const Char *name, const UTF8Char *vBuff);
			void AddHexBuff(UInt32 frameOfst, UInt32 size, const Char *name, const UInt8 *vBuff, Bool multiLine);
			void AddIPv4(UInt32 frameOfst, const Char *name, const UInt8 *vBuff);
			void AddMACAddr(UInt32 frameOfst, const Char *name, const UInt8 *macBuff, Bool showVendor);
		};
	}
}
#endif
