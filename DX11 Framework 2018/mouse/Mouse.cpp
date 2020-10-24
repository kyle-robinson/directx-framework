#include "Mouse.h"

/*   MOUSE EVENT   */
Mouse::MouseEvent::MouseEvent() : type( EventType::Invalid ), x( 0 ), y( 0 )
{ }

Mouse::MouseEvent::MouseEvent( const EventType type, const int x, const int y ) : type( type ), x( x ), y( y )
{ }

bool Mouse::MouseEvent::IsValid() const noexcept
{
	return this->type != EventType::Invalid;
}

Mouse::MouseEvent::EventType Mouse::MouseEvent::GetType() const noexcept
{
	return this->type;
}

MousePoint Mouse::MouseEvent::GetPos() const noexcept
{
	return { this->x, this->y };
}

int Mouse::MouseEvent::GetPosX() const noexcept
{
	return this->x;
}

int Mouse::MouseEvent::GetPosY() const noexcept
{
	return this->y;
}

/*   MOUSE CLASS   */
void Mouse::OnLeftPressed( int x, int y ) noexcept
{
	this->isLeftDown = true;
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::LPress, x, y ) );
}

void Mouse::OnLeftReleased( int x, int y ) noexcept
{
	this->isLeftDown = false;
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::LRelease, x, y ) );
}

void Mouse::OnRightPressed( int x, int y ) noexcept
{
	this->isRightDown = true;
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::RPress, x, y ) );
}

void Mouse::OnRightReleased( int x, int y ) noexcept
{
	this->isRightDown = false;
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::RRelease, x, y ) );
}

void Mouse::OnMiddlePressed( int x, int y ) noexcept
{
	this->isMiddleDown = true;
	this->eventBuffer.push( MouseEvent( Mouse::MouseEvent::EventType::MPress, x, y ) );
}

void Mouse::OnMiddleReleased( int x, int y ) noexcept
{
	this->isMiddleDown = false;
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::MRelease, x, y ) );
}

void Mouse::OnWheelUp( int x, int y ) noexcept
{
	MouseEvent mouseEvent( MouseEvent::WheelUp, x, y );
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::WheelUp, x, y ) );
}

void Mouse::OnWheelDown( int x, int y ) noexcept
{
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::WheelDown, x, y ) );
}

void Mouse::OnMouseMove( int x, int y ) noexcept
{
	this->x = x;
	this->y = y;
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::Move, x, y ) );
}

void Mouse::OnMouseMoveRaw( int x, int y ) noexcept
{
	this->eventBuffer.push( MouseEvent( MouseEvent::EventType::RawMove, x, y ) );
}

bool Mouse::IsLeftDown() const noexcept
{
	return this->isLeftDown;
}

bool Mouse::IsRightDown() const noexcept
{
	return this->isRightDown;
}

bool Mouse::IsMiddleDown() const noexcept
{
	return this->isMiddleDown;
}

int Mouse::GetPosX() const noexcept
{
	return this->x;
}

int Mouse::GetPosY() const noexcept
{
	return this->y;
}

MousePoint Mouse::GetPos() const noexcept
{
	return { this->x, this->y };
}

bool Mouse::EventBufferIsEmpty() const noexcept
{
	return eventBuffer.empty();
}

Mouse::MouseEvent Mouse::ReadEvent() noexcept
{
	if ( this->eventBuffer.empty() )
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = this->eventBuffer.front();
		this->eventBuffer.pop();
		return e;
	}
}