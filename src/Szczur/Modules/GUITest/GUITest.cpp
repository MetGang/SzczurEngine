#include "GUITest.hpp"

#include "StressTester.hpp"

#include "Szczur/Modules/GUI/Widget.hpp"
#include "Szczur/Modules/GUI/ImageWidget.hpp"
#include "Szczur/Modules/GUI/ScrollAreaWidget.hpp"
#include "Szczur/Modules/GUI/WindowWidget.hpp"
#include "Szczur/Modules/GUI/ListWidget.hpp"
#include "Szczur/Modules/GUI/TextWidget.hpp"
#include "Szczur/Modules/GUI/TextAreaWidget.hpp"

#include "Szczur/Modules/GUI/Animation/Anim.hpp"

namespace rat
{
    GUITest::GUITest()
    {
        auto& mainWindow = getModule<Window>();
        auto& window = mainWindow.getWindow();

        mainWindow.pushGLStates();
        _canvas.create(window.getSize().x, window.getSize().y);
        mainWindow.popGLStates();

        init();
    }
    void GUITest::init()
    {
        auto& gui = getModule<GUI>();

        /*
        gui.addAsset<sf::Texture>("Assets/Test/NinePatchTest.png");
        gui.addAsset<sf::Texture>("Assets/Test/Scroller.png");
        gui.addAsset<sf::Texture>("Assets/Test/ScrollerBar.png");
        gui.addAsset<sf::Texture>("Assets/Test/ScrollerBound.png");
        gui.addAsset<sf::Texture>("Assets/Test/NinePatchTest.png");
        */

        gui.addAsset<sf::Texture>("Assets/GUITest/Blue.png");
        gui.addAsset<sf::Texture>("Assets/GUITest/Red.png");
        gui.addAsset<sf::Font>("Assets/GUITest/lumos.ttf");
        gui.addTexture("Assets/GUITest/Cubic.png");

        _widget = gui.addInterface();
        _widget->setPadding(10.f, 20.f);

        _widget->setSizingWidthToHeightProportion(1.f);
        //_widget->setWidthToHeightProp(16.f/9.f);

        scroll = new ScrollAreaWidget;
        _widget->add(scroll);

        scroll->setScrollerTexture(gui.getAsset<sf::Texture>("Assets/Test/Scroller.png"));
        scroll->setPathTexture(gui.getTexture("Assets/Test/ScrollerBar.png"));
        scroll->setBoundsTexture(gui.getTexture("Assets/Test/ScrollerBound.png"));

        scroll->setPropSize(0.5f, 0.5f);
        scroll->setScrollPropWidth(0.02f);
        scroll->makeScrollAutoHiding();
        scroll->setPropPosition(1.0f, 1.0f);

        //auto gui::AnimData{4.f, gui::Easing::EaseInQuad, [](){}};

        // scroll->setPropPosition({0.5f, 0.f}, {3.f, gui::Easing::EaseInBack, [scroll](){
        //     scroll->setPropPosition({0.5f, 0.5f}, {3.f, gui::Easing::EaseInOutBack, [scroll](){
        //         scroll->setPropPosition({0.f, 0.5f}, {3.f, gui::Easing::EaseOutBack, [scroll](){
        //             scroll->setPropPosition({0.f, 0.f}, {3.f, gui::Easing::EaseInBack, [scroll](){
        //                     scroll->setPropPosition({0.5f, 0.f}, {3.f, gui::Easing::EaseInBack, [scroll](){
            
        //                 }});
        //             }});
        //         }});
        //     }});
        // }});

        
        // image = new ImageWidget;
        // image->setTexture(gui.getAsset<sf::Texture>("Assets/GUITest/Cubic.png"));
        // image->setPropSize(0.3f, 0.3f);
        // image->setFullyTexSizing();
        // image->setStaticTexPositing();
        // image->setPropPosition(0.5f, 0.5f);
        // _widget->add(image);
        
        list = new ListWidget;
        list->makeReversed();
        //list->setBetweenPadding(20.f);
        scroll->add(list);
        //list->setPropPosition(0.f, 1.f);
        //list->setPropSize(0.3f, 1.f);
        //list->setAutoBetweenPadding();

        

        float size = 0.1f;
        for(int i = 0; i < 2; i++)
        {
            auto* w = new ImageWidget;
            list->add(w);
            w->setPropSize(size, size);
            size += 0.05f;
            w->setTexture(gui.getAsset<sf::Texture>("Assets/GUITest/Blue.png"));
            
            w->setCallback(Widget::CallbackType::onHoverIn, [w](auto){
                w->setColorInTime({0, 0, 0}, {1.f, gui::Easing::EaseInCubic});
            });
            w->setCallback(Widget::CallbackType::onHoverOut, [w](auto){
                w->setColorInTime({255, 255, 255}, {1.f, gui::Easing::EaseInCubic});
            });
        }

        fps = new TextAreaWidget;
        _widget->add(fps);
        fps->setFont(gui.getAsset<sf::Font>("Assets/GUITest/lumos.ttf"));
        //fps->setCharacterPropSize(0.06f);
        fps->setCharacterSize(40u);
        fps->setColor({255, 255, 255});
        fps->setString(".");
        fps->setAlign(TextAreaWidget::Align::Center);
        fps->setPropPosition(0.5f, 1.f);
        fps->setOutlineColor({255, 0, 255});
        fps->setOutlinePropThickness(0.02f);
        //fps->makeStaticPropPositing();
        //fps->setPropOrigin(0.5f, 0.5f);
 
    }
    
    
    void GUITest::update(float deltaTime)
    {
        const auto& window = getModule<Window>().getWindow();

        [[maybe_unused]] auto mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            _scale-= deltaTime * 0.4f;
            if(_scale < 0.2f) _scale = 0.2f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            _scale+= deltaTime * 0.4f;
            //if(_scale > 1.f) _scale = 1.f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            _prop+= deltaTime * 0.4f;
            if(_prop > 1.f) _prop = 1.f;            
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            _prop-= deltaTime * 0.4f;
            if(_prop < 0.f) _prop = 0.f;
        }
        /*
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            _shift+= deltaTime * 0.4f;
            if(_shift > 1.f) _shift = 1.f;            
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            _shift-= deltaTime * 0.4f;
            if(_shift < 0.f) _shift = 0.f;
        }
        */
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            _size.x -= deltaTime * 150.f;
            if(_size.x < 0.f) _size.x = 0.f;
            //if(_shift > 1.f) _shift = 1.f;            
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            _size.x += deltaTime * 150.f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            _size.y += deltaTime * 150.f;
            //if(_shift > 1.f) _shift = 1.f;            
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            _size.y -= deltaTime * 150.f;
            if(_size.y < 0.f) _size.y = 0.f;
        }

        //fps->setOutlineThickness(10.f * _prop);

        //list->setBetweenPadding(400.f * _prop);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            fps->setSize(mousePos.x, 0.f);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) fps->setAlign(TextAreaWidget::Align::Left);
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) fps->setAlign(TextAreaWidget::Align::Center);
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) fps->setAlign(TextAreaWidget::Align::Right);

        //scroll->setScrollPropWidth(_prop);
        //std::cout << _prop << '\n';
        //fps->setString(std::to_string(int(1.f / deltaTime)) + " fps");

        //fps->setPropPosition(_prop, _prop);

        //image->setPropTextureRect({{_prop * 0.5f, _prop * 0.5f}, {1.f - _prop, 1.f - _prop}});
    }
    void GUITest::input(const sf::Event& event)
    {
        switch(event.type)
        {
            case sf::Event::KeyPressed:
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Num1:
                        fps->setString("Lorem ipsum dolor sit amet, consectetur adipisicing elit.");
                    break;
                    case sf::Keyboard::Num2:
                        fps->setString("Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
                    break;
                    case sf::Keyboard::Num3:
                        fps->setString("Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
                    break;

                    default: break;
                }
            } 
            break;

            default: break;
        }
    }
    void GUITest::render()
    {
        auto& mainWindow = getModule<Window>();

        mainWindow.pushGLStates();

        _canvas.clear(sf::Color::Transparent);
        _canvas.display();

        mainWindow.getWindow().draw(sf::Sprite(_canvas.getTexture()));

        mainWindow.popGLStates();
    }
}