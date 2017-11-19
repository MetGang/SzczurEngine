#include "GUI.hpp"

namespace rat {
    void GUI::init() {
        /*_root.add(new TextWidget(
            &_root, 
            TextWidget::createText("Hello Fucking World!", 100u, sf::Color(255,0,255))
        )) ->setPosition({500.f, 500.f});
        */
        _root.add( new CheckboxWidget(&_root) )
            ->setPosition({700.f, 700.f});
    }

    void GUI::input(const sf::Event& event) {
        if(
            event.type == sf::Event::MouseButtonPressed || 
            event.type == sf::Event::MouseButtonReleased || 
            event.type == sf::Event::MouseMoved ||
            event.type == sf::Event::TextEntered
        ) {
            _root.input(event);
        }
    }

    void GUI::update(float deltaTime) {
        _root.update(deltaTime);
    }

    void GUI::render() {
        getModule<Canvas>().draw(Canvas::LayerId::GUI, _root);
    }
}