#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/YAMLParser.h"
#include "IO/Java/JavaAnnotation.h"
#include "IO/Java/JavaArrayValue.h"
#include "IO/Java/JavaEnumValue.h"
#include "IO/Java/JavaMethod.h"
#include "IO/Java/JavaStringValue.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/ClassParser.h"
#include "SSWR/AVIRead/AVIRJavaSpringForm.h"
#include "UI/GUIFileDialog.h"

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
	NN<SSWR::AVIRead::AVIRJavaSpringForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaSpringForm>();
	if (me->reqList.GetCount() > 0)
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"JavaSpring", true);
		dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			UnsafeArrayOpt<const UTF8Char> csvArr[5];
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Path,Request Method,Return Type,Source File,Declaration Name\r\n"));
			UIntOS i = 0;
			UIntOS j = me->reqList.GetCount();
			while (i < j)
			{
				NN<ControllerRequest> req = me->reqList.GetItemNoCheck(i);
				csvArr[0] = UnsafeArray<const UTF8Char>(req->path->v);
				csvArr[1] = Net::WebUtil::RequestMethodGetName(req->reqMethod).v;
				csvArr[2] = UnsafeArray<const UTF8Char>(req->returnType->v);
				csvArr[3] = UnsafeArray<const UTF8Char>(req->sourceFile->v);
				csvArr[4] = UnsafeArray<const UTF8Char>(req->declName->v);
				sb.AppendCSV(csvArr, 5);
				sb.AppendC(UTF8STRC("\r\n"));
				i++;
			}
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(sb.ToByteArray());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRJavaSpringForm::OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRJavaSpringForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaSpringForm>();
	if (files.GetCount() > 0)
	{
		if (me->LoadPath(files[0]->ToCString()))
		{
			me->DisplayClasses();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRJavaSpringForm::OnClassPackagesSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJavaSpringForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaSpringForm>();
	NN<Text::String> s;
	me->lbClasses->ClearItems();
	if (me->lbClassPackages->GetSelectedItemTextNew().SetTo(s))
	{
		NN<IO::PackageFile> pkg;
		Bool needRelease = false;
		pkg = me->vpf;
		if (s->leng != 0)
		{
			if (!pkg->GetItemPack(s->ToCString(), needRelease).SetTo(pkg))
			{
				s->Release();
				return;
			}
		}
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i = 0;
		UIntOS j = pkg->GetCount();
		while (i < j)
		{
			if (pkg->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData && pkg->GetItemName(sbuff, i).SetTo(sptr))
			{
				if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC(".class")))
				{
					sptr[-6] = 0;
					me->lbClasses->AddItem(CSTRP(sbuff, sptr - 6), 0);
				}
			}
			i++;
		}
		if (needRelease)
			pkg.Delete();
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRJavaSpringForm::OnClassesDoubleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJavaSpringForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaSpringForm>();
	NN<Text::String> className;
	NN<Text::String> packageName;
	if (me->lbClasses->GetSelectedItemTextNew().SetTo(className))
	{
		if (me->lbClassPackages->GetSelectedItemTextNew().SetTo(packageName))
		{
			NN<IO::StreamData> fd;
			Text::StringBuilderUTF8 sb;
			if (packageName->leng > 0)
			{
				sb.Append(packageName)->AppendUTF8Char('/');
			}
			sb.Append(className);
			sb.Append(CSTR(".class"));

			if (me->vpf.GetItemStmDataPathNew(sb.ToCString()).SetTo(fd))
			{
				Parser::FileParser::ClassParser parser;
				NN<IO::ParsedObject> pobj;
				if (parser.ParseFile(fd, nullptr, IO::ParserType::JavaClass).SetTo(pobj))
				{
					me->core->OpenObject(pobj);
				}
				fd.Delete();
			}
			packageName->Release();
		}
		className->Release();
	}
}

