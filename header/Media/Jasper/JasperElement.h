#ifndef _SM_MEDIA_JASPER_JASPERELEMENT
#define _SM_MEDIA_JASPER_JASPERELEMENT
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
		public:
			virtual ~JasperElement() {};

			virtual ElementType GetType() const = 0;
		};
	}
}
#endif
