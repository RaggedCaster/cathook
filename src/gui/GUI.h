/*
 * GUI.h
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#ifndef GUI_H_
#define GUI_H_

class IWidget;
class CatVar;

#define GUI_ENABLED true

#include "../beforecheaders.h"
#include <string>
#include "../aftercheaders.h"

#include "ncc/Root.hpp"
#include "ncc/Menu.hpp"

#include "../fixsdk.h"
#include "../inputsystem/ButtonCode.h"

class CTooltip;
class RootWindow;

extern CatVar gui_color_r;
extern CatVar gui_color_g;
extern CatVar gui_color_b;
int GUIColor();

extern CatVar gui_visible;
extern CatVar gui_draw_bounds;

class CatGUI {
public:
	CatGUI();

	bool Visible();
	void Update();
	void Setup();
	bool ConsumesKey(ButtonCode_t key);

	IWidget* CurrentSystem();

	// TODO Probably use unique_ptr or shared_ptr?
	std::vector<IWidget*> systems {};

	int  last_scroll_value { 0 };
	bool keys_init { false };
	bool keys_pressed[ButtonCode_t::BUTTON_CODE_COUNT] { false };
	int  keys_pressed_frame[ButtonCode_t::BUTTON_CODE_COUNT] { 0 };
	int  mouse_x { 0 };
	int  mouse_y { 0 };
	int  mouse_dx { 0 };
	int  mouse_dy { 0 };
};

extern CatGUI* g_pGUI;

#endif /* GUI_H_ */
