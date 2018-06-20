#include "SkillBar.hpp"

#include "Szczur/Modules/GUI/GUI.hpp"
#include "Szczur/Modules/GUI/ImageWidget.hpp"
#include "Szczur/Modules/GUI/Widget.hpp"
#include "Szczur/Modules/GUI/TextWidget.hpp"
#include "Szczur/Modules/GUI/WindowWidget.hpp"
#include "Szczur/Modules/GUI/ListWidget.hpp"

#include "GrayPPArea.hpp"
#include "SkillArea.hpp"
#include "ChosenSkillArea.hpp"

#include "Szczur/Utility/Logger.hpp"


namespace rat
{
    
     sf::Vector2u SkillBar::_size = {240, 72};

    SkillBar::SkillBar(SkillArea& parentArea)
    :
    _parentArea(parentArea),
    _chosenArea(_parentArea.getChosenSkillArea()),
    _sourceArea(_parentArea.getSourceArea()),
    BaseBar([]{auto* base = new ListWidget; base->makeHorizontal(); return base;}())
    {
        setSize(_size);

        _iconWindow = new WindowWidget; 
        _icon = new ImageWidget;
        _infoBar = new WindowWidget;
        _name = new TextWidget;

        _iconWindow->setPropSize(0.09f, 0.09f);
        _iconWindow->setScale(0.3f, 0.3f);
        _addWidget(_iconWindow);        
        
        
        _icon->setPropSize(0.08f, 0.08f);
        _icon->setPropPosition(0.5f, 0.5f);
        _iconWindow->add(_icon);

        _infoBar->setPropSize(0.21f, 0.09f);
        _infoBar->setScale(0.3f, 0.3f);
        _infoBar->setPadding(10, 10);
        _infoBar->makeChildrenPenetrable();
        _infoBar->setCallback(Widget::CallbackType::onPress, [&](Widget* owner){
            _onClick();
        });
        _infoBar->setCallback(Widget::CallbackType::onHoverIn, [&](Widget* owner){
            _onHoverIn();
            owner->setColor({180, 180, 180}, 0.3f);
        });
        _infoBar->setCallback(Widget::CallbackType::onHoverOut, [&](Widget* owner){
            _onHoverOut();
            owner->setColor({255, 255, 255}, 0.3f);
        });
        _addWidget(_infoBar);

        _name->setCharacterSize(20);
        _name->setPropPosition(0.f, 0.f);
        _infoBar->add(_name);

        _costBar.setParent(_infoBar);
        _costBar.setPropPosition(0.f, 1.f);
        _costBar.setWidth(_infoBar->getMinimalSize().x);
    }

    void SkillBar::setSkill(Skill* skill)
    {
        _skill = skill;
        _name->setString(skill->getName());
        _costBar.setSkill(skill);
        _icon->setTexture(skill->getTexture());
    }

    const std::string& SkillBar::getIconPath() const
    {
        assert(_skill);
        return _skill->getTexturePath();
    }

    void SkillBar::_onClick()
    {      
        auto& source = _sourceArea.getSource();
        std::cout << "Clicked\n";

        if(isBought()) return;
        
        std::cout << "Not bought\n";
        //if(!_skill->canBeBoughtFrom(source)) return;
        std::cout << "Enough\n";
        if(!_chosenArea.hasFreeSpace()) return;

        std::cout << "Cyk Kupione\n";
        _skill->buyFrom(source);

        _chosenArea.addSkill(_skill);

        _parentArea.recalculate();
        _sourceArea.recalculate();
            
    }

    void SkillBar::_onHoverIn()
    {
        if(!_skill) return;    
        _parentArea.setSkillInfo(_skill, {float(getSize().x) + 40.f, getPosition().y});
    }
    void SkillBar::_onHoverOut()
    {
        if(!_skill) return;
        if(_parentArea.isSkillInInfo(_skill) || true) _parentArea.deactivateInfo();
    }

    void SkillBar::loadAssetsFromGUI(GUI& gui)
    {
        auto* barTex = gui.getAsset<sf::Texture>("Assets/Test/Bar.png");
        _iconWindow->setTexture(barTex, 6);
        _infoBar->setTexture(barTex, 6);
        _name->setFont(gui.getAsset<sf::Font>("Assets/fonts/NotoMono.ttf"));
        _costBar.loadAssetsFromGUI(gui);
    }
    
}