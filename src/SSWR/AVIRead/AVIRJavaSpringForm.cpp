#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/Java/JavaAnnotation.h"
#include "IO/Java/JavaArrayValue.h"
#include "IO/Java/JavaMethod.h"
#include "IO/Java/JavaStringValue.h"
#include "SSWR/AVIRead/AVIRJavaSpringForm.h"

void __stdcall SSWR::AVIRead::AVIRJavaSpringForm::FreeControllerRequest(NN<ControllerRequest> req)
{
	req->declName->Release();
	req->sourceFile->Release();
	req->returnType->Release();
	req->path->Release();
	MemFreeNN(req);
}

void __stdcall SSWR::AVIRead::AVIRJavaSpringForm::OnControllerSaveClicked(AnyType userObj)
{

}

void __stdcall SSWR::AVIRead::AVIRJavaSpringForm::OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRJavaSpringForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaSpringForm>();
	if (files.GetCount() > 0)
	{
		me->LoadPath(files[0]->ToCString());
	}
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadPath(Text::CStringNN path)
{
	IO::DirectoryPackage pkg(path);
	return this->LoadPackage(pkg);
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadPackage(NN<IO::PackageFile> pkg)
{
	this->reqList.FreeAll(FreeControllerRequest);
	this->lvController->ClearItems();
	return this->LoadPackageInner(pkg);
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadPackageInner(NN<IO::PackageFile> pkg)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::PackageFile::PackObjectType pot;
	UIntOS i = 0;
	UIntOS j = pkg->GetCount();
	while (i < j)
	{
		pot = pkg->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			if (pkg->GetItemName(sbuff, i).SetTo(sptr))
			{
				if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC(".class")))
				{
					NN<IO::StreamData> fd;
					if (pkg->GetItemStmDataNew(i).SetTo(fd))
					{
						this->LoadFile(fd);
						fd.Delete();
					}
				}
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			Bool needRelease;
			NN<IO::PackageFile> subPkg;
			if (pkg->GetItemPack(i, needRelease).SetTo(subPkg))
			{
				this->LoadPackageInner(subPkg);
				if (needRelease)
				{
					subPkg.Delete();
				}
			}
		}
		i++;
	}
	return true;
}

