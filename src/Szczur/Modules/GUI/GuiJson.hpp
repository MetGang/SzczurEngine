#pragma once

#include <string>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <boost/lexical_cast.hpp>

#include <Szczur/Json.hpp>

#include "Widget.hpp"
#include "ImageWidget.hpp"
#include "TextWidget.hpp"
#include "InputWidget.hpp"
#include "CheckWidget.hpp"
#include "RadioWidget.hpp"



namespace rat {
    class GuiJson {
    public:
        using Function_t = std::function<bool()>;

        void init(const std::string& filePath, Widget* root, const sf::Vector2u& windowSize);

        void reload(Widget *root);
    private:
        void _browseJsonObject(Json &json, Widget *parent);

        template<typename T, typename F>
        void _createJsonValue(Json& json, Widget* parent, F valuesCall);

        bool _handleBasicValues(Json::iterator it, Widget *widget);

        int _stringToValue(const std::string &strOrigin, unsigned int valueOf);

        Widget::CallbackType _stringTypeToEnum(const std::string &name);

        sf::Vector2u _windowSize;
        Json _json;
    };
}