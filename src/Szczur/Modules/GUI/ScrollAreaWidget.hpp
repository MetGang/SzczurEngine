#pragma once

#include <SFML/Graphics.hpp>

#include "Widget.hpp"
#include "Szczur/Modules/GUITest/Scroller.hpp"

namespace rat 
{
    class Script;
    namespace gui { class AnimData; }
    class ScrollAreaWidget : public Widget 
    {
    public:
        ScrollAreaWidget();

        static void initScript(Script& script);

        void setScrollerTexture(sf::Texture* texture, float boundsHeight = 140);
        void setPathTexture(sf::Texture* texture);
        void setBoundsTexture(sf::Texture* texture);        

        void setScrollSpeed(float speed);
        float getScrollSpeed() const;

        void resetScrollerPosition();
        void resetScrollerPositionInTime(float time);
        void resetScrollerPositionInTime(const gui::AnimData& data);
        void setScrollerProp(float prop);

        void setScrollWidth(float width);
        void setScrollPropWidth(float propWidth);
        void makeScrollAutoHiding();
    protected:
        virtual void _draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        virtual void _update(float deltaTime) override;
		virtual void _input(const sf::Event& event) override;
        virtual sf::Vector2f _getSize() const override;
		virtual void _calculateSize() override;

        virtual sf::Vector2f _getChildrenSize() override;
		virtual void _drawChildren(sf::RenderTarget& target, sf::RenderStates states) const override;
        
        virtual void _recalcChildrenPos() override;
        virtual void _recalcPos() override;
        virtual void _recalcElementsPropSize() override;   

        virtual sf::Vector2f _getInnerSize() const override;
    private:
        mutable sf::RenderTexture _renderTexture;
        mutable sf::Sprite _displaySprite;

        float _offset;
        float _scrollSpeed{7.f};

        Scroller _scroller;
        sf::Vector2f _minScrollSize{30.f, 50.f};

        void _recalcScroller();
        float _scrollerProp{0.f};

        float _childrenHeight{0};
        float _childrenHeightProp{1.f};

        bool _hasScrollerPropWidth{false};
        float _scrollerPropWidth;

        void _recalcScrollerPropWidth();

        bool _isAutoHiding{false};

        virtual void _callback(CallbackType type) override;
    };
}