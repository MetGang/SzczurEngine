#pragma once
#include "Szczur/Modules/GUI/GUI.hpp"
#include "Szczur/Modules/GUI/TextWidget.hpp"
#include "Szczur/Modules/GUI/ScrollAreaWidget.hpp"
#include "Szczur/Modules/GUI/ListWidget.hpp"

#include "StepsManager.hpp"
#include "DescriptionManager.hpp"
#include "QuestName.hpp"
#include "Quest.hpp"

namespace rat
{
    class TextWidget;
    class QuestJournal : public Module<Window,GUI>
    {
        public:
            QuestJournal();
            ~QuestJournal();

            void addQuest(const unsigned int &i);
            void addStep(const unsigned int &i);

            void addDescription(const unsigned int &a,const unsigned int &b);

            void moveIterator(std::string questName);
        private:
            InterfaceWidget* _interface{nullptr};
            std::shared_ptr<FileLoader> _fileLoader;

            std::shared_ptr<StepsManager> _stepManager;
            std::shared_ptr<DescriptionManager> _descriptionManager;
            std::shared_ptr<QuestName> _questName;

            std::vector<std::shared_ptr<Quest> > _quests;
            std::vector<std::shared_ptr<Quest> >::iterator it;
            
    };
}