Optional<IO::ConfigFile> SSWR::AVIRead::AVIRJavaSpringForm::OpenManifest(NN<IO::PackageFile> pkg)
{
	NN<IO::StreamData> fd;
	if (pkg->GetItemStmDataPathNew(CSTR("META-INF/MANIFEST.MF")).SetTo(fd))
	{
		IO::StreamDataStream stm(fd);
		Optional<IO::ConfigFile> cfg = IO::YAMLParser::ParseStream(stm);
		fd.Delete();
		return cfg;
	}
	return nullptr;
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadJar(Text::CStringNN fileName, NN<IO::StreamData> fd)
{
	UIntOS index = this->lvLib->AddItem(fileName, 0);
	NN<IO::PackageFile> pkg;
	if (Optional<IO::PackageFile>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::PackageFile)).SetTo(pkg))
	{
		NN<IO::ConfigFile> cfg;
		if (OpenManifest(pkg).SetTo(cfg))
		{
			NN<Text::String> s;
			if (cfg->GetValue(CSTR("Implementation-Title")).SetTo(s))
			{
				this->lvLib->SetSubItem(index, 1, s);
			}
			if (cfg->GetValue(CSTR("Implementation-Version")).SetTo(s))
			{
				this->lvLib->SetSubItem(index, 2, s);
			}
			NN<IO::PackageFile> classPkg;
			Bool needRelease = false;
			if (cfg->GetValue(CSTR("Spring-Boot-Classes")).SetTo(s))
			{
				if (pkg->GetItemPack(s->ToCString(), needRelease).SetTo(classPkg))
				{
					this->vpf.MergePackage(classPkg);
					if (needRelease)
					{
						classPkg.Delete();
					}
				}
				else
				{
					this->vpf.MergePackage(pkg);
				}
			}
			else
			{
				this->vpf.MergePackage(pkg);
			}
			cfg.Delete();
		}
		else
		{
			this->vpf.MergePackage(pkg);
		}
		pkg.Delete();
		return true;
	}
	return false;
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadPath(Text::CStringNN path)
{
	IO::Path::PathType pt = IO::Path::GetPathType(path);
	if (pt == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage pkg(path);
		return this->LoadClasses(pkg);
	}
	else if (pt == IO::Path::PathType::File)
	{
		if (path.EndsWithICase(UTF8STRC(".jar")) || path.EndsWithICase(UTF8STRC(".zip")))
		{
			IO::StmData::FileData fd(path, false);
			NN<IO::PackageFile> pkg;
			if (Optional<IO::PackageFile>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::PackageFile)).SetTo(pkg))
			{
				Bool succ = this->LoadClasses(pkg);
				pkg.Delete();
				return succ;
			}
		}
	}
	return false;
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadClasses(NN<IO::PackageFile> pkg)
{
	this->reqList.FreeAll(FreeControllerRequest);
	this->lvController->ClearItems();
	this->lvLib->ClearItems();
	this->lbClassPackages->ClearItems();
	this->lbClasses->ClearItems();
	this->vpf.ClearItems();
	this->txtPath->SetText(pkg->GetSourceNameObj()->ToCString());
	NN<IO::ConfigFile> cfg;
	if (OpenManifest(pkg).SetTo(cfg))
	{
		NN<Text::String> s;
		NN<Text::String> mainClass;
		if (cfg->GetValue(CSTR("Main-Class")).SetTo(mainClass))
		{
			if (mainClass->Equals(CSTR("org.springframework.boot.loader.JarLauncher")))
			{
				cfg->GetValue(CSTR("Start-Class")).SetTo(mainClass);

				if (cfg->GetValue(CSTR("Spring-Boot-Classes")).SetTo(s))
				{
					NN<IO::PackageFile> classPkg;
					Bool needRelease;
					if (pkg->GetItemPack(s->ToCString(), needRelease).SetTo(classPkg))
					{
						this->vpf.MergePackage(classPkg);
						if (needRelease)
						{
							classPkg.Delete();
						}
					}
					else
					{
						printf("AVIRJavaSpringForm: Classes path not found: %s\r\n", s->v.Ptr());
					}
				}
				if (cfg->GetValue(CSTR("Spring-Boot-Lib")).SetTo(s))
				{
					NN<IO::PackageFile> libPkg;
					Bool needRelease;
					if (pkg->GetItemPack(s->ToCString(), needRelease).SetTo(libPkg))
					{
						UTF8Char sbuff[512];
						UnsafeArray<UTF8Char> sptr;
						UIntOS i = 0;
						UIntOS j = libPkg->GetCount();
						while (i < j)
						{
							if (libPkg->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData && libPkg->GetItemName(sbuff, i).SetTo(sptr))
							{
								if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC(".jar")))
								{
									NN<IO::StreamData> fd;
									if (libPkg->GetItemStmDataNew(i).SetTo(fd))
									{
										this->LoadJar(CSTRP(sbuff, sptr), fd);
										fd.Delete();
									}
								}
							}
							i++;
						}
						if (needRelease)
						{
							libPkg.Delete();
						}
					}
					else
					{
						printf("AVIRJavaSpringForm: Lib path not found: %s\r\n", s->v.Ptr());
					}
				}
			}
			this->txtMainClass->SetText(mainClass->ToCString());
		}
		else
		{
			this->txtMainClass->SetText(CSTR(""));
		}
		cfg.Delete();
	}
	else
	{
		this->txtMainClass->SetText(CSTR(""));
	}
	return this->LoadClassesInner(pkg);
}

