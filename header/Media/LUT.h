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
		const UTF8Char *remark;
		OSInt inputCh;
		OSInt inputLev;
		OSInt outputCh;
		UInt8 *luTable;
		DataFormat fmt;

	public:
		LUT(OSInt inputCh, OSInt inputLev, OSInt outputCh, DataFormat fmt, const UTF8Char *sourceName);
		virtual ~LUT();

		virtual IO::ParsedObject::ParserType GetParserType();

		void SetRemark(const UTF8Char *remark);
		const UTF8Char *GetRemark();
		OSInt GetInputCh();
		DataFormat GetFormat();
		OSInt GetInputLevel();
		OSInt GetOutputCh();

		UInt8 *GetTablePtr();
		void GetValueUInt8(Int32 *inputVals, UInt8 *outVals);
		void GetValueUInt16(Int32 *inputVals, UInt16 *outVals);
		void GetValueSingle(Int32 *inputVals, Single *outVals);

		LUT *Clone();
		Bool Equals(Media::LUT *lut);
	};
};
#endif
