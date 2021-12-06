#ifndef _SM_MEDIA_LUT
#define _SM_MEDIA_LUT
#include "IO/ParsedObject.h"

namespace Media
{
	class LUT : public IO::ParsedObject
	{
	public:
		typedef enum
		{
			DF_UINT8,
			DF_UINT16,
			DF_SINGLE
		} DataFormat;
	private:
		Text::String *remark;
		UOSInt inputCh;
		UOSInt inputLev;
		UOSInt outputCh;
		UInt8 *luTable;
		DataFormat fmt;

		void Init();
	public:
		LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, Text::String *sourceName);
		LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, const UTF8Char *sourceName);
		virtual ~LUT();

		virtual IO::ParserType GetParserType();

		void SetRemark(Text::String *remark);
		void SetRemark(const UTF8Char *remark);
		Text::String *GetRemark();
		UOSInt GetInputCh();
		DataFormat GetFormat();
		UOSInt GetInputLevel();
		UOSInt GetOutputCh();

		UInt8 *GetTablePtr();
		void GetValueUInt8(UInt32 *inputVals, UInt8 *outVals);
		void GetValueUInt16(UInt32 *inputVals, UInt16 *outVals);
		void GetValueSingle(UInt32 *inputVals, Single *outVals);

		LUT *Clone();
		Bool Equals(Media::LUT *lut);
	};
}
#endif
