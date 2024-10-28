#include "ui.h"
#include "gf2d_draw.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gfc_input.h"
#include "physics.h"
#include "simple_logger.h"

// ui system adapted from ui system made for 2d game programming
// https://github.com/AlaraBread/2d-game-dev-class/blob/midterm/src/entities/ui_element.c

unsigned int g_maxUIElements;
unsigned int g_lastAllocatedUIElement;
UIElement *g_UIElements;

void freeUISystem();

void initUISystem(unsigned int maxElements) {
	g_maxUIElements = maxElements;
	g_UIElements = calloc(sizeof(UIElement), maxElements);
	g_lastAllocatedUIElement = maxElements - 1;
	atexit(freeUISystem);
}

void freeUIElement(UIElement *ent);

void freeUISystem() {
	for(int i = 0; i < g_maxUIElements; i++)
		freeUIElement(&g_UIElements[i]);
	free(g_UIElements);
}

UIElement *newUIElement() {
	for(int i = (g_lastAllocatedUIElement + 1) % g_maxUIElements; i != g_lastAllocatedUIElement;
		i = (i + 1) % g_maxUIElements) {
		if(!g_UIElements[i].inuse) {
			g_lastAllocatedUIElement = i;
			UIElement *entity = &g_UIElements[i];
			memset(entity, 0, sizeof(UIElement));
			entity->inuse = true;
			entity->new = true;
			return entity;
		}
	}
	return NULL;
}

void clearUI() {
	for(int i = 0; i < g_maxUIElements; i++)
		freeUIElement(&g_UIElements[i]);
	memset(g_UIElements, 0, sizeof(UIElement) * g_maxUIElements);
}

void clearUIGroup(int group) {
	for(int i = 0; i < g_maxUIElements; i++) {
		UIElement *ent = &g_UIElements[i];
		if(ent->group == group) { freeUIElement(ent); }
	}
}

void freeUIElement(UIElement *ent) {
	if(!ent->inuse) { return; }
	if(ent->cleanup) { ent->cleanup(ent); }
	ent->inuse = false;
}

Bool rectTestUI(UIElement *ent, int x, int y) {
	return x > ent->position.x && x < ent->position.x + ent->size.x && y > ent->position.y &&
		   y < ent->position.y + ent->size.y;
}

void processMouseEventsUIElement(UIElement *element) {
	if(!element->mouseEnter && !element->mouseExit) { return; }
	GFC_Vector2D mouse = gfc_input_get_mouse_position();
	GFC_Vector2D prevMouse = gfc_input_get_prev_mouse_position();
	Bool is_in_rect = rectTestUI(element, mouse.x, mouse.y);
	Bool was_in_rect = rectTestUI(element, prevMouse.x, prevMouse.y);
	if(element->new) {
		was_in_rect = false;
		element->new = false;
	}

	if(element->mouseEnter && is_in_rect && !was_in_rect) { element->mouseEnter(element); }
	if(element->mouseExit && !is_in_rect && was_in_rect) {
		if(element->clicked && element->mouseUp) { element->mouseUp(element); }
		element->mouseExit(element);
		element->clicked = false;
	}
	if(is_in_rect && (gf2d_mouse_button_held(0))) {
		element->clicked = true;
		if(element->mouseDown) { element->mouseDown(element); }
	}
	if(is_in_rect && !(gf2d_mouse_button_held(0)) && element->clicked) {
		element->clicked = false;
		if(element->mouseUp) element->mouseUp(element);
		if(element->click) { element->click(element); }
	}
}

void uiFrame(double delta) {
	for(int i = 0; i < g_maxUIElements; i++) {
		UIElement *ent = &g_UIElements[i];
		if(!ent->inuse) { continue; }
		processMouseEventsUIElement(ent);
		if(ent->think) { ent->think(ent, delta * getTimeScale()); }
	}
	for(int i = 0; i < g_maxUIElements; i++) {
		UIElement *ent = &g_UIElements[i];
		if(!ent->inuse) { continue; }
		if(ent->update) { ent->update(ent, delta * getTimeScale()); }
		if(!ent->inuse) { continue; }
		if(ent->draw) { ent->draw(ent); }
	}
}

void buttonMouseEnter(UIElement *button) { button->bgColor = gfc_color(0.2, 0.2, 0.2, 1.0); }

void buttonMouseExit(UIElement *button) { button->bgColor = gfc_color(0.1, 0.1, 0.1, 1.0); }

void buttonMouseDown(UIElement *button) { button->bgColor = gfc_color(0.0, 0.0, 0.0, 1.0); }

void buttonMouseUp(UIElement *button) { button->bgColor = gfc_color(0.2, 0.2, 0.2, 1.0); }

void drawTextRect(UIElement *ent) {
	GFC_Rect rect = gfc_rect(ent->position.x, ent->position.y, ent->size.x, ent->size.y);
	gf2d_draw_rect_filled(rect, ent->bgColor);
	int borderThickness = 4;
	gf2d_draw_rect(rect, ent->borderColor, borderThickness);
	if(ent->text[0] != 0) {
		rect = gfc_rect(
			ent->position.x + borderThickness, ent->position.y + borderThickness, ent->size.x - borderThickness * 2,
			ent->size.y - borderThickness * 2
		);
		gf2d_font_draw_text_wrap_tag(ent->text, ent->fontSize, ent->color, rect);
	}
}

UIElement *createLabel(GFC_Vector2D position, GFC_Vector2D size) {
	UIElement *label = newUIElement();
	label->position = position;
	label->size = size;
	label->fontSize = FT_H1;
	label->color = gfc_color(1.0, 1.0, 1.0, 1.0);
	label->draw = drawTextRect;
	return label;
}

UIElement *createButton(GFC_Vector2D position, GFC_Vector2D size, const char *text) {
	UIElement *button = newUIElement();
	button->bgColor = gfc_color(0.1, 0.1, 0.1, 1.0);
	button->borderColor = gfc_color(1.0, 1.0, 1.0, 1.0);
	button->color = gfc_color(1.0, 1.0, 1.0, 1.0);
	button->draw = drawTextRect;
	button->mouseEnter = buttonMouseEnter;
	button->mouseExit = buttonMouseExit;
	button->mouseDown = buttonMouseDown;
	button->mouseUp = buttonMouseUp;
	button->position = gfc_vector2d(position.x - size.x / 2, position.y - size.y / 2);
	button->size = size;
	button->fontSize = 5;
	memcpy(button->text, text, strlen(text) + 1);
	return button;
}

UIElement *createPanel(GFC_Vector2D position, GFC_Vector2D size, GFC_Color color) {
	UIElement *panel = newUIElement();
	panel->position = position;
	panel->size = size;
	panel->bgColor = color;
	panel->draw = drawTextRect;
	return panel;
}