Bool SSWR::AVIRead::AVIRJavaSpringForm::LoadClassesInner(NN<IO::PackageFile> pkg)
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
				this->LoadClassesInner(subPkg);
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
					else if (typeStr->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/Controller;")))
					{
						isRestController = true;
					}
					else if (typeStr->Equals(UTF8STRC("Lorg/springframework/stereotype/Controller;")))
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
										else if (anno->GetAnnoType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/RequestMapping;")))
										{
											NN<IO::Java::JavaElementValue> value;
											if (anno->GetValue(CSTR("method")).SetTo(value))
											{
												if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::Array)
												{
													NN<IO::Java::JavaArrayValue>::ConvertFrom(value)->GetItem(0).SetTo(value);
												}
												if (value->GetElementType() == IO::Java::JavaElementValue::ElementType::Enum)
												{
													NN<IO::Java::JavaEnumValue> enumVal = NN<IO::Java::JavaEnumValue>::ConvertFrom(value);
													if (enumVal->GetType()->GetTypeStr()->Equals(UTF8STRC("Lorg/springframework/web/bind/annotation/RequestMethod;")))
													{
														NN<Text::String> enumName = enumVal->GetConstName();
														if (enumName->Equals(UTF8STRC("GET")))
														{
															reqMethod = Net::WebUtil::RequestMethod::HTTP_GET;
														}
														else if (enumName->Equals(UTF8STRC("POST")))
														{
															reqMethod = Net::WebUtil::RequestMethod::HTTP_POST;
														}
														else if (enumName->Equals(UTF8STRC("PUT")))
														{
															reqMethod = Net::WebUtil::RequestMethod::HTTP_PUT;
														}
														else if (enumName->Equals(UTF8STRC("DELETE")))
														{
															reqMethod = Net::WebUtil::RequestMethod::HTTP_DELETE;
														}
														else if (enumName->Equals(UTF8STRC("PATCH")))
														{
															reqMethod = Net::WebUtil::RequestMethod::HTTP_PATCH;
														}
													}
												}
											}
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

void SSWR::AVIRead::AVIRJavaSpringForm::DisplayClasses()
{
	this->lbClassPackages->ClearItems();
	this->lbClasses->ClearItems();
	UTF8Char sbuff[512];
	this->DisplayClassesInner(sbuff, sbuff, this->vpf);
}

void SSWR::AVIRead::AVIRJavaSpringForm::DisplayClassesInner(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UTF8Char> sbuffEnd, NN<IO::PackageFile> pf)
{
	sbuffEnd[0] = 0;
	UIntOS i;
	UIntOS j;
	Bool found = false;
	i = 0;
	j = pf->GetCount();
	while (i < j)
	{
		IO::PackageFile::PackObjectType pot = pf->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			if (!found)
			{
				UnsafeArray<UTF8Char> sptr;
				sbuffEnd[1] = 0;
				if (pf->GetItemName(&sbuffEnd[1], i).SetTo(sptr) && Text::StrEndsWithC(&sbuffEnd[1], (UIntOS)(sptr - &sbuffEnd[1]), UTF8STRC(".class")))
				{
					found = true;
					sbuffEnd[0] = 0;
					if (sbuffEnd == sbuff)
					{
						this->lbClassPackages->AddItem(CSTRP(sbuff, sbuffEnd), 0);
					}
					else
					{
						this->lbClassPackages->AddItem(CSTRP(sbuff + 1, sbuffEnd), 0);
					}
				}
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			NN<IO::PackageFile> npf;
			Bool needRelease = false;
			if (pf->GetItemPack(i, needRelease).SetTo(npf))
			{
				sbuffEnd[0] = '/';
				this->DisplayClassesInner(sbuff, pf->GetItemName(&sbuffEnd[1], i).Or(&sbuffEnd[1]), npf);
				if (needRelease)
				{
					npf.Delete();
				}
			}
		}
		i++;
	}
}

SSWR::AVIRead::AVIRJavaSpringForm::AVIRJavaSpringForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), vpf(CSTR("Classes"))
{
	this->SetFont(nullptr, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Java Spring"));

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 55, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPath = ui->NewLabel(this->pnlMain, CSTR("Path"));
	this->lblPath->SetRect(4, 4, 100, 23, false);
	this->txtPath = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtPath->SetRect(104, 4, 600, 23, false);
	this->txtPath->SetReadOnly(true);
	this->lblMainClass = ui->NewLabel(this->pnlMain, CSTR("Main Class"));
	this->lblMainClass->SetRect(4, 32, 100, 23, false);
	this->txtMainClass = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtMainClass->SetRect(104, 32, 600, 23, false);
	this->txtMainClass->SetReadOnly(true);
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

	this->tpLib = this->tcMain->AddTabPage(CSTR("Library"));
	this->lvLib = ui->NewListView(this->tpLib, UI::ListViewStyle::Table, 3);
	this->lvLib->SetFullRowSelect(true);
	this->lvLib->SetShowGrid(true);
	this->lvLib->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLib->AddColumn(CSTR("FileName"), 200);
	this->lvLib->AddColumn(CSTR("Title"), 200);
	this->lvLib->AddColumn(CSTR("Version"), 100);

	this->tpClasses = this->tcMain->AddTabPage(CSTR("Classes"));
	this->lbClassPackages = ui->NewListBox(this->tpClasses, false);
	this->lbClassPackages->SetRect(0, 0, 200, 100, false);
	this->lbClassPackages->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbClassPackages->HandleSelectionChange(OnClassPackagesSelChg, this);
	this->lbClasses = ui->NewListBox(this->tpClasses, false);
	this->lbClasses->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbClasses->HandleDoubleClicked(OnClassesDoubleClicked, this);

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
