#pragma once

#include "Szczur/Modules/GUI/Base/BaseBar.hpp"

#include <SFML/Graphics/Texture.hpp>

#include "Szczur/Modules/PrepScreen/PrepSkill/PrepSkill.hpp"

namespace rat
{
    class PrepScreen;

    class ChosenSkillBar : public BaseBar
    {
    public:
        ChosenSkillBar(PrepScreen& prepScreen);

        void setSkill(const PrepSkill* skill);
        void removeSkill();
        bool hasSkill() const;
        bool isFree() const;
        bool isSkillUnbought() const { return !_skill->isBought(); }

        void initAssetsViaGUI(GUI& gui);

        void swapSkillsWith(ChosenSkillBar& other);

    private:
        PrepScreen& _prepScreen;
        const PrepSkill* _skill{nullptr};
        ImageWidget* _icon{nullptr};
        ImageWidget* _border{nullptr};      

        bool _hasSkill{false};

        void _onClick();

    };
}