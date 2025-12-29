#ifndef _SM_MEDIA_PDFOBJECT
#define _SM_MEDIA_PDFOBJECT
#include "IO/Stream.h"
#include "IO/StreamData.h"
#include "Media/PDFParameter.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class PDFObject
	{
	private:
		UInt32 id;
		Bool streamData;
		Optional<IO::StreamData> fd;
		Optional<IO::StreamData> decFd;
		Optional<PDFParameter> parameter;
	public:
		PDFObject(UInt32 id);
		~PDFObject();

		void SetStream(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len);
		void SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len);
		void SetParameter(Optional<PDFParameter> parameter);
		Optional<PDFParameter> GetParameter() const;
		UInt32 GetId() const;
		Bool IsImage() const;
		Bool IsFlateDecode() const;
		Optional<Text::String> GetType() const;
		Optional<Text::String> GetSubtype() const;
		Optional<Text::String> GetFilter() const;
		Optional<Text::String> GetColorSpace() const;
		UOSInt GetBitPerComponent() const;
		UOSInt GetWidth() const;
		UOSInt GetHeight() const;
		Bool ToString(NN<Text::StringBuilderUTF8> sb) const;
		Optional<IO::StreamData> GetRAWData() const;
		Optional<IO::StreamData> GetData();
		Bool SaveFile(Text::CStringNN fileName);
		Bool SaveStream(NN<IO::Stream> stm);
	};
}
#endif
