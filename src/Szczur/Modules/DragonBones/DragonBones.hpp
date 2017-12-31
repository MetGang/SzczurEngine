#pragma once

#include <memory>

#include <dragonBones/SFMLFactory.h>

#include "Szczur/Modules/Canvas/Canvas.hpp"
#include "Szczur/Utility/Modules.hpp"

#include "Armature.hpp"

namespace rat 
{
	class DragonBones : public ModuleBase<Canvas> 
	{ 
		using ModuleBase::ModuleBase;
	
	private:
		std::unique_ptr<dragonBones::SFMLFactory> _factory;

	public:
		void init();

		void update(float deltaTime);

		void render();

		void input(sf::Event& e);

		Armature* createArmaturee(const std::string& actorName);

		void addSoundEvent(const std::function<void(dragonBones::EventObject*)>& listener) { _factory->addSoundEventListener(listener); }
	};
}
