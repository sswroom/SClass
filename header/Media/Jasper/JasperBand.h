#ifndef _SM_MEDIA_JASPER_JASPERBAND
#define _SM_MEDIA_JASPER_JASPERBAND
#include "Data/ArrayListNN.h"
#include "Media/Jasper/JasperElement.h"
#include "Text/String.h"
namespace Media
{
	namespace Jasper
	{
		class JasperBand
		{
		private:
			Data::ArrayListNN<JasperElement> elements;
			UOSInt height;
			Optional<Text::String> splitType;
		public:
			JasperBand();
			~JasperBand();

			void SetSplitType(Optional<Text::String> splitType);
			void SetHeight(UOSInt height);
			Optional<Text::String> GetSplitType() const;
			Bool HasHeight() const;
			UOSInt GetHeight() const;
			void AddElement(NN<JasperElement> element);

			UOSInt GetCount() const;
			Optional<JasperElement> GetElement(UOSInt index) const;
		};
	}
}
#endif
