#ifndef _SM_MEDIA_JASPER_JASPERBAND
#define _SM_MEDIA_JASPER_JASPERBAND
#include "Data/ArrayList.h"
#include "Media/Jasper/JasperElement.h"
#include "Text/String.h"
namespace Media
{
	namespace Jasper
	{
		class JasperBand
		{
		private:
			Data::ArrayList<JasperElement*> elements;
			UOSInt height;
			Text::String *splitType;
		public:
			JasperBand();
			~JasperBand();

			void SetSplitType(Text::String *splitType);
			void SetHeight(UOSInt height);
			Text::String *GetSplitType() const;
			Bool HasHeight() const;
			UOSInt GetHeight() const;

			UOSInt GetCount() const;
			JasperElement *GetElement(UOSInt index) const;
		};
	}
}
#endif
