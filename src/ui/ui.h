#ifndef __ALARA_UI_H__
#define __ALARA_UI_H__

#include "gf2d_font.h"
#include "gfc_color.h"
#include "gfc_text.h"
#include "gfc_vector.h"

typedef struct UIElement_S {
	Bool inuse;
	GFC_TextLine text;
	char filename[512];
	GFC_Color color;
	GFC_Color bgColor;
	GFC_Color borderColor;
	Bool clicked;
	GFC_Vector2D position;
	GFC_Vector2D size;
	FontTypes fontSize;
	int index;
	int group;
	Bool new;
	double timer;
	void (*think)(struct UIElement_S *self, double);
	void (*update)(struct UIElement_S *self, double);
	void (*draw)(struct UIElement_S *self);
	void (*cleanup)(struct UIElement_S *self);
	void (*mouseEnter)(struct UIElement_S *self);
	void (*mouseExit)(struct UIElement_S *self);
	void (*click)(struct UIElement_S *self);
	void (*mouseDown)(struct UIElement_S *self);
	void (*mouseUp)(struct UIElement_S *self);
} UIElement;

void initUISystem(unsigned int maxElements);
UIElement *newUIElement();
void freeUIElement(UIElement *ent);
void clearUI();
void clearUIGroup(int group);
void uiFrame(double delta);

void drawTextRect(UIElement *ent);

UIElement *createLabel(GFC_Vector2D position, GFC_Vector2D size);
UIElement *createButton(GFC_Vector2D position, GFC_Vector2D size, const char *text);
UIElement *createPanel(GFC_Vector2D position, GFC_Vector2D size, GFC_Color color);

#endif
