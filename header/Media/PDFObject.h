#ifndef _SM_MEDIA_PDFOBJECT
#define _SM_MEDIA_PDFOBJECT
#include "IO/MemoryStream.h"
#include "IO/StreamData.h"
#include "Media/PDFParameter.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class PDFDocument;
	class PDFObject
	{
	public:
		enum class ImageType
		{
			NonImage,
			Unknown,
			PlainImage,
			JPEG,
			CCITTFax,
			JBIG2
		};
	private:
		UInt32 id;
		NN<PDFDocument> document;
		Bool streamData;
		Optional<IO::StreamData> fd;
		Optional<IO::StreamData> decFd;
		Optional<PDFParameter> parameter;

		static Bool IsSingleFilter(Text::CStringNN filter, Text::CStringNN target);
		static Bool ApplyPredictor(NN<IO::MemoryStream> decoded, NN<IO::MemoryStream> output, UIntOS predictor, UIntOS colors, UIntOS columns, UIntOS bits);
		Bool ReadPredictorParams(NN<PDFParameter> parameter, UIntOS &earlyChange, UIntOS &predictor, UIntOS &colors, UIntOS &columns, UIntOS &bits);
	public:
		PDFObject(UInt32 id, NN<PDFDocument> document);
		~PDFObject();

		void SetStream(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len);
		void SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len);
		void SetParameter(Optional<PDFParameter> parameter);
		void ClearContents();
		void ClearDecodedData();
		Optional<PDFParameter> GetParameter() const;
		UInt32 GetId() const;
		Bool IsImage() const;
		ImageType GetImageType(NN<PDFDocument> doc) const;
		Bool IsFlateDecode() const;
		Optional<Text::String> GetType() const;
		Optional<Text::String> GetSubtype() const;
		Optional<Text::String> GetFilter() const;
		Optional<Text::String> GetColorSpace() const;
		UIntOS GetBitsPerComponent() const;
		UIntOS GetWidth() const;
		UIntOS GetHeight() const;
		Bool ToString(NN<Text::StringBuilderUTF8> sb) const;
		Optional<IO::StreamData> GetRAWData() const;
		Optional<IO::StreamData> GetData();
		Bool SaveFile(Text::CStringNN fileName);
		Bool SaveStream(NN<IO::Stream> stm);

		static Text::CStringNN ImageTypeGetName(ImageType imgType);
	};
}
#endif
