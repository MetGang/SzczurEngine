#include "SoundEditor.hpp"

#include <cmath>
#include <fstream>

#include <experimental/filesystem>
#include "Szczur/Modules/FileSystem/FileDialog.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#include <nlohmann/json.hpp>

namespace rat
{
    SoundEditor::SoundEditor(Sound& sound)
        : _sound(sound), _assets(sound.getAssetsManager())
    {
        _currentEditing = _soundHolder.end();
    }

    void SoundEditor::render()
    { 
		if (_loadingSound)
			load();

		ImGui::Begin("Sounds List");

		float width = ImGui::GetWindowContentRegionWidth() * 0.33;

		if (ImGui::Button("Save##SoundLists", { width,0 })) {
			for (auto it = _soundHolder.begin(); it != _soundHolder.end(); ++it) {
				save(it);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Load##SoundLists", { width,0 })) {
			_loadingSound = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Add##SoundLists", { width ,0 })) {
			add();
		}

		if (ImGui::CollapsingHeader("Sounds list"))
		{
			for (auto it = _soundHolder.begin(); it != _soundHolder.end(); ++it) {
				std::string name = it->getName();

				ImGui::Separator();

				ImGui::Bullet();

				ImGui::SameLine();
				ImGui::Text(name.c_str());
				
				ImGui::SameLine();
				if (ImGui::SmallButton("Open editor")) {
					if (_currentEditing != _soundHolder.end() && _currentEditing->getName() != name)
						_sound.stop();
					_currentEditing = it;
				}
				
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV( 2.f / 7.f, 0.6f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2.f / 7.f, 0.7f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2.f / 7.f, 0.8f, 0.7f));
				if (ImGui::SmallButton("Save")) {
					save(it);
				}
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.7f));
				if (ImGui::SmallButton("Delete")) {
					_soundHolder.erase(it);
					if (_currentEditing->getName() == name)
						_currentEditing = _soundHolder.end();
					break;
				}
				ImGui::PopStyleColor(3);
			}
		}

		ImGui::End();

        if (_currentEditing != _soundHolder.end()) {
            ImGui::Begin("Sound Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

                ImGui::Text(("Path: " + _currentEditing->getFileName()).c_str());

                ImGui::Separator();

                auto currName = _currentEditing->getName();

                ImGui::Text("Name: "); 
                ImGui::SameLine();

                size_t size = currName.length() + 100;
                char *newText = new char[size] {};
                strncpy(newText, currName.c_str(), size);

                ImGui::PushItemWidth(300);
                    if (ImGui::InputText("##SoundNameInput", newText, size)) {
                        currName = newText;
                        _currentEditing->setName(currName);
                    }
                ImGui::PopItemWidth();

                delete[] newText;

                ImGui::Separator();

                bool relative = _currentEditing->isRelativeToListener();
                ImGui::Checkbox("Relative To Listener", &relative);
                _currentEditing->setRelativeToListener(relative);

                ImGui::Separator();

                if (ImGui::Button("PLAY##SoundEditor")) {
                    _currentEditing->play();
                }
                ImGui::SameLine();
                if (ImGui::Button("PAUSE##SoundEditor")) {
                    _sound.pause();
                }
                
                ImGui::SameLine();
                if (ImGui::Button("STOP##SoundEditor")) {
                    _sound.stop();
                }   

                ImGui::Separator();

                char beginTime[6] = "00:00"; 
                char endTime[6] = "00:00";

                strncpy(beginTime, toTime(_currentEditing->getBeginTime()).c_str(), 6);
                strncpy(endTime, toTime(_currentEditing->getEndTime()).c_str(), 6);

                ImGui::PushItemWidth(50); 
                if (ImGui::InputText("##SongBeginTimeSelector", beginTime, 6, 1)) {
                    std::string timeString = beginTime;
                    checkTimeString(timeString);
                    _currentEditing->setOffset(toFloatSeconds(timeString), _currentEditing->getEndTime());
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                ImGui::Text("-");
                ImGui::SameLine();
                ImGui::PushItemWidth(50);
                if (ImGui::InputText("##SongEndTimeSelector", endTime, 6, 1)) {
                    std::string timeString = endTime;
                    checkTimeString(timeString);
                    _currentEditing->setOffset(_currentEditing->getBeginTime(), toFloatSeconds(timeString));
                }
                ImGui::PopItemWidth();
                
                ImGui::SameLine();
                ImGui::Text("Offset");

                ImGui::Separator();

                float volume = _currentEditing->getVolume();
                if (ImGui::SliderFloat("Volume##SoundEditor", &volume, 0, 100)) {
                    _currentEditing->setVolume(volume);
                }

                ImGui::Separator();

                float pitch = _currentEditing->getPitch();
                if (ImGui::InputFloat("Pitch##SoundEditor", &pitch, 0.0f, 0.0f, 2)) {
                    _currentEditing->setPitch(pitch);
                }

                ImGui::Separator();

                float attenuation = _currentEditing->getAttenuation();
                if (ImGui::SliderFloat("Attenuation##SoundEditor", &attenuation, 0, 100)) {
                    _currentEditing->setAttenuation(attenuation);
                }

                ImGui::Separator();

                float minDistance = _currentEditing->getMinDistance();
                if (ImGui::InputFloat("Minimum Distance##SoundEditor", &minDistance, 0.0f, 0.0f, 2)) {
                    if(minDistance > 0) {
                        _currentEditing->setMinDistance(minDistance);
                    }
                }

                ImGui::Separator();


            ImGui::End();
        }
    }

    void SoundEditor::save(Container_t::iterator it)
    {
        auto name = it->getName();

        nlohmann::json j;

        std::ifstream ifile(SOUND_DATA_FILE_PATH);
        if (ifile.is_open()) {
            ifile >> j;
            ifile.close();
        }

        std::ofstream ofile(SOUND_DATA_FILE_PATH, std::ios::trunc);
        if (ofile.is_open()) {
            j[name]["Path"] = it->getFileName();
            j[name]["BeginTime"] = it->getBeginTime();
            j[name]["EndTime"] = it->getEndTime();
            j[name]["Volume"] = it->getVolume();
            j[name]["Pitch"] = it->getPitch();
            j[name]["Attenuation"] = it->getAttenuation();
            j[name]["MinDistance"] = it->getMinDistance();
            j[name]["Relative"] = it->isRelativeToListener();

            ofile << std::setw(4) << j << std::endl;
            ofile.close();
        }
    }

    void SoundEditor::load()
    {
        static std::string loadingSoundName = "";

		if (!ImGui::Begin("Load Sound", &_loadingSound))
		{
			ImGui::End();
		}
		else {
			ImGui::Text("Name: ");
			ImGui::SameLine();

			size_t size = loadingSoundName.length() + 100;
			char *newText = new char[size] {};
			strncpy(newText, loadingSoundName.c_str(), size);

			ImGui::PushItemWidth(300);
			if (ImGui::InputText("##LoadingSoundNameInput", newText, size)) {
				loadingSoundName = newText;
			}
			if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
				ImGui::SetKeyboardFocusHere(0);
			}
			ImGui::PopItemWidth();

			delete[] newText;

			ImGui::SetCursorPosX(260);

			if (ImGui::Button("CANCEL##LoadSound")) {
				loadingSoundName = "";
				_loadingSound = false;
			}

			ImGui::SameLine();

			if (ImGui::Button(" OK ##LoadSound")) {

				_soundHolder.push_back(SoundBase(loadingSoundName));
				_soundHolder.back().load();

				auto fileName = _soundHolder.back().getFileName();

				if (fileName.empty()) {
					_soundHolder.pop_back();
				}
				else {
					_assets.load(fileName);
					_soundHolder.back().setBuffer(_assets.get(fileName));
					_soundHolder.back().init();
				}


				loadingSoundName = "";
				_loadingSound = false;
			}

			ImGui::End();
		}
    
    }

    void SoundEditor::add()
    {
        auto currentPath = std::experimental::filesystem::current_path().string();
        auto path = FileDialog::getOpenFileName("", currentPath, "Sound files (*.flac)|*.flac");
        std::string filePath;
        
        if (path.find(currentPath) != std::string::npos && !path.empty()) {
            filePath = path.substr(currentPath.length() + 1, path.length());
            std::replace(filePath.begin(), filePath.end(), '\\', '/');

            _soundHolder.push_back(SoundBase("Unnnamed"));
            _assets.load(filePath);
            _soundHolder.back().setBuffer(_assets.get(filePath));
            _soundHolder.back().init();
        }
    }

    std::string SoundEditor::toTime(float secF)
    {
        int minI = std::floor(secF);
        int secI = (secF - minI) * 100;

        auto minS = std::to_string(minI);
        auto secS = std::to_string(secI);

        return (minI >= 10 ? minS : "0" + minS) + ":" + (secI >= 10 ? secS : "0" + secS);
    }

    float SoundEditor::toFloatSeconds(const std::string& timeString) const
    {
        return atoi(&timeString.at(0)) + (float(atoi(&timeString.at(3))) / 100);
    }

    void SoundEditor::checkTimeString(std::string& timeString)
    {
        auto semicolonPos = timeString.find(':');
        if (semicolonPos == std::string::npos || semicolonPos != 2 || timeString.length() != 5) {
            timeString = "00:00";
        } 
        else if (timeString[3] >= 54) {
            timeString[3] -= 6;
            timeString[1] += 1;
        }
    }

}