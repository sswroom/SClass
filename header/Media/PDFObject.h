#ifndef _SM_MEDIA_PDFOBJECT
#define _SM_MEDIA_PDFOBJECT
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

		void SetStream(IO::StreamData *fd, UInt64 ofst, UInt64 len);
		void SetData(IO::StreamData *fd, UInt64 ofst, UInt64 len);
		void SetParameter(PDFParameter *parameter);
		PDFParameter *GetParameter();
	};
}
#endif
