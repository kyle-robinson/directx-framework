#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <queue>

class Keyboard
{
public:
	class KeyboardEvent
	{
	public:
		enum EventType
		{
			Press,
			Release,
			Invalid
		};
		KeyboardEvent();
		KeyboardEvent( const EventType type, const unsigned char key );
		bool IsPress() const noexcept;
		bool IsRelease() const noexcept;
		bool IsValid() const noexcept;
		unsigned char GetKeyCode() const noexcept;
	private:
		EventType type;
		unsigned char key;
	};
public:
	Keyboard();
	bool KeyIsPressed( const unsigned char keycode );
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed( const unsigned char key );
	void OnKeyReleased( const unsigned char key );
	void OnChar( const unsigned char key );
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();
private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

#endif