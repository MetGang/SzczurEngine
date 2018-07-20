#include "ProfessionBar.hpp"

#include "Szczur/Modules/PrepScreen/PrepScreen.hpp"

#include "Szczur/Modules/GUI/GUI.hpp"
#include "Szczur/Modules/GUI/ImageWidget.hpp"
#include "Szczur/Modules/GUI/WindowWidget.hpp"

#include "Szczur/Utility/Logger.hpp"

#include <ctime>

namespace rat
{
    ProfessionBar::ProfessionBar(PrepScreen& prepScreen)
    :
    _prepScreen(prepScreen),
    BaseBar()
    {
        _window = new WindowWidget;
        _addWidget(_window);
        _window->makeChildrenPenetrable();
        _window->setPadding(0.f, 0.f);
        _window->setMainPatchPropSize({0.3, 0.3});
        _window->setPropSize({0.15f, 0.15f});
        _window->makeChildrenUnresizable();

        _icon = new ImageWidget;

        auto click = [this](Widget* owner){
            _onClick();
        };
        _icon->setPropSize(0.11f, 0.11f);
        _icon->setPropPosition(0.5f, 0.5f);

        _window->add(_icon);
        _window->setCallback(Widget::CallbackType::onPress, click);
        _window->setCallback(Widget::CallbackType::onHoverIn, [this](Widget* owner){
            if(!_isChosen)
            {
                _icon->setColorInTime({180, 180, 180}, 0.2f);
            }
            _isHovered = true;
        });
        _window->setCallback(Widget::CallbackType::onHoverOut, [this](Widget* owner){
            if(!_isChosen)
            {
                _icon->setColorInTime({255, 255, 255}, 0.2f);
            }
            _isHovered = false;
        });
    }

    void ProfessionBar::initAssetsViaGUI(GUI& gui)
    {
        std::string path = "Assets/PrepScreen/Professions/" + _profession + "Icon";
        gui.addAsset<sf::Texture>(path + "0.png");
        gui.addAsset<sf::Texture>(path + "1.png");

        _unchosen = gui.getTexture(path + "0.png");
        _chosen = gui.getTexture(path + "1.png");

        _icon->setTexture(_unchosen);
        _window->setTexture(gui.getAsset<sf::Texture>("Assets/PrepScreen/SkillBack.png"), 30);
        //_window->setScale(0.2f, 0.2f);
        //_window->setPadding(5.f, 5.f);
    }

    void ProfessionBar::setProfession(const std::string& profession)
    {
        _profession = profession;
    }
    const std::string& ProfessionBar::getProfession() const
    {
        return _profession;
    }

    void ProfessionBar::_onClick()
    {
        _prepScreen.setProfession(_profession);
        _isChosen = true;
        _icon->setTexture(_chosen);
        //_icon->setPropSize(0.11f, 0.11f);
    }

    void ProfessionBar::unchoose()
    {
        _isChosen = false;
        if(!_isHovered)
        {
            _icon->setTexture(_unchosen);
            //_icon->setPropSize(0.11f, 0.11f);
        }
        _window->setColorInTime({255, 255, 255}, 0.2f);

    }
    
}