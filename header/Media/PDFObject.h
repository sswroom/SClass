#ifndef _SM_MEDIA_PDFOBJECT
#define _SM_MEDIA_PDFOBJECT
#include "IO/Stream.h"
#include "IO/StreamData.h"
#include "Media/PDFParameter.h"

namespace Media
{
	class PDFObject
	{
	private:
		UInt32 id;
		Bool streamData;
		IO::StreamData *fd;
		PDFParameter *parameter;
	public:
		PDFObject(UInt32 id);
		~PDFObject();

		void SetStream(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 len);
		void SetData(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 len);
		void SetParameter(PDFParameter *parameter);
		PDFParameter *GetParameter() const;
		UInt32 GetId() const;
		Bool IsImage() const;
		Optional<Text::String> GetType() const;
		Optional<Text::String> GetSubtype() const;
		Optional<Text::String> GetFilter() const;
		Optional<Text::String> GetColorSpace() const;
		UOSInt GetBitPerComponent() const;
		UOSInt GetWidth() const;
		UOSInt GetHeight() const;
		IO::StreamData *GetData() const;
		Bool SaveFile(Text::CStringNN fileName);
		Bool SaveStream(NotNullPtr<IO::Stream> stm);
	};
}
#endif
