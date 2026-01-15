#ifndef _SM_TEXT_MIMEOBJ_UNKNOWNMIMEOBJ
#define _SM_TEXT_MIMEOBJ_UNKNOWNMIMEOBJ
#include "Text/MIMEObject.h"

namespace Text
{
	namespace MIMEObj
	{
		class UnknownMIMEObj : public Text::MIMEObject
		{
		private:
			UnsafeArray<UInt8> dataBuff;
			UIntOS buffSize;
			NN<Text::String> contType;

		public:
			UnknownMIMEObj(UnsafeArray<UInt8> dataBuff, UIntOS buffSize, Text::CStringNN contentType);
			virtual ~UnknownMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UIntOS WriteStream(NN<IO::Stream> stm) const;
			virtual NN<MIMEObject> Clone() const;

			UnsafeArray<const UInt8> GetRAWData(OutParam<UIntOS> dataSize) const;
		};
	}
}
#endif
