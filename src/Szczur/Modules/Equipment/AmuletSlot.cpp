#include "AmuletSlot.hpp"
#include "EquipmentObject.hpp"
#include "Szczur/Modules/GUI/ImageWidget.hpp"

namespace rat {
	AmuletSlot::AmuletSlot(sf::Texture* slotText, sf::Texture* upText, sf::Texture* downText, sf::Vector2u slotSize) {
		yOffset = slotSize.y / 2 + 5;		//offset needen't becouse up arrow cannot be on - y

		_slot = new ImageWidget;
		_slot->setTexture(slotText);
		_addWidget(_slot);
		_slot->setSize(slotSize);
		_slot->setPosition(sf::Vector2f(0.f, yOffset));
		
		_upArrow = new ImageWidget;
		_upArrow->setTexture(upText);
		_addWidget(_upArrow);
		_upArrow->setSize(slotSize/2u);
		_upArrow->setPosition(sf::Vector2f(slotSize.x / 4, 0.f));

		_downArrow = new ImageWidget;
		_downArrow->setTexture(downText);
		_addWidget(_downArrow);
		_downArrow->setSize(slotSize / 2u);
		_downArrow->setPosition(sf::Vector2f(slotSize.x / 4, slotSize.y + 5 + yOffset));

		_amuletImage = new ImageWidget;
		_addWidget(_amuletImage);
		_amuletImage->setSize(slotSize);
		_amuletImage->setPosition(0.f, yOffset);
	}
	void AmuletSlot::addAmulet(EquipmentObject* amulet) {
		//_amulets.push_back(amulet);
		_amulets.push_back(amulet);
		if (_chosenAmulet == nullptr) {
			_amuletImage->resetColor();
			_chosenAmulet = amulet;
			_amuletImage->setTexture(amulet->getIcon());
		}
	}
	void AmuletSlot::removeAmulet(sf::String name) {
		for (size_t i = 0; i < _amulets.size(); i++)
		{
			if (_amulets[i]->getName() == name) {
				if (_chosenAmulet->getName() == name) {
					if (i != 0)		//there is some other amulet to display
					{
						_amuletImage->setTexture(_amulets[i - 1]->getIcon());
						_chosenAmulet = _amulets[i - 1];
						_amulets.erase(_amulets.begin() + i);
					}
					else {
						_amuletImage->setColor(sf::Color::Color(0, 0, 0, 0));
						_amulets.erase(_amulets.begin());
						_chosenAmulet = nullptr;
					}
				}
				else {
					_amulets.erase(_amulets.begin() + i);
				}

			}
		}
	}
	EquipmentObject* AmuletSlot::getChosenAmulet() {
		return _chosenAmulet;
	}
	std::vector<EquipmentObject*> AmuletSlot::getAmuletList() {
		return _amulets;
	}
}