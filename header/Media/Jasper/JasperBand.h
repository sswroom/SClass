#ifndef _SM_MEDIA_JASPER_JASPERBAND
#define _SM_MEDIA_JASPER_JASPERBAND
#include "Data/ArrayListNN.hpp"
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
			UIntOS height;
			Optional<Text::String> splitType;
		public:
			JasperBand();
			~JasperBand();

			void SetSplitType(Optional<Text::String> splitType);
			void SetHeight(UIntOS height);
			Optional<Text::String> GetSplitType() const;
			Bool HasHeight() const;
			UIntOS GetHeight() const;
			void AddElement(NN<JasperElement> element);

			UIntOS GetCount() const;
			Optional<JasperElement> GetElement(UIntOS index) const;
		};
	}
}
#endif
