#ifndef _SM_IO_VIRTUALPACKAGEFILEICASE
#define _SM_IO_VIRTUALPACKAGEFILEICASE
#include "Data/ICaseStringMapObj.hpp"
#include "IO/VirtualPackageFile.h"

namespace IO
{
	class VirtualPackageFileICase : public VirtualPackageFile
	{
	private:
		Data::ICaseStringMapObj<PackFileItem *> namedItems;

		VirtualPackageFileICase(NN<const VirtualPackageFileICase> pkg);
	public:
		VirtualPackageFileICase(NN<Text::String> fileName);
		VirtualPackageFileICase(Text::CStringNN fileName);
		virtual ~VirtualPackageFileICase();

		virtual Optional<const PackFileItem> GetItemByName(Text::CStringNN name) const;
		virtual void PutItem(NN<Text::String> name, NN<PackFileItem> item);
		virtual void RemoveItem(NN<Text::String> name);
		virtual NN<PackageFile> Clone() const;
	};
}
#endif
