#pragma once

#include "Keys.h"
#include "../../Common.h"

namespace nCine
{
	enum class ButtonName : int16_t
	{
		UNKNOWN = -1,
		A = 0,
		B,
		X,
		Y,
		BACK,
		GUIDE,
		START,
		LSTICK,
		RSTICK,
		LBUMPER,
		RBUMPER,
		DPAD_UP,
		DPAD_DOWN,
		DPAD_LEFT,
		DPAD_RIGHT,
		MISC1,
		PADDLE1,
		PADDLE2,
		PADDLE3,
		PADDLE4,
		// Analog axes converted to button presses
		LTRIGGER,
		RTRIGGER,

		COUNT
	};

	enum class AxisName : int16_t
	{
		UNKNOWN = -1,
		LX = 0,
		LY,
		RX,
		RY,
		LTRIGGER,
		RTRIGGER
	};

	/// A structure containing joystick hat values
	struct HatState
	{
		enum
		{
			CENTERED = 0,
			UP = 1,
			RIGHT = 2,
			DOWN = 4,
			LEFT = 8,
			RIGHT_UP = RIGHT | UP,
			RIGHT_DOWN = RIGHT | DOWN,
			LEFT_UP = LEFT | UP,
			LEFT_DOWN = LEFT | DOWN
		};
	};

	enum class TouchEventType
	{
		/// Called every time the first screen touch is made
		Down,
		/// Called every time the last screen touch is released
		Up,
		/// Called every time a screen touch is moved
		Move,
		/// Called every time a screen touch different than the first one is made
		PointerDown,
		/// Called every time a screen touch different than the last one is released
		PointerUp
	};

	/// Information about a screen touch event
	class TouchEvent
	{
	public:
		static constexpr unsigned int MaxPointers = 10;

		struct Pointer
		{
			int id;
			float x, y;
			float pressure;
		};

		TouchEvent()
			: count(0), actionIndex(-1) {}

		TouchEventType type;
		unsigned int count;
		int actionIndex;
		Pointer pointers[MaxPointers];

		inline int findPointerIndex(int pointerId) const
		{
			int pointerIndex = -1;
			for (unsigned int i = 0; i < count && i < MaxPointers; i++) {
				if (pointers[i].id == pointerId) {
					pointerIndex = i;
					break;
				}
			}
			return pointerIndex;
		}
	};

#if defined(DEATH_TARGET_ANDROID)
	/// Information about an accelerometer event
	class AccelerometerEvent
	{
	public:
		AccelerometerEvent()
			: x(0.0f), y(0.0f), z(0.0f) {}

		float x, y, z;
	};
#endif

	/// Information about mouse state
	class MouseState
	{
	public:
		/// Pointer position on the X axis
		int x;
		/// Pointer position on the Y axis
		int y;

		virtual bool isLeftButtonDown() const = 0;
		virtual bool isMiddleButtonDown() const = 0;
		virtual bool isRightButtonDown() const = 0;
		virtual bool isFourthButtonDown() const = 0;
		virtual bool isFifthButtonDown() const = 0;
	};

	/// Information about a mouse event
	class MouseEvent
	{
	public:
		/// Pointer position on the X axis
		int x;
		/// Pointer position on the Y axis
		int y;

		virtual bool isLeftButton() const = 0;
		virtual bool isMiddleButton() const = 0;
		virtual bool isRightButton() const = 0;
		virtual bool isFourthButton() const = 0;
		virtual bool isFifthButton() const = 0;
	};

	/// Information about a scroll event (mouse wheel, touchpad gesture, etc.)
	class ScrollEvent
	{
	public:
		/// Scroll offset on the X axis
		float x;
		/// Scroll offset on the Y axis
		float y;
	};

	/// Information about keyboard state
	class KeyboardState
	{
	public:
		/// Returns 'true' if the specified key is down
		virtual bool isKeyDown(KeySym key) const = 0;
	};

	/// Information about a keyboard event
	class KeyboardEvent
	{
	public:
		/// Key scan code
		int scancode;
		/// Key symbol code
		KeySym sym;
		/// Key modifiers mask
		int mod;

		KeyboardEvent()
			: scancode(0), sym(KeySym::UNKNOWN), mod(0) {}
	};

	/// Information about a text input event
	class TextInputEvent
	{
	public:
		/// Unicode code point encoded in UTF-8
		char text[5];

		TextInputEvent()
		{
			text[0] = '\0';
		}
	};

	/// Information about the joystick state
	class JoystickState
	{
	public:
		virtual ~JoystickState() { }

		/// Returns 'true' if the specified button is pressed
		virtual bool isButtonPressed(int buttonId) const = 0;
		/// Returns the state of the specified hat
		virtual unsigned char hatState(int hatId) const = 0;
		/// Returns a normalized value between -1.0 and 1.0 for a joystick axis
		virtual float axisValue(int axisId) const = 0;
	};

	/// Information about a joystick button event
	class JoyButtonEvent
	{
	public:
		/// Joystick id
		int joyId;
		/// Button id
		int buttonId;
	};

	/// Information about a joystick hat event
	class JoyHatEvent
	{
	public:
		/// Joystick id
		int joyId;
		/// Hat id
		int hatId;
		/// Hat position state
		unsigned char hatState;
	};

	/// Information about a joystick axis event
	class JoyAxisEvent
	{
	public:
		/// Joystick id
		int joyId;
		/// Axis id
		int axisId;
		/// Axis value normalized between -1.0f and 1.0f
		float value;
	};

	/// Information about a joystick connection event
	class JoyConnectionEvent
	{
	public:
		/// Joystick id
		int joyId;
	};

	/// Information about a mapped joystick state
	class JoyMappedState
	{
	public:
		/// The number of joystick buttons with a mapping name
		static constexpr unsigned int NumButtons = (int)ButtonName::COUNT;
		/// The number of joystick axes with a mapping name
		static constexpr unsigned int NumAxes = 6;

		virtual ~JoyMappedState() {}

		/// Returns 'true' if the specified button is pressed
		virtual bool isButtonPressed(ButtonName name) const = 0;
		/// Returns the value of the specified axis
		virtual float axisValue(AxisName name) const = 0;
	};

	/// Information about a joystick mapped button event
	class JoyMappedButtonEvent
	{
	public:
		/// Joystick id
		int joyId;
		/// Button name
		ButtonName buttonName;
	};

	/// Information about a joystick mapped axis event
	class JoyMappedAxisEvent
	{
	public:
		/// Joystick id
		int joyId;
		/// Axis name
		AxisName axisName;
		/// Axis value between its minimum and maximum
		float value;
	};

}
