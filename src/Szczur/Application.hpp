#pragma once
 
#include "Szczur/Utility/Time/Clock.hpp"
#include "Szczur/Modules/Input/Input.hpp"
#include "Szczur/Modules/Window/Window.hpp"
#include "Szczur/Modules/PrepScreen/PrepScreen.hpp"
#include "Szczur/Utility/Modules/ModulesHolder.hpp"
#include "Szczur/Modules/GUI/GUI.hpp"
//#include "Szczur/Modules/GUITest/GUITest.hpp"
#include "Szczur/Modules/Script/Script.hpp"
namespace rat
{

class Application
{
public:

	Application() = default;

	Application(const Application&) = delete;

	Application& operator = (const Application&) = delete;

	Application(Application&&) = delete;

	Application& operator = (Application&&) = delete;

	int run();

	void init();

	void input();

	void update();

	void render();

	template <typename U, typename... Us>
	void initModule(Us&&... args);

	template <typename U>
	U& getModule();
	template <typename U>
	const U& getModule() const;

private:

	ModulesHolder<Window, Input, Script, GUI, PrepScreen> _modules;
	Clock _mainClock;
};

}
#include "Application.tpp"