#pragma once

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Scroll.hpp"

namespace rat
{

enum InputDevice_e { Keyboard = 0x10000, Mouse = 0x20000, Scroll = 0x30000 };

class InputCode
{
public:

	///
	InputCode() = default;

	///
	InputCode(const InputCode&) = default;

	///
	InputCode& operator = (const InputCode&) = default;

	///
	InputCode(InputCode&&) = default;

	///
	InputCode& operator = (InputCode&&) = default;

	///
	~InputCode() = default;

	///
	InputCode(Keyboard::Code_e code);

	///
	InputCode(Mouse::Code_e code);

	///
	InputCode(Scroll::Code_e code);

	///
	explicit InputCode(int data);

	///
	InputDevice_e getType() const;

	///
	int getID() const;

	///
	operator int () const;

	///
	int getData() const;

	///
	bool isValid() const;

private:

	int _data = 0;

};

}
