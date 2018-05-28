#include "Scroller.hpp"

#include "Szczur/Utility/Logger.hpp"

namespace rat
{
    void Scroller::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(_path, states);
        target.draw(_scroller, states);
        target.draw(_upperBound, states);
        target.draw(_bottomBound, states);
    }

    void Scroller::setPosition(float x, float y)
    {
        _position = {x, y};
        sf::Transformable::setPosition(_position);        
       _recalcAll();
    }
    void Scroller::setPosition(const sf::Vector2f& position)
    {
        setPosition(position.x, position.y);
    }
    void Scroller::setScrollerPosition(const sf::Vector2f& position)
    {
        _scroller.setPosition(position);
        _recalcScrollerPos();
    }
    void Scroller::setScrollerHeightProp(float prop)
    {
        if(prop < 1.f) prop = 1.f;
        _scrollerHeightProp = prop;
        if(_isScrollerSet)
        {
            _recalcScrollerSize();
            _recalcScrollerSize();
        }
    }
    
    
    

    void Scroller::setPathTexture(const sf::Texture* texture)
    {
        _isPathSet = true;
        _path.setTexture(*texture);
        _recalcAll();
    }
    void Scroller::setScrollerTexture(sf::Texture* texture, int boundHeight)
    {
        _isScrollerSet = true;
        auto texSize = texture->getSize();
        auto texHalfWidth = int(round(float(texSize.x) * 0.5f));
        _scroller.setTexture(texture, texHalfWidth, 139);
        _recalcAll();
    }

    void Scroller::setBoundTexture(const sf::Texture* texture)
    {
        _areBoundsSet = true;
        _upperBound.setTexture(*texture);
        _bottomBound.setTexture(*texture);
        _recalcAll();
    }
    
    
    void Scroller::setProportion(float proportion)
    {
        if(proportion < 0.f) proportion = 0.f;
        if(proportion > 1.f) proportion = 1.f;
        _proportion = proportion;
        if(_isScrollerSet) _recalcScrollerPosByProp();
    }

    void Scroller::setScrollerPropHeight(float propY)
    {
        if(propY < 1.f) propY = 1.f;
        _scrollerHeightProp = propY;
        _recalcScrollerSize();
        _recalcScrollerPosByProp();
    }

    void Scroller::moveProportion(float proportionOffset)
    {
        setProportion(_proportion + proportionOffset/_scrollerHeightProp);
    }
    float Scroller::getProportion() const
    {
        return _proportion;
    }
    
    

    void Scroller::setSize(int width, int height)
    {
        setSize({width, height});
    }
    
    void Scroller::setSize(const sf::Vector2i& size)
    {
        _size = size;
        _recalcAll();
    }
    void Scroller::setWidthProportion(float proportion)
    {
        _widthProp = proportion;
        _recalcAll();
    }

    void Scroller::setBoundShiftProportion(float proportion)
    {
        if(proportion < 0.f) proportion = 0.f;
        if(proportion > 1.f) proportion = 1.f;
        _boundShiftProp = proportion;
        _recalcAll();
    }

    sf::Vector2u Scroller::getSize() const
    {
        return sf::Vector2u(_size);
    }

    sf::Vector2i Scroller::getScrollerSize() const
    {
        return _scroller.getSize();
    }
    sf::Vector2f Scroller::getScrollerPosition() const
    {
        return _scroller.getPosition();
    }
    
    

    void Scroller::input(sf::Event event)
    {
        
        switch (event.type)
        {
            case sf::Event::MouseMoved:
            {
                if(_hasBeenClicked)
                {
                    _clickedShift = sf::Vector2f(event.mouseMove.x, event.mouseMove.y) - _scroller.getPosition();
                    _hasBeenClicked = false;
                }
                if(_isClicked)
                {
                    setScrollerPosition(sf::Vector2f(event.mouseMove.x, event.mouseMove.y) - _clickedShift);
                }
                event.mouseMove.x -= ((_size.x - _scroller.getSize().x)/2) + _scroller.getPosition().x;
                event.mouseMove.y -= (_scroller.getPosition().y);

                bool isMouseOverlap = event.mouseMove.x >= 0 &&
                    event.mouseMove.x <= _scroller.getSize().x /* * _winProp.x */&&
                    event.mouseMove.y >= 0 &&
                    event.mouseMove.y <= _scroller.getSize().y /* * _winProp.y*/;

                if(isMouseOverlap)
                {
                    _isHovered = true;
                }
                else _isHovered = false;

            } break;

            case sf::Event::MouseButtonPressed:
            {
                if(_isHovered) 
                {
                    _isClicked = true;
                    _hasBeenClicked = true;
                }
            } break;

            case sf::Event::MouseButtonReleased:
            {
                _isClicked = false;
            }
        }
    }

    void Scroller::_recalcPathSize()
    {
        float widthProp = 1.f/_widthProp;

        sf::Vector2f newScale = _path.getScale();
        auto texSize = sf::Vector2f{float(_path.getTextureRect().width), float(_path.getTextureRect().height)};
        newScale.x = float(_size.x)/texSize.x;
        if(widthProp < 1.f) 
        {
            newScale.x *= widthProp;
        }
        newScale.y = (float(_size.y) - 2.f * _getRealBoundLength()) / texSize.y;

        _path.setScale(newScale);
    }
    void Scroller::_recalcPathPos()
    {
        float pathWidth = _path.getGlobalBounds().width;
        float width = float(_size.x);

        float xShift = (width - pathWidth) / 2.f;
        float newX = xShift;

        _path.setPosition(newX, _getRealBoundLength());
    }

    void Scroller::_recalcScrollerSize()
    {
        auto innerTexSize = _scroller.getInnerTextureRect();
        sf::Vector2i texSize = {innerTexSize.left * 2, innerTexSize.top * 2};
        float scale = float(_size.x) / float(texSize.x);
        _scroller.setScale(scale, scale);

        auto newSize = _size;
        if(_widthProp < 1.f) 
        {
            newSize.x = _size.x * _widthProp;
        }
        newSize.y = _path.getGlobalBounds().height / _scrollerHeightProp;
        _scroller.setSize(newSize);

    }
    
    void Scroller::_recalcScrollerPos()
    {
        auto scrollerPos = _scroller.getPosition();
        float newX = scrollerPos.x;
        float newY = scrollerPos.y;

        auto realPathLength = _getRealPathLength();
        float maxTop = _path.getGlobalBounds().top;
        float maxBottom = maxTop + realPathLength;

        if(newY < maxTop) newY = maxTop;
        if(newY > maxBottom) newY = maxBottom;

        float scrollerWidth = _scroller.getSize().x;
        float width = float(_size.x);

        float xShift = (width - scrollerWidth) / 2.f;
        newX = xShift;

        _scroller.setPosition(newX, newY);

        float prop = (newY - maxTop) / realPathLength;

        _proportion = std::max(0.f, std::min(1.f, prop));
    }
    
    void Scroller::_recalcScrollerPosByProp()
    {
        auto scrollerPos = _scroller.getPosition();
        auto realPathLength = _getRealPathLength();
        float maxTop = _path.getGlobalBounds().top;

        _scroller.setPosition(scrollerPos.x, maxTop + (realPathLength * _proportion));
    }

    void Scroller::_recalcBoundPos()
    {
        float boundHeight = _bottomBound.getGlobalBounds().height;
        _bottomBound.setPosition(0.f, float(_size.y));
    }
    void Scroller::_recalcBoundSize()
    {
        auto texSize = sf::Vector2f{float(_upperBound.getTextureRect().width), float(_upperBound.getTextureRect().height)};
        float scale = float(_size.x) / texSize.x;
        _upperBound.setScale(scale, scale);
        _bottomBound.setScale(scale, -scale);
    }
    
    float Scroller::_getRealPathLength() const
    {
        //if(!_isPathSet) return 0.f;
        auto length =  _path.getGlobalBounds().height - _scroller.getSize().y;
        if(length < 0.f) length = 0.f;
        return length;
    }
    float Scroller::_getRealBoundLength() const
    {
        if(!_areBoundsSet) return 0.f;
        return (_upperBound.getGlobalBounds().height * _boundShiftProp);
    }

    void Scroller::_recalcAll()
    {
        if(_areBoundsSet)
        {
            _recalcBoundSize();
            _recalcBoundPos();
        }
        if(_isPathSet)
        {
            _recalcPathSize();
            _recalcScrollerSize();
        }
        if(_isScrollerSet)
        {
            _recalcScrollerPosByProp();
            _recalcScrollerPos();
            _recalcPathPos();
        }
    }
    
}