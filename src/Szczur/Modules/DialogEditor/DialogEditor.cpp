#include "DialogEditor.hpp"

#include <experimental/filesystem>

#include "Szczur/Modules/FileSystem/DirectoryDialog.hpp"

#include "Szczur/Utility/MsgBox.hpp"

namespace fs = std::experimental::filesystem;

namespace rat
{

DialogEditor::DialogEditor()
	: _characters(_CharactersManager.getCharactersContainer()),
	  _dlgEditor(_characters), _nodeEditor(this), _CharactersManager(_dlgEditor.getContainer())
{
	LOG_INFO("Initializing DialogEditor module");
	refreshDialogsList();
	LOG_INFO("Module DialogEditor initialized");
}

DialogEditor::~DialogEditor()
{
	LOG_INFO("Module DialogEditor destructed");
}

void DialogEditor::update()
{
	if (_projectLoaded)
	{
		if (_showCharactersManager)
			_CharactersManager.update();

		if (_showDlgEditor)
			_dlgEditor.update();

		if (_showNodeEditor)
			_nodeEditor.update();
	}

	if (_projectLoaded)
	{
		if (ImGui::Begin("Dialog Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Path: ./%s", _projectPath.c_str());

			ImGui::Separator();

			if (ImGui::Button("Test dialog"))
			{
				getModule<Dialog>().unload();

				_dlgEditor.save();
				_nodeEditor.save(_projectPath + "/dialog.json", NodeEditor::Json);
				_nodeEditor.save(_projectPath + "/dialog.lua", NodeEditor::Lua);
				_showCharactersManager = false;
				_showDlgEditor = false;
				_showNodeEditor = false;

				getModule<Script>().scriptFile(_projectPath + "/dialog.lua");
			}

			ImGui::SameLine();

			if (ImGui::Button("Stop dialog"))
			{
				getModule<Dialog>().unload();
			}

			if (ImGui::Button("Show in explorer"))
			{
#ifdef OS_WINDOWS
				auto path = fs::current_path().string() + "\\" + _projectPath;

				ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
			}

			ImGui::Separator();

			if (ImGui::Button("Save"))
			{
				if (!_projectPath.empty())
				{
					_CharactersManager.save(_projectPath + "/characters.json");
					_dlgEditor.save();
					_nodeEditor.save(_projectPath + "/dialog.json", NodeEditor::Json);
				}
			}

			if (ImGui::Button("Generate"))
			{
				LOG_INFO("Generating lua...");

				if (!_projectPath.empty())
					_nodeEditor.save(_projectPath + "/dialog.lua", NodeEditor::FileFormat::Lua);
			}

			ImGui::Separator();

			ImGui::Checkbox("Characters Manager", &_showCharactersManager);
			ImGui::Checkbox("Dlg Editor", &_showDlgEditor);
			ImGui::Checkbox("Node Editor", &_showNodeEditor);
		}
		ImGui::End();
	}

	if (ImGui::Begin("Dialogs' Directory Browser", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Dialogs");

		if (ImGui::Button("Refresh list"))
		{
			refreshDialogsList();
		}

		ImGui::SameLine();

		if (ImGui::Button("Show in explorer"))
		{
			auto path = fs::current_path().string() + "\\" + _dialogsDirectory.Path;

			ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
		}

		ImGui::Separator();

		showDirectory(_dialogsDirectory);
	}

	ImGui::End();

	if (_showDirectoryPopup)
	{
		ImGui::OpenPopup("Directory Popup");
		_showDirectoryPopup = false;
	}

	if (ImGui::BeginPopup("Directory Popup"))
	{
		ImGui::Selectable("Create directory");
		ImGui::Selectable("Create project");

		ImGui::EndPopup();
	}
}

void DialogEditor::createProject(const std::string& path)
{
	_projectPath = fixPathSlashes(path);

	_dlgEditor.load(_projectPath);
	_nodeEditor.createNew();
	_nodeEditor.save(_projectPath + "/dialog.json", NodeEditor::Json);
	_CharactersManager.clear();
	_CharactersManager.save(_projectPath + "/characters.json");

	fs::copy("dialog/dialog.flac", path + "/dialog.flac");

	_projectLoaded = true;
}

void DialogEditor::openProject(const std::string& path)
{
	if (path.empty())
		return;

	auto _path = fixPathSlashes(path);

	auto currentPath = fs::current_path().string();

	LOG_INFO("Opening `", _path, "`...");

	if (isProjectDirectory(_path))
	{
		_projectPath = _path;

		_CharactersManager.load(_projectPath + "/characters.json");
		_dlgEditor.load(_projectPath);
		_nodeEditor.load(_projectPath + "/dialog.json", NodeEditor::Json);
		_nodeEditor.setTextContainer(&_dlgEditor.getContainer());

		_projectLoaded = true;
	}
	else
	{
		LOG_INFO("Missing files!");
		MsgBox::show("Cannot open project because cannot find dialog.dlg, dialog.json or characters.json", "Missing files", MsgBox::Icon::Warning);
	}
}

void DialogEditor::showDirectory(Directory& directory)
{
	for (auto& child : directory.Childs)
	{
		if (child.Type == Directory::DialogsDir)
		{
			if (ImGui::TreeNode(child.Name.c_str()))
			{
				ImGui::SameLine();

				if (ImGui::SmallButton(("+##" + child.Name).c_str()))
				{
					_showDirectoryPopup = true;
				}

				showDirectory(child);
				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::BulletText(child.Name.c_str());

			ImGui::SameLine();

			if (ImGui::SmallButton("Open"))
			{
				openProject(child.Path);
			}
		}
	}
}

void DialogEditor::refreshDialogsList()
{
	_dialogsDirectory.Type = Directory::DialogsDir;
	_dialogsDirectory.Name = "dialogs";
	_dialogsDirectory.Path = "dialogs";

	_dialogsDirectory.Childs.clear();

	scanFolder(_dialogsDirectory, "dialogs");
}

void DialogEditor::scanFolder(Directory& directory, const std::string& path)
{
	for (auto& p : fs::directory_iterator(path))
	{
		if (fs::is_directory(p.status()))
		{
			Directory newDir;

			newDir.Name = p.path().filename().string();
			newDir.Path = p.path().string();

			if (isProjectDirectory(p.path().string()))
			{
				newDir.Type = Directory::ProjectDir;
				//LOG_INFO("(", newDir.Path, "): ProjectDir");
			}
			else
			{
				newDir.Type = Directory::DialogsDir;
				//LOG_INFO("(", newDir.Path, "): DialogDir");

				scanFolder(newDir, newDir.Path);
			}

			directory.Childs.push_back(newDir);
		}
	}
}

bool DialogEditor::isProjectDirectory(const std::string& path)
{
	bool error = false;

	if (!fs::exists(path + "/dialog.dlg"))
		error = true;

	if (!fs::exists(path + "/dialog.json"))
		error = true;

	if (!fs::exists(path + "/characters.json"))
		error = true;

	return !error;
}

std::string DialogEditor::fixPathSlashes(const std::string& path)
{
	std::string result = path;

	std::replace(result.begin(), result.end(), '\\', '/');

	return result;
}

}
