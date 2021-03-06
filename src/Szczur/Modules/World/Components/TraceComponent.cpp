#include "TraceComponent.hpp"

#include "Szczur/Utility/ImGuiTweaks.hpp"
#include "Szczur/Modules/Script/Script.hpp"

#include "Trace/Trace.hpp"
#include "Trace/Timeline.hpp"
#include "Trace/Actions/AnimAction.hpp"
#include "Trace/Actions/MoveAction.hpp"
#include "Trace/Actions/WaitAction.hpp"
#include "Trace/Actions/ScriptAction.hpp"

#include "../ScenesManager.hpp"

namespace rat
{

TraceComponent::TraceComponent(Entity* parent)
  : Component { parent, fnv1a_64("TraceComponent"), "TraceComponent" }
{
	_trace = std::make_shared<Trace>(getEntity());
}

void TraceComponent::setTimeline(int id)
{
	auto& timelines = _trace->getTimelines();

	auto timeline = std::find_if(timelines.begin(), timelines.end(), [id] (auto& timeline) { return id == timeline->getId(); });

	if (*timeline)
		_trace->setCurrentTimeline((*timeline).get());
}

void TraceComponent::play()
{
	if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
	{
		tm->start();
	}
}

void TraceComponent::stop()
{
	if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
	{
		tm->setStatus(Timeline::Stopped);
	}
}

void TraceComponent::pause()
{
	if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
	{
		tm->setStatus(Timeline::Paused);
	}
}

void TraceComponent::resume()
{
	if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
	{
		tm->setStatus(Timeline::Playing);
	}
}

void TraceComponent::loadFromConfig(Json& config)
{
	Component::loadFromConfig(config);
	
	_trace->loadFromConfig(config);

	_currentTimeline = nullptr;
	_currentAction = nullptr;
}

void TraceComponent::saveToConfig(Json& config) const
{
	Component::saveToConfig(config);

	_trace->saveToConfig(config);
}

void TraceComponent::update(ScenesManager& scenes, float deltaTime)
{
	if (_trace)
	{
		_trace->update(deltaTime);
	}
}

void TraceComponent::render(sf3d::RenderTarget& target)
{
	_trace->draw(target, sf3d::RenderStates());
}

void TraceComponent::initScript(ScriptClass<Entity>& entity, Script& script)
{
	auto object = script.newClass<TraceComponent>("TraceComponent", "World");

	// Main
	object.set("play", &TraceComponent::play);
	object.set("stop", &TraceComponent::stop);
	object.set("pause", &TraceComponent::pause);
	object.set("resume", &TraceComponent::resume);
	object.set("setTimeline", &TraceComponent::setTimeline);
	object.set("getEntity", sol::resolve<Entity*()>(&Component::getEntity));

	// Entity
	entity.set("addTraceComponent", [&](Entity& e){return (TraceComponent*)e.addComponent<TraceComponent>();});
	entity.set("trace", &Entity::getComponentAs<TraceComponent>);

	object.init();
}

std::unique_ptr<Component> TraceComponent::copy(Entity* newParent) const
{
	auto ptr = std::make_unique<TraceComponent>(*this);

	ptr->setEntity(newParent);

	return ptr;
}

void TraceComponent::renderHeader(ScenesManager& scenes, Entity* object) 
{
	if (ImGui::CollapsingHeader("Trace##trance_component"))
	{
		ImGui::Text("Timeline: %d\n", _trace->getCurrentTimeline() ? _trace->getCurrentTimeline()->getId() : -1);

		ImGui::Separator();

		if (ImGui::Button("+"))
		{
			_trace->addTimeline();
		}

		ImGui::SameLine();

		std::string playStopButtonText = "Play";

		if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
		{
			if (tm->getStatus() == Timeline::Playing)
				playStopButtonText = "Stop";
		}

		if (ImGui::Button(playStopButtonText.c_str()))
		{
			if (_currentTimeline)
			{
				_trace->setCurrentTimeline(_currentTimeline);

				if (_currentTimeline->getStatus() == Timeline::Playing)
					stop();
				else
					play();
			}
			else
			{
				stop();
				_trace->setCurrentTimeline(nullptr);
			}
		}

		ImGui::SameLine();

		std::string pauseResumeButtonText = "Pause";

		if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
		{
			if (tm->getStatus() == Timeline::Paused)
				pauseResumeButtonText = "Resume";
		}

		if (ImGui::Button(pauseResumeButtonText.c_str()))
		{
			if (auto tm = _trace->getCurrentTimeline(); tm != nullptr)
			{
				if (tm->getStatus() == Timeline::Playing)
					pause();
				else if (tm->getStatus() == Timeline::Paused)
					resume();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("-"))
		{
			if (_currentTimeline)
			{
				_trace->removeTimeline(_currentTimeline);
				_currentTimeline = nullptr;
			}
		}

		auto& style = ImGui::GetStyle();

		// Show all timelines
		ImGui::BeginChild("Timelines", ImVec2(0.f, ImGui::GetFrameHeightWithSpacing() + 12.f), true);
		{
			for (auto& timeline : _trace->getTimelines())
			{
				ImGui::PushID(timeline.get());

				bool activeTimeline = timeline.get() == _currentTimeline;

				if (activeTimeline)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_HeaderHovered]);
					ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SeparatorHovered]);
				}

