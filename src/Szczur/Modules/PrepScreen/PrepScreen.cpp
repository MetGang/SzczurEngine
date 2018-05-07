#include "PrepScreen.hpp"
#include "Skill.hpp"

#include "GUI/SkillBar.hpp"

#include "ProfessionTypes.hpp"

#include "Szczur/Modules/GUI/ImageWidget.hpp"
#include "Szczur/Modules/GUI/Widget.hpp"
#include "Szczur/Modules/GUI/TextWidget.hpp"
#include "Szczur/Modules/GUI/ScrollAreaWidget.hpp"

#include "Szczur/Utility/Logger.hpp" 

namespace rat
{
    void PrepScreen::initScript()
    {
        Script& script = getModule<Script>();
        auto module = script.newModule("PrepScreen");

        SCRIPT_SET_MODULE(PrepScreen, getSkillCodex);

    }

    SkillCodex& PrepScreen::getSkillCodex()
    {
        return _codex;
    }

    sf::Texture* PrepScreen::getTextureFrom(const std::string& name, GUI& gui)
    {
        std::string path = _folder + name;
        gui.addAsset<sf::Texture>(path);
        return gui.getAsset<sf::Texture>(path);
    }

    void PrepScreen::initGUI()
    {
        auto& gui = getModule<GUI>();

        _codex.initAssetsViaGUI(gui);

        gui.addAsset<sf::Texture>("assets/PrepScreen/MenuBar.png");
        sf::Texture* menuBarTexture = gui.getAsset<sf::Texture>("assets/PrepScreen/MenuBar.png");

        _base = gui.addInterface();

        ImageWidget* ppBar = new ImageWidget;
        _base->add(ppBar);
        ppBar->setTexture(menuBarTexture);
        _setWidgetSize(ppBar, 1280.f, 120.f);
        
        _coloredPPBar = new ImageWidget;
        _grayPPBar = new Widget;
        _skillsBar = new ScrollAreaWidget;
        _chosenSkillsBar = new ImageWidget;

        ppBar->add(_coloredPPBar);
        //_coloredPPBar->setTexture(getTextureFrom("coloredPPBar.png", gui));

        ppBar->add(_grayPPBar);
        //_grayPPBar->setTexture(getTextureFrom("grayPPBar.png", gui));
        _grayPPBar->setPosition(0.f, 80.f);

        _centerWindow = new ImageWidget;
        _base->add(_centerWindow);
        _centerWindow->setTexture(menuBarTexture);
        _centerWindow->setPosition(200, 120);
        _setWidgetSize(_centerWindow, 760.f, 480.f);

        _skillsBar = new ScrollAreaWidget;
        _centerWindow->add(_skillsBar);
        _skillsBar->setPosition(40, 40);
        _skillsBar->setSize(240, 400);
        _skillsBar->setScrollSpeed(15);

        _eq = new ImageWidget;
        _base->add(_eq);
        _eq->setTexture(menuBarTexture);
        _eq->setPosition(0, 600);
        _setWidgetSize(_eq, 200.f, 120.f);

        _base->add(_chosenSkillsBar);
        _chosenSkillsBar->setTexture(menuBarTexture);
        _chosenSkillsBar->setPosition(200, 600);
        _setWidgetSize(_chosenSkillsBar, 760.f, 120.f);
             
        //mainWidget

        test();
    }


    void PrepScreen::test()
    {
        auto& gui = getModule<GUI>();

        _coloredPPsBar.setParent(_coloredPPBar);
        _coloredPPsBar.initTexturesViaGui(gui);
        _coloredPPsBar.setCenter(1280.f/2.f, 0.f);

        _grayPPsBar.setParent(_grayPPBar);
        _grayPPsBar.initTextureViaGui(gui);
        _grayPPsBar.setCenter(1280.f/2.f, 0);

        PPSource fireSource = {"Fire", 1, 2};
        PPSource earthSource = {"Earth", 1, 3};
        PPSource essenceSource = {"Essence", 1, 3};
        PPSource physicalSource = {"Physical", 1, 2};
        PPSource waterSource = {"Water", 1, 1};
        PPSource spiritSource = {"Spirit", 1, 1};
        PPSource airSource = {"Air", 1, 0};
        
        _source.addSource(fireSource);
        _source.addSource(earthSource);
        _source.addSource(essenceSource);
        /*
        _source.addSource(physicalSource);
        _source.addSource(waterSource);
        _source.addSource(spiritSource);
        _source.addSource(airSource);
        */
        
        _coloredPPsBar.recalculate();
        _grayPPsBar.recalculate();
        


        _coloredPPsBar.recalculate();
        _grayPPsBar.recalculate();

        _initSkillArea();
        _initChosenSkillArea();
        _initProfArea();
        _initColorFilterArea();

        _infoBar.setParent(_base);
        _infoBar.setPosition(200.f, 400.f);
        _infoBar.setName("Test");
        _infoBar.initAssetsViaGUI(gui);
        _infoBar.setInfo("Testowe info\nHalo halo dzieciak efwefe wfsdvds grefdbkrfikbnj hrebf wkjbjerfkefb reji nnbi nui uyt  h ctr v yt  vhyt  h crcg ctr gc yctych  tyc ytc trc h t yh vg c\nAmadeusz");
    }

    void PrepScreen::_setWidgetSize(ImageWidget* widget, float x, float y)
    {
        auto baseSize = static_cast<sf::Vector2f>(widget->getTexture()->getSize());
        widget->setScale({x/baseSize.x, y/baseSize.y}); 
    }

    void PrepScreen::_initSkillArea()
    {
        _skillArea.initChosenSkillArea(_chosenSkillArea);
        _skillArea.setParent(_centerWindow);
        _skillArea.initViaSkillCodex(_codex);
        _skillArea.initAssetsViaGUI(getModule<GUI>());
        _skillArea.activate("Mele", {});
    }

    void PrepScreen::_initProfArea()
    {
        _profArea.setParent(_base);
        _profArea.setPosition(40.f, 120.f);
        _profArea.initAssetsViaGUI(getModule<GUI>());
    }
    
    void PrepScreen::_initChosenSkillArea()
    {
        _chosenSkillArea.setParent(_base);
        _chosenSkillArea.setPosition(200.f, 600.f);
    }
    void PrepScreen::_initColorFilterArea()
    {
        _colorFilterArea.setParent(_base);
        _colorFilterArea.setPosition(500.f, 200.f);
        _colorFilterArea.initAssetsViaGUI(getModule<GUI>());
        _colorFilterArea.setWidth(250.f);   
    }
    
}