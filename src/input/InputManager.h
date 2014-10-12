#ifndef INPUT_INPUTMANAGER_H_
#define INPUT_INPUTMANAGER_H_

#include <glm/glm.hpp>

#include "core/GloveCore.h"
#include "core/GloveObject.h"
#include "event/EventBus.h"
#include "event/EventSubscriber.h"
#include "event/type/KeyEvent.h"
#include "event/type/MouseButtonEvent.h"
#include "event/type/MouseMoveEvent.h"

namespace glove {

enum KeyState {
	/** The key is up */
	KS_UP,
	/** The key is down */
	KS_DOWN,
	/** The key has just been pushed down */
	KS_PRESSED,
	/** The key has just been released */
	KS_RELEASED
};

enum ButtonState {
	/** The key is up */
	BS_UP,
	/** The key is down */
	BS_DOWN,
	/** The key has just been pushed down */
	BS_PRESSED,
	/** The key has just been released */
	BS_RELEASED
};

class InputManager : public GloveObject, public EventSubscriber, public std::enable_shared_from_this < InputManager > {
	GLOVE_MEM_ALLOC_FUNCS("InputManager")
public:
	InputManager();
	virtual ~InputManager();

	virtual void OnKeyEvent(const KeyEvent& evnt);
	virtual void OnMouseButtonEvent(const MouseButtonEvent& evnt);
	virtual void OnMouseMoveEvent(const MouseMoveEvent& evnt);

	virtual void SyncUpdate();

	virtual const KeyState& GetKey(const KeyCode& key) const { return keyMap[key]; }
	virtual bool IsKeyDown(const KeyCode& key) const { return GetKey(key) == KS_DOWN; }
	virtual bool IsKeyPressed(const KeyCode& key) const { return GetKey(key) == KS_PRESSED; }
	virtual bool IsKeyReleased(const KeyCode& key) const { return GetKey(key) == KS_RELEASED; }

	virtual const ButtonState& GetButton(const MouseButton& button) const { return mouseButtonMap[button]; }
	virtual bool IsButtonDown(const MouseButton& button) const { return GetButton(button) == BS_DOWN; }
	virtual bool IsButtonPressed(const MouseButton& button) const { return GetButton(button) == BS_PRESSED; }
	virtual bool IsButtonyReleased(const MouseButton& button) const { return GetButton(button) == BS_RELEASED; }

	virtual glm::vec2 GetMousePosition() const { return mousePosition; }
	virtual const glm::vec2& GetMousePositionRef() const { return mousePosition; }

private:
	KeyState keyMap[KC_LAST];
	ButtonState mouseButtonMap[MB_LAST];

	glm::vec2 mousePosition;
};


} // namespace glove

#endif