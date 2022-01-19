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
			const UTF8Char *boundary;
			const UTF8Char *defMsg;
			Data::ArrayList<PartInfo*> *parts;

			void ParsePart(UInt8 *buff, UOSInt buffSize);
			MultipartMIMEObj(Text::String *contentType, const UTF8Char *defMsg, const UTF8Char *boundary);
			MultipartMIMEObj(const UTF8Char *contentType, UOSInt contTypeLen, const UTF8Char *defMsg, const UTF8Char *boundary);
		public:
			MultipartMIMEObj(const UTF8Char *contentType, const UTF8Char *defMsg);
			virtual ~MultipartMIMEObj();

			virtual const UTF8Char *GetClassName();
			virtual Text::CString GetContentType();
			virtual UOSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			const UTF8Char *GetDefMsg();
			UOSInt AddPart(Text::IMIMEObj *obj);
			Bool AddPartHeader(UOSInt partIndex, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			Text::IMIMEObj *GetPartObj(UOSInt partIndex);
			PartInfo *GetPart(UOSInt partIndex);
			UOSInt GetPartCount();

			static MultipartMIMEObj *ParseFile(const UTF8Char *contentType, IO::IStreamData *data);
		};
	}
}
#endif
