#include "Keyboard.h"

/*   KEYBOARD EVENT   */
Keyboard::KeyboardEvent::KeyboardEvent() : type( KeyboardEvent::EventType::Invalid ), key( 0u )
{ }

Keyboard::KeyboardEvent::KeyboardEvent( const EventType type, const unsigned char key ) : type( type ), key( key )
{ }

bool Keyboard::KeyboardEvent::IsPress() const noexcept
{
	return this->type == EventType::Press;
}

bool Keyboard::KeyboardEvent::IsRelease() const noexcept
{
	return this->type == EventType::Release;
}

bool Keyboard::KeyboardEvent::IsValid() const noexcept
{
	return this->type != EventType::Invalid;
}

unsigned char Keyboard::KeyboardEvent::GetKeyCode() const noexcept
{
	return this->key;
}

/*   KEYBOARD CLASS   */
Keyboard::Keyboard()
{
	for ( int i = 0; i < 256; i++ )
		this->keyStates[i] = false;
}

bool Keyboard::KeyIsPressed( const unsigned char keycode )
{
	return this->keyStates[keycode];
}

bool Keyboard::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

Keyboard::KeyboardEvent Keyboard::ReadKey()
{
	if ( this->keyBuffer.empty() )
	{
		return KeyboardEvent();
	}
	else
	{
		KeyboardEvent e = this->keyBuffer.front(); // get first event from queue
		this->keyBuffer.pop();
		return e;
	}
}

unsigned char Keyboard::ReadChar()
{
	if ( this->charBuffer.empty() )
	{
		return 0u;
	}
	else
	{
		unsigned char e = this->charBuffer.front();
		this->charBuffer.pop();
		return e;
	}
}

void Keyboard::OnKeyPressed( const unsigned char key )
{
	this->keyStates[key] = true;
	this->keyBuffer.push( KeyboardEvent( KeyboardEvent::EventType::Press, key ) );
}

void Keyboard::OnKeyReleased( const unsigned char key )
{
	this->keyStates[key] = false;
	this->keyBuffer.push( KeyboardEvent( KeyboardEvent::EventType::Release, key ) );
}

void Keyboard::OnChar( const unsigned char key )
{
	this->charBuffer.push( key );
}

void Keyboard::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars()
{
	this->autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars()
{
	this->autoRepeatChars = false;
}

bool Keyboard::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat()
{
	return this->autoRepeatChars;
}