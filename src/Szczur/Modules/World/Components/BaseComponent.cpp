#include "BaseComponent.hpp"

#include "../Entity.hpp"
#include "../Scene.hpp"

#include "Szczur/Utility/Convert/Windows1250.hpp"

#include <Szczur/Modules/World/World.hpp>
#include "Szczur/Modules/Script/Script.hpp"

#include <imgui.h>

namespace rat {
	
	BaseComponent::BaseComponent(Entity* parent)
	: Component { parent, fnv1a_64("BaseComponent"), "BaseComponent" }
	{

	}

	std::unique_ptr<Component> BaseComponent::copy(Entity* newParent) const
	{
		auto ptr = std::make_unique<BaseComponent>(*this);
		ptr->setEntity(newParent);
		return ptr;
	}

	void BaseComponent::loadFromConfig(Json& config)
	{
		Component::loadFromConfig(config);
		if(auto& var = config["position_only"]; !var.is_null()) _positionOnly = var;
	}

	void BaseComponent::saveToConfig(Json& config) const
	{
		Component::saveToConfig(config);
		config["position_only"] = _positionOnly;
	}

	void BaseComponent::renderHeader(ScenesManager& scenes, Entity* object) {
		if(ImGui::CollapsingHeader("Base##base_component", ImGuiTreeNodeFlags_DefaultOpen)) {
			
			// Set name with input text			
			static char name[255];
			std::strcpy(&name[0], object->getName().c_str());
			ImGui::InputText("", name, 255);
			object->setName(name);

			// Set position
			glm::vec3 position = object->getPosition();
			ImGui::DragFloat3("Position##base_component", reinterpret_cast<float*>(&position));
			if(ImGui::BeginPopupContextItem("Position##context")) {
				if(ImGui::Selectable("Copy##clipboard")) detail::globalPtr<World>->getLevelEditor().setClipboard(position);
				if(ImGui::Selectable("Paste##clipboard")) position = detail::globalPtr<World>->getLevelEditor().getClipboardVec3();
				ImGui::EndPopup();
			}
			object->setPosition(position);

			// Position only
			if(_positionOnly) return;

			// Set origin
			glm::vec3 origin = object->getOrigin();
			origin.y *= -1;
			ImGui::DragFloat3("Origin##base_component", reinterpret_cast<float*>(&origin));
			if(ImGui::BeginPopupContextItem("Origin##context")) {
				if(ImGui::Selectable("Copy##clipboard")) detail::globalPtr<World>->getLevelEditor().setClipboard(origin);
				if(ImGui::Selectable("Paste##clipboard")) origin = detail::globalPtr<World>->getLevelEditor().getClipboardVec3();
				ImGui::EndPopup();
			}
			object->setOrigin(origin);

			// Set rotation
			glm::vec3 rotation = object->getRotation();
			ImGui::DragFloat3("Rotation##base_component", reinterpret_cast<float*>(&rotation));
			if(ImGui::BeginPopupContextItem("Rotation##context")) {
				if(ImGui::Selectable("Copy##clipboard")) detail::globalPtr<World>->getLevelEditor().setClipboard(rotation);
				if(ImGui::Selectable("Paste##clipboard")) rotation = detail::globalPtr<World>->getLevelEditor().getClipboardVec3();
				ImGui::EndPopup();
			}
			object->setRotation(rotation);

			// Set scale
			glm::vec3 scale = object->getScale();
			static bool lockRatio = false;
			if (ImGui::DragFloat2("##Scale|base_component", reinterpret_cast<float*>(&scale), 0.01f))
			{
				if (lockRatio == false) {
					object->setScale(scale);
				}
				else {
					float offset = (scale.x - object->getScale().x) + (scale.y - object->getScale().y);

					auto scale = object->getScale();
					scale.x += offset;
					scale.y += offset;

					object->setScale(scale);
				}
			}
			if(ImGui::BeginPopupContextItem("Scale##context")) {
				if(ImGui::Selectable("Copy##clipboard")) detail::globalPtr<World>->getLevelEditor().setClipboard(scale);
				if(ImGui::Selectable("Paste##clipboard")) scale = detail::globalPtr<World>->getLevelEditor().getClipboardVec3();
				ImGui::EndPopup();
			}
			ImGui::SameLine(0.f, 0.f);
			ImGui::Checkbox("Scale##base_component", &lockRatio);
		}
	}

	void BaseComponent::positionOnly(bool flag) {
		_positionOnly = flag;
	}

	void BaseComponent::initScript(ScriptClass<Entity>& entity, Script& script)
	{
		auto object = script.newClass<BaseComponent>("BaseComponent", "World");

		// Entity
		entity.set("move", [](Entity& entity, float x, float y, float z){entity.move({x,y,z});});
		entity.set("setPosition", [](Entity& entity, float x, float y, float z){entity.setPosition({x,y,z});});
		entity.set("getPosition", [](Entity& entity){return entity.getPosition();});

		entity.set("rotate", [](Entity& entity, float x, float y, float z){entity.rotate({x,y,z});});
		entity.set("setRotation", [](Entity& entity, float x, float y, float z){entity.setRotation({x,y,z});});
		entity.set("getRotation", [](Entity& entity){return entity.getRotation();});

		entity.set("scale", [](Entity& entity, float x, float y, float z){entity.scale({x,y,z});});
		entity.set("setScale", [](Entity& entity, float x, float y, float z){entity.setScale({x,y,z});});
		entity.set("getScale", [](Entity& entity){return entity.getScale();});

		entity.set("setOrigin", [](Entity& entity, float x, float y, float z){entity.setOrigin({x,y,z});});
		entity.set("getOrigin", [](Entity& entity){return entity.getOrigin();});

		object.init();

	}
}
