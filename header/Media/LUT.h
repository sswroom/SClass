#ifndef _SM_MEDIA_LUT
#define _SM_MEDIA_LUT
#include "Data/ByteBuffer.h"
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
		Optional<Text::String> remark;
		UOSInt inputCh;
		UOSInt inputLev;
		UOSInt outputCh;
		Data::ByteBuffer luTable;
		DataFormat fmt;

		void Init(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt);
	public:
		LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, NN<Text::String> sourceName);
		LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, Text::CStringNN sourceName);
		virtual ~LUT();

		virtual IO::ParserType GetParserType() const;

		void SetRemark(Optional<Text::String> remark);
		void SetRemark(Text::CString remark);
		Optional<Text::String> GetRemark() const;
		UOSInt GetInputCh() const;
		DataFormat GetFormat() const;
		UOSInt GetInputLevel() const;
		UOSInt GetOutputCh() const;

		UInt8 *GetTablePtr();
		const UInt8 *GetTablePtrRead() const;
		Data::ByteArray GetTableArray() const;
		void GetValueUInt8(UInt32 *inputVals, UInt8 *outVals) const;
		void GetValueUInt16(UInt32 *inputVals, UInt16 *outVals) const;
		void GetValueSingle(UInt32 *inputVals, Single *outVals) const;

		LUT *Clone() const;
		Bool Equals(Media::LUT *lut) const;
	};
}
#endif
