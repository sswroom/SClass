#ifndef _SM_TEXT_VSPROJECT
#define _SM_TEXT_VSPROJECT
#include "Data/ArrayList.h"
#include "Text/CodeProject.h"

namespace Text
{
	class VSFile : public Text::CodeFile
	{
	private:
		NotNullPtr<Text::String> fileName;
	public:
		VSFile(NotNullPtr<Text::String> fileName);
		VSFile(Text::CString fileName);
		virtual ~VSFile();

		virtual NotNullPtr<Text::String> GetFileName() const;
	};

	class VSProjContainer
	{
	public:
		virtual void AddChild(Text::CodeObject *obj) = 0;
	};

	class VSContainer : public Text::CodeContainer, public VSProjContainer
	{
	private:
		NotNullPtr<Text::String> contName;
		Data::ArrayList<CodeObject*> childList;
	public:
		VSContainer(NotNullPtr<Text::String> contName);
		VSContainer(Text::CString contName);
		virtual ~VSContainer();

		virtual void SetContainerName(Text::CString contName);
		virtual NotNullPtr<Text::String> GetContainerName() const;

		virtual UOSInt GetChildCount() const;
		virtual CodeObject *GetChildObj(UOSInt index) const;

		virtual void AddChild(Text::CodeObject *obj);
	};

	class VSProject : public Text::CodeProject, public VSProjContainer 
	{
	public:
		typedef enum
		{
			VSV_UNKNOWN,
			VSV_VS6,
			VSV_VS71,
			VSV_VS8,
			VSV_VS9,
			VSV_VS10,
			VSV_VS11,
			VSV_VS12
		} VisualStudioVersion;
	private:
		VisualStudioVersion ver;
		NotNullPtr<Text::String> projName;
		Data::ArrayList<CodeObject*> childList;
	public:
		VSProject(Text::CString name, VisualStudioVersion ver);
		virtual ~VSProject();

		virtual ProjectType GetProjectType() const;
		virtual void SetProjectName(Text::String *projName);
		virtual void SetProjectName(Text::CString projName);
		virtual NotNullPtr<Text::String> GetContainerName() const;

		virtual UOSInt GetChildCount() const;
		virtual CodeObject *GetChildObj(UOSInt index) const;

		virtual void AddChild(Text::CodeObject *obj);
		VisualStudioVersion GetVSVersion() const;
	};
}
#endif