				if (ImGui::Button(std::to_string(timeline->getId()).c_str(), { ImGui::GetFrameHeight(), 0.f }))
				{
					if (_currentTimeline == timeline.get())
						_currentTimeline = nullptr;
					else
						_currentTimeline = timeline.get();

					_currentAction = nullptr;
				}

				if (activeTimeline)
				{
					ImGui::PopStyleColor(2);
				}

				ImGui::SameLine();

				ImGui::PopID();
			}
		}
		ImGui::EndChild();

		// Show current timeline
		if (_currentTimeline)
		{
			ImGui::Checkbox("Loop", &_currentTimeline->Loop);
			ImGui::Checkbox("Show lines in editor", &_currentTimeline->ShowLines);

			ImGui::PushItemWidth(100.f);
			ImGui::DragFloat<ImGui::CopyPaste>("Speed multiplier", _currentTimeline->SpeedMultiplier, 0.01f, 0.f, 100.f);
			ImGui::PopItemWidth();
			ImGui::Spacing();

			auto& actions = _currentTimeline->getActions();

			if (ImGui::Button("+Move"))
			{
				_currentTimeline->addAction(new MoveAction(object));
			}

			ImGui::SameLine();

			if (ImGui::Button("+Anim"))
			{
				if (object->hasComponent<ArmatureComponent>())
					_currentTimeline->addAction(new AnimAction(object));
			}

			ImGui::SameLine();

			if (ImGui::Button("+Wait"))
			{
				_currentTimeline->addAction(new WaitAction(object));
			}

			ImGui::SameLine();

			if (ImGui::Button("+Script"))
			{
				if (object->hasComponent<ScriptableComponent>())
					_currentTimeline->addAction(new ScriptAction(object));
			}
			
			ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.f);

			ImGui::BeginChild("Actions", ImVec2(0.f, ImGui::GetFrameHeightWithSpacing() + style.ScrollbarSize + 12.f), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				for (int aId = 0; aId < actions.size(); ++aId)
				{
					auto& action = actions[aId];

					std::string buttonName;

					switch (action->getType())
					{
						case Action::Move:
							buttonName = "M";
							break;
						case Action::Anim:
							buttonName = "A";
							break;
						case Action::Wait:
							buttonName = "W";
							break;
						case Action::Script:
							buttonName = "S";
							break;
					}

					ImGui::PushID(action.get());

					bool active = action.get() == _currentAction;

					if (active)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_HeaderHovered]);
						ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SeparatorHovered]);
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, action->ButtonColor);
					}

					if (ImGui::Button((buttonName).c_str(), { ImGui::GetFrameHeight(), 0.f }))
					{

						if (_currentAction)
							_currentAction->Color = glm::vec3(1.f, 1.f, 1.f);

						if (_currentAction == action.get())
							_currentAction = nullptr;
						else
						{
							_currentAction = action.get();
							_currentAction->Color = glm::vec3(0.f, 1.f, 0.f);
						}
					}


					// draging objects
					static int draggedObject = -1;
					if (ImGui::IsMouseReleased(0)) 
						draggedObject = -1;

					if (ImGui::IsItemClicked()) 
						draggedObject = aId;

					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && draggedObject != -1 && draggedObject != aId)
					{
						std::swap(actions[aId], actions[draggedObject]);
						draggedObject = aId;
					}


					if (active)
					{
						ImGui::PopStyleColor(2);
					}
					else
					{
						ImGui::PopStyleColor();
					}

					if (ImGui::BeginPopupContextItem("item context menu"))
					{
						if (ImGui::Selectable("Remove"))
						{
							_currentTimeline->removeAction(action.get());
							ImGui::EndPopup();
							ImGui::PopID();
							break;
						}

						ImGui::Separator();

						if (ImGui::Selectable("Move to left"))
						{
							for (int i = 0; i < actions.size(); ++i)
							{
								if (i > 0 && actions[i].get() == action.get())
								{
									std::swap(actions[i], actions[i - 1]);
									break;
								}
							}
						}

						if (ImGui::Selectable("Move to right"))
						{
							for (int i = 0; i < actions.size(); ++i)
							{
								if (i < actions.size() - 1 && actions[i].get() == action.get())
								{
									std::swap(actions[i], actions[i + 1]);
									break;
								}
							}
						}

						ImGui::EndPopup();
					}


					ImGui::PopID();

					ImGui::SameLine();
				}
			}
			ImGui::EndChild();

			ImGui::PopStyleVar();

			if (_currentAction)
			{
				ImGui::Indent(ImGui::GetStyle().IndentSpacing);

				switch (_currentAction->getType())
				{
					case Action::Move:
					{
						auto moveAction = static_cast<MoveAction*>(_currentAction);

						ImGui::PushItemWidth(-80);

						ImGui::Text("Action: Move");
						ImGui::Separator();

						ImGui::Text("Start position");

						ImGui::Checkbox("Use current position as start position", &moveAction->UseCurrentPosition);

						if (!moveAction->UseCurrentPosition)
						{
							if (ImGui::Button("C##Start"))
							{
								moveAction->Start = object->getPosition();
							}

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Set current position");
								ImGui::EndTooltip();
							}

							ImGui::SameLine();

							ImGui::DragVec3<ImGui::CopyPaste>("Start##", moveAction->Start);
						}

						ImGui::Spacing();

						ImGui::Text("End position");

						ImGui::Checkbox("Relative to Start", &moveAction->EndRelativeToStart);

						if (ImGui::Button("C##End"))
						{
							moveAction->End = object->getPosition();
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Set current position");
							ImGui::EndTooltip();
						}

						ImGui::SameLine();

						ImGui::DragVec3<ImGui::CopyPaste>("End", moveAction->End);
						ImGui::Spacing();

						ImGui::DragFloat<ImGui::CopyPaste>("Speed", moveAction->Speed, 0.01f, 0.f, 50.f);

						ImGui::Checkbox("Teleport", &moveAction->Teleport);

						ImGui::PopItemWidth();
					} break;
					case Action::Anim:
					{
						auto animAction = static_cast<AnimAction*>(_currentAction);

						ImGui::Text("Action: Anim");
						ImGui::Separator();

						if (ImGui::BeginCombo("Animation##2", animAction->AnimationName.c_str()))
						{
							if (auto arm = object->getComponentAs<ArmatureComponent>(); arm && arm->getArmature())
							{
								for (auto& anim : arm->getArmature()->getAnimation()->getAnimationNames())
								{
									bool isSelected = (animAction->AnimationName == anim);

									if (ImGui::Selectable(anim.c_str(), isSelected))
										animAction->AnimationName = anim;
									else
										ImGui::SetItemDefaultFocus();
								}
							}

							ImGui::EndCombo();
						}

						ImGui::DragFloat<ImGui::CopyPaste>("Fade in time", animAction->FadeInTime, 0.01f, 0.01f, 1.f);

						ImGui::Checkbox("Play once", &animAction->PlayOnce);

						if (animAction->PlayOnce)
							ImGui::Checkbox("Wait to the end of animation", &animAction->WaitToEnd);

						ImGui::Checkbox("Flip X", &animAction->FlipX);
					} break;
					case Action::Wait:
					{
						auto waitAction = static_cast<WaitAction*>(_currentAction);

						ImGui::Text("Action: Wait");
						ImGui::Separator();

						ImGui::DragFloat<ImGui::CopyPaste>("Time to wait", waitAction->TimeToWait, 0.1f, 0.1f, 60.f);
					} break;
					case Action::Script:
					{
						auto scriptAction = static_cast<ScriptAction*>(_currentAction);

						ImGui::Text("Action: Script");
						ImGui::Separator();

						if (ImGui::Button("Set script file path"))
						{
							// Path to script
							std::string file = scenes.getRelativePathFromExplorer("Select script file", ".\\Assets", "Lua (*.lua)|*.lua");

							// Loading script
							if (file != "")
							{
								scriptAction->ScriptFilePath = file;
							}
						}

						ImGui::Text("Path: %s", scriptAction->ScriptFilePath.empty() ? "None" : scriptAction->ScriptFilePath.c_str());
					} break;
				}

				ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
			}
		}
	}
}

}
