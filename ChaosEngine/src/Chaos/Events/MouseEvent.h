#pragma once
#include "Chaos/Events/Event.h"

namespace Chaos
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: mMouseX(x), mMouseY(y) {}

		inline float GetX() { return mMouseX; }
		inline float GetY() { return mMouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float mMouseX, mMouseY;
	};
	class MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float x, float y) : mScrollX(x), mScrollY(y) {}

		inline float GetScrollX() { return mScrollX; }
		inline float GetScrollY() { return mScrollY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << mScrollX << ", " << mScrollY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float mScrollX;
		float mScrollY;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return mButton; }

		EVENT_CLASS_TYPE(MouseButtonPressed)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(int button) : mButton(button) {}
		int mButton;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << mButton;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << mButton;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}