#ifndef _SM_MEDIA_JASPER_JASPERELEMENT
#define _SM_MEDIA_JASPER_JASPERELEMENT
#include "Media/Jasper/JasperReportElement.h"
namespace Media
{
	namespace Jasper
	{
		class JasperElement
		{
		public:
			enum class ElementType
			{
				StaticText,
				TextField,
				Image,
				Line
			};
		private:
			JasperReportElement *reportElement;
		public:
			JasperElement();
			virtual ~JasperElement();

			virtual ElementType GetType() const = 0;
			void SetReportElement(JasperReportElement *reportElement);
			JasperReportElement *GetReportElement() const;
		};
	}
}
#endif
