#ifndef _SM_TEXT_MIMEOBJ_MULTIPARTMIMEOBJ
#define _SM_TEXT_MIMEOBJ_MULTIPARTMIMEOBJ
#include "Data/ArrayListNN.hpp"
#include "IO/StreamData.h"
#include "Text/MIMEObject.h"
#include "Text/MIMEObj/MIMEMessage.h"

namespace Text
{
	namespace MIMEObj
	{
		class MultipartMIMEObj : public Text::MIMEObject
		{
		private:
			NN<Text::String> contentType;
			NN<Text::String> boundary;
			Optional<Text::String> defMsg;
			Data::ArrayListNN<MIMEMessage> parts;

			void ParsePart(UInt8 *buff, UIntOS buffSize);
			MultipartMIMEObj(NN<Text::String> contentType, Optional<Text::String> defMsg, NN<Text::String> boundary);
			MultipartMIMEObj(Text::CStringNN contentType, Text::CString defMsg, Text::CStringNN boundary);
		public:
			MultipartMIMEObj(Text::CStringNN contentType, Text::CString defMsg);
			virtual ~MultipartMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UIntOS WriteStream(NN<IO::Stream> stm) const;
			virtual NN<MIMEObject> Clone() const;

			Optional<Text::String> GetDefMsg() const;
			UIntOS AddPart(NN<Text::MIMEObject> obj);
			void SetPartTransferData(UIntOS partIndex, const UInt8 *data, UIntOS dataSize);
			Bool AddPartHeader(UIntOS partIndex, Text::CStringNN name, Text::CStringNN value);
			Optional<Text::MIMEObject> GetPartContent(UIntOS partIndex) const;
			Optional<MIMEMessage> GetPart(UIntOS partIndex) const;
			UIntOS GetPartCount() const;

			static Optional<MultipartMIMEObj> ParseFile(Text::CStringNN contentType, NN<IO::StreamData> data);
		};
	}
}
#endif