void SSWR::AVIRead::AVIRJavaSpringForm::LoadFile(NN<IO::StreamData> fd)
{
	UInt64 dsize = fd->GetDataSize();
	if (dsize < 26 || dsize > 1048576)
	{
		return;
	}
	Data::ByteBuffer buff((UIntOS)dsize);
	NN<IO::Java::JavaClass> cls;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	if (fd->GetRealData(0, (UIntOS)dsize, buff) == dsize)
	{
		if (IO::Java::JavaClass::ParseBuff(fd->GetFullFileName(), buff).SetTo(cls))
		{
			UnsafeArray<UInt8> annoPtr;
			UInt32 annoLen;
			if (cls->GetAnnotations(annoLen).SetTo(annoPtr))
			{
				Data::ArrayListNN<IO::Java::JavaAnnotation> annoList;
				IO::Java::JavaElementValue::ParseAnnotations(annoList, cls, annoPtr, annoPtr + annoLen);
				NN<IO::Java::JavaAnnotation> anno;
				Bool isRestController = false;
				Optional<Text::String> mapPath = nullptr;
				i = 0;
				j = annoList.GetCount();
				while (i < j)
				{
					anno = annoList.GetItemNoCheck(i);
					NN<Text::String> typeStr = anno->GetAnnoType()->GetTypeStr();
					if (typeStr->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/RestController;")))
					{
						isRestController = true;
					}
					else if (typeStr->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/RequestMapping;")))
					{
						NN<IO::Java::JavaElementValue> value;
						if (anno->GetValue(CSTR("value")).SetTo(value))
						{
							if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::Array)
							{
								NN<IO::Java::JavaArrayValue> arr = NN<IO::Java::JavaArrayValue>::ConvertFrom(value);
								if (arr->GetItem(0).SetTo(value))
								{
									if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::String)
									{
										OPTSTR_DEL(mapPath);
										mapPath = NN<IO::Java::JavaStringValue>::ConvertFrom(value)->GetString()->Clone();
									}
								}
							}
							else if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::String)
							{
								OPTSTR_DEL(mapPath);
								mapPath = NN<IO::Java::JavaStringValue>::ConvertFrom(value)->GetString()->Clone();
							}
						}
					}
					i++;
				}
				annoList.DeleteAll();
				if (isRestController)
				{
					Text::StringBuilderUTF8 sb;
					Data::ArrayListStringNN importList;
					i = 0;
					j = cls->GetMethodCount();
					while (i < j)
					{
						UnsafeArray<UInt8> methodPtr;
						NN<IO::Java::JavaMethod> method;
						NN<IO::Java::JavaAnnotation> anno;
						if (cls->GetMethod(i).SetTo(methodPtr))
						{
							if (IO::Java::JavaMethod::ParseMethod(cls, methodPtr).SetTo(method))
							{
								k = method->GetAnnotationCount();
								while (k-- > 0)
								{
									if (method->GetAnnotation(k).SetTo(anno))
									{
										Net::WebUtil::RequestMethod reqMethod = Net::WebUtil::RequestMethod::Unknown;
										if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/GetMapping;")))
										{
											reqMethod = Net::WebUtil::RequestMethod::HTTP_GET;
										}
										else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/PostMapping;")))
										{
											reqMethod = Net::WebUtil::RequestMethod::HTTP_POST;
										}
										else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/PutMapping;")))
										{
											reqMethod = Net::WebUtil::RequestMethod::HTTP_PUT;
										}
										else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/DeleteMapping;")))
										{
											reqMethod = Net::WebUtil::RequestMethod::HTTP_DELETE;
										}
										else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/PatchMapping;")))
										{
											reqMethod = Net::WebUtil::RequestMethod::HTTP_PATCH;
										}
										if (reqMethod != Net::WebUtil::RequestMethod::Unknown)
										{
											NN<IO::Java::JavaElementValue> value;
											if (anno->GetValue(CSTR("value")).SetTo(value))
											{
												if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::Array)
												{
													NN<IO::Java::JavaArrayValue>::ConvertFrom(value)->GetItem(0).SetTo(value);
												}

												if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::String)
												{
													NN<ControllerRequest> req = MemAllocNN(ControllerRequest);
													sb.ClearStr();
													sb.AppendOpt(mapPath);
													sb.Append(NN<IO::Java::JavaStringValue>::ConvertFrom(value)->GetString());
													req->path = Text::String::New(sb.ToCString());
													req->reqMethod = reqMethod;
													sb.ClearStr();
													method->GetReturnType()->ToString(sb, importList, nullptr);
													req->returnType = Text::String::New(sb.ToCString());
													sb.ClearStr();
													cls->GetClassNameFull(sb);
													k = sb.IndexOf('<');
													if (k != INVALID_INDEX)
													{
														sb.TrimToLength(k);
													}
													k = sb.LastIndexOf('.');
													if (k != INVALID_INDEX)
													{
														sb.SetSubstr(k + 1);
													}
													sb.Append(CSTR(".java"));
													req->sourceFile = Text::String::New(sb.ToCString());
													sb.ClearStr();
													method->ToDeclarationNameParams(sb, importList, nullptr);
													req->declName = Text::String::New(sb.ToCString());

													k = this->lvController->AddItem(req->path, req.Ptr());
													this->lvController->SetSubItem(k, 1, Net::WebUtil::RequestMethodGetName(reqMethod));
													this->lvController->SetSubItem(k, 2, req->returnType);
													this->lvController->SetSubItem(k, 3, req->sourceFile);
													this->lvController->SetSubItem(k, 4, req->declName);
													this->reqList.Add(req);
												}
											}
											break;
										}
									}
								}
								method.Delete();
							}
						}
						i++;
					}
					importList.FreeAll();
				}
				OPTSTR_DEL(mapPath);
			}
			cls.Delete();
		}
	}
}

SSWR::AVIRead::AVIRJavaSpringForm::AVIRJavaSpringForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Java Spring"));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpController = this->tcMain->AddTabPage(CSTR("Controller"));
	this->pnlController = ui->NewPanel(this->tpController);
	this->pnlController->SetRect(0, 0, 100, 31, false);
	this->pnlController->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnControllerSave = ui->NewButton(this->pnlController, CSTR("Save"));
	this->btnControllerSave->SetRect(4, 4, 75, 23, false);
	this->btnControllerSave->HandleButtonClick(OnControllerSaveClicked, this);
	this->lvController = ui->NewListView(this->tpController, UI::ListViewStyle::Table, 5);
	this->lvController->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvController->AddColumn(CSTR("Path"), 200);
	this->lvController->AddColumn(CSTR("Request Method"), 100);
	this->lvController->AddColumn(CSTR("Return Type"), 100);
	this->lvController->AddColumn(CSTR("Source File"), 200);
	this->lvController->AddColumn(CSTR("Declaration Name"), 200);

	this->HandleDropFiles(OnDirectoryDrop, this);
}

SSWR::AVIRead::AVIRJavaSpringForm::~AVIRJavaSpringForm()
{
	this->reqList.FreeAll(FreeControllerRequest);
}

void SSWR::AVIRead::AVIRJavaSpringForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
