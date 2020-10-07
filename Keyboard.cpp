#include "Keyboard.h"

bool Keyboard::KeyIsPressed( unsigned int keycode ) const noexcept
{
	return keyStates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
	if ( keyBuffer.size() > 0u )
	{
		Keyboard::Event e = keyBuffer.front();
		keyBuffer.pop();
		return e;
	}
	return { };
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keyBuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if ( charBuffer.size() > 0u )
	{
		unsigned char charcode = charBuffer.front();
		charBuffer.pop();
		return charcode;
	}
	return 0u;
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	keyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
	autoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	autoRepeatEnabled = false;
}

bool Keyboard::AutoRepeatIsEnabled() const noexcept
{
	return autoRepeatEnabled;
}

void Keyboard::OnKeyPressed( unsigned int keycode ) noexcept
{
	keyStates[keycode] = true;
	keyBuffer.push( Keyboard::Event( Keyboard::Event::Type::Press, keycode ) );
	TrimBuffer( keyBuffer );
}

void Keyboard::OnKeyReleased( unsigned int keycode ) noexcept
{
	keyStates[keycode] = false;
	keyBuffer.push( Keyboard::Event( Keyboard::Event::Type::Release, keycode ) );
	TrimBuffer( keyBuffer );
}

void Keyboard::OnChar( char character ) noexcept
{
	charBuffer.push( character );
	TrimBuffer( charBuffer );
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer( std::queue<T>& buffer ) noexcept
{
	while ( buffer.size() < bufferSize )
		buffer.pop();
}