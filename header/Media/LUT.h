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

		void Init(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt);
	public:
		LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, Text::String *sourceName);
		LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, Text::CString sourceName);
		virtual ~LUT();

		virtual IO::ParserType GetParserType();

		void SetRemark(Text::String *remark);
		void SetRemark(Text::CString remark);
		Text::String *GetRemark();
		UOSInt GetInputCh() const;
		DataFormat GetFormat() const;
		UOSInt GetInputLevel() const;
		UOSInt GetOutputCh() const;

		UInt8 *GetTablePtr();
		const UInt8 *GetTablePtrRead() const;
		void GetValueUInt8(UInt32 *inputVals, UInt8 *outVals);
		void GetValueUInt16(UInt32 *inputVals, UInt16 *outVals);
		void GetValueSingle(UInt32 *inputVals, Single *outVals);

		LUT *Clone() const;
		Bool Equals(Media::LUT *lut);
	};
}
#endif
