#ifndef _SM_IO_VIRTUALPACKAGEFILEFAST
#define _SM_IO_VIRTUALPACKAGEFILEFAST
#include "Data/FastStringMap.h"
#include "IO/VirtualPackageFile.h"

namespace IO
{
	class VirtualPackageFileFast : public VirtualPackageFile
	{
	private:
		Data::FastStringMap<PackFileItem *> namedItems;

		VirtualPackageFileFast(NotNullPtr<const VirtualPackageFileFast> pkg);
	public:
		VirtualPackageFileFast(NotNullPtr<Text::String> fileName);
		VirtualPackageFileFast(Text::CStringNN fileName);
		virtual ~VirtualPackageFileFast();

		virtual Optional<const PackFileItem> GetItemByName(Text::CStringNN name) const;
		virtual void PutItem(NotNullPtr<Text::String> name, NotNullPtr<PackFileItem> item);
		virtual void RemoveItem(NotNullPtr<Text::String> name);
		virtual NotNullPtr<PackageFile> Clone() const;
	};
}
#endif
