#ifndef _SM_TEXT_MIMEOBJ_MULTIPARTMIMEOBJ
#define _SM_TEXT_MIMEOBJ_MULTIPARTMIMEOBJ
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "Text/IMIMEObj.h"
#include "Text/MIMEObj/MIMEHeader.h"

namespace Text
{
	namespace MIMEObj
	{
		class MultipartMIMEObj : public Text::IMIMEObj
		{
		public:
			class PartInfo : public Text::MIMEObj::MIMEHeader
			{
			private:
				Text::IMIMEObj *obj;

			public:
				PartInfo(Text::IMIMEObj *obj);
				virtual ~PartInfo();

				Text::IMIMEObj *GetObject();
				PartInfo *Clone();
			};
		private:
			Text::String *contentType;
			Text::String *boundary;
			Text::String *defMsg;
			Data::ArrayList<PartInfo*> *parts;

			void ParsePart(UInt8 *buff, UOSInt buffSize);
			MultipartMIMEObj(Text::String *contentType, Text::String *defMsg, Text::String *boundary);
			MultipartMIMEObj(Text::CString contentType, Text::CString defMsg, Text::CString boundary);
		public:
			MultipartMIMEObj(const UTF8Char *contentType, Text::CString defMsg);
			virtual ~MultipartMIMEObj();

			virtual Text::CString GetClassName();
			virtual Text::CString GetContentType();
			virtual UOSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			Text::String *GetDefMsg();
			UOSInt AddPart(Text::IMIMEObj *obj);
			Bool AddPartHeader(UOSInt partIndex, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			Text::IMIMEObj *GetPartObj(UOSInt partIndex);
			PartInfo *GetPart(UOSInt partIndex);
			UOSInt GetPartCount();

			static MultipartMIMEObj *ParseFile(Text::CString contentType, IO::IStreamData *data);
		};
	}
}
#endif
