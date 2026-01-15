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
		UIntOS inputCh;
		UIntOS inputLev;
		UIntOS outputCh;
		Data::ByteBuffer luTable;
		DataFormat fmt;

		void Init(UIntOS inputCh, UIntOS inputLev, UIntOS outputCh, DataFormat fmt);
	public:
		LUT(UIntOS inputCh, UIntOS inputLev, UIntOS outputCh, DataFormat fmt, NN<Text::String> sourceName);
		LUT(UIntOS inputCh, UIntOS inputLev, UIntOS outputCh, DataFormat fmt, Text::CStringNN sourceName);
		virtual ~LUT();

		virtual IO::ParserType GetParserType() const;

		void SetRemark(Optional<Text::String> remark);
		void SetRemark(Text::CString remark);
		Optional<Text::String> GetRemark() const;
		UIntOS GetInputCh() const;
		DataFormat GetFormat() const;
		UIntOS GetInputLevel() const;
		UIntOS GetOutputCh() const;

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
