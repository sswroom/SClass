#ifndef _SM_IO_VIRTUALPACKAGEFILEICASE
#define _SM_IO_VIRTUALPACKAGEFILEICASE
#include "Data/ICaseStringMap.h"
#include "IO/VirtualPackageFile.h"

namespace IO
{
	class VirtualPackageFileICase : public VirtualPackageFile
	{
	private:
		Data::ICaseStringMap<PackFileItem *> namedItems;

		VirtualPackageFileICase(NotNullPtr<const VirtualPackageFileICase> pkg);
	public:
		VirtualPackageFileICase(NotNullPtr<Text::String> fileName);
		VirtualPackageFileICase(Text::CStringNN fileName);
		virtual ~VirtualPackageFileICase();

		virtual Optional<const PackFileItem> GetItemByName(Text::CStringNN name) const;
		virtual void PutItem(NotNullPtr<Text::String> name, NotNullPtr<PackFileItem> item);
		virtual void RemoveItem(NotNullPtr<Text::String> name);
		virtual NotNullPtr<PackageFile> Clone() const;
	};
}
#endif
