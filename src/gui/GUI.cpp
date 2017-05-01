/*
 * GUI.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: nullifiedcat
 */

#include "GUI.h"
#include "IWidget.h"
#include "RootWindow.h"
#include "CTooltip.h"

#include "../common.h"
#include "../sdk.h"

void GUIVisibleCallback(IConVar* var, const char* pOldValue, float flOldValue) {
	g_IInputSystem->SetCursorPosition(draw::width / 2, draw::height / 2);
	g_ISurface->SetCursor(vgui::CursorCode::dc_none);
	g_ISurface->SetCursorAlwaysVisible(false);
	if (gui_visible) {
		g_ISurface->UnlockCursor();
	} else {
		g_ISurface->LockCursor();
	}
}

CatVar gui_visible(CV_SWITCH, "gui_visible", "0", "GUI Active", "GUI switch (bind it to a key!)");
CatVar gui_draw_bounds(CV_SWITCH, "gui_bounds", "0", "Draw Bounds", "Draw GUI elements' bounding boxes");

CatEnum gui_system_enum({"NCC", "LMAOBOX"});
CatVar gui_system(gui_system_enum, "gui_system", "0", "GUI Mode", "Select GUI mode");

CatGUI::CatGUI() : systems{} {}

bool CatGUI::Visible() {
	return gui_visible;
}

int gui_current_color = 0;

void UpdateColor(IConVar* var, const char* pOldValue, float flOldValue) {
	gui_current_color = colors::Create((int)gui_color_r, (int)gui_color_g, (int)gui_color_b, 255);
}

CatVar gui_color_r(CV_INT, "gui_color_r", "255", "Main GUI color (red)", "Defines red component of main gui color", 0, 255);
CatVar gui_color_g(CV_INT, "gui_color_g", "105", "Main GUI color (green)", "Defines green component of main gui color", 0, 255);
CatVar gui_color_b(CV_INT, "gui_color_b", "180", "Main GUI color (blue)", "Defines blue component of main gui color", 0, 255);

static CatVar gui_rainbow(CV_SWITCH, "gui_rainbow", "0", "Rainbow GUI", "RGB all the things!!!");

int GUIColor() {
	return gui_rainbow ? colors::RainbowCurrent() : gui_current_color;
}

void CatGUI::Setup() {
	auto InstallUpdateColorCallback = [](CatVar* var) {
		var->convar_parent->InstallChangeCallback(UpdateColor);
	};
	gui_color_r.OnRegister(InstallUpdateColorCallback);
	gui_color_g.OnRegister(InstallUpdateColorCallback);
	gui_color_b.OnRegister(InstallUpdateColorCallback);
	systems.push_back(new menu::ncc::Root());
	gui_visible.OnRegister([](CatVar* var) {
		var->convar_parent->InstallChangeCallback(GUIVisibleCallback);
	});
	gui_system.OnRegister([](CatVar* var) {
		var->convar_parent->InstallChangeCallback([](IConVar* var, const char* pOldValue, float flOldValue) {
			try {
				g_pGUI->systems.at((int)flOldValue)->Hide();
			} catch (...) {}
		});
	});
}

void CatGUI::Update() {
	try {
		IWidget* system = CurrentSystem();

		m_bShowTooltip = false;
		int new_scroll = g_IInputSystem->GetAnalogValue(AnalogCode_t::MOUSE_WHEEL);
		//logging::Info("scroll: %i", new_scroll);
		if (last_scroll_value < new_scroll) {
			// Scrolled up
			keys_pressed[ButtonCode_t::MOUSE_WHEEL_DOWN] = false;
			keys_pressed[ButtonCode_t::MOUSE_WHEEL_UP] = true;
		} else if (last_scroll_value > new_scroll) {
			// Scrolled down
			keys_pressed[ButtonCode_t::MOUSE_WHEEL_DOWN] = true;
			keys_pressed[ButtonCode_t::MOUSE_WHEEL_UP] = false;
		} else {
			// Didn't scroll
			keys_pressed[ButtonCode_t::MOUSE_WHEEL_DOWN] = false;
			keys_pressed[ButtonCode_t::MOUSE_WHEEL_UP] = false;
		}

		last_scroll_value = new_scroll;
		for (int i = 0; i < ButtonCode_t::BUTTON_CODE_COUNT; i++) {
			bool down = false, changed = false;;
			if (i != ButtonCode_t::MOUSE_WHEEL_DOWN && i != ButtonCode_t::MOUSE_WHEEL_UP) {
				down = g_IInputSystem->IsButtonDown((ButtonCode_t)(i));
				changed = keys_pressed[i] != down;
			} else {
				down = keys_pressed[i];
				changed = down;
			}
			if (changed && down) keys_pressed_frame[i] = g_GlobalVars->framecount;
			keys_pressed[i] = down;
			if (keys_init) {
				if (changed) {
					//logging::Info("Key %i changed! Now %i.", i, down);
					if (i == ButtonCode_t::MOUSE_LEFT) {
						if (Visible()) {
							if (down) root->OnMousePress();
							else root->OnMouseRelease();
						}
					} else {
						if (i == ButtonCode_t::KEY_INSERT && down) {
							gui_visible = !gui_visible;
						}
						if (Visible()) {
							if (down) root->OnKeyPress((ButtonCode_t)i, false);
							else root->OnKeyRelease((ButtonCode_t)i);
						}
					}
				} else {
					if (down) {
						int frame = g_GlobalVars->framecount - keys_pressed_frame[i];
						bool shouldrepeat = false;
						if (frame) {
							if (frame > 150) {
								if (frame > 400) {
									if (frame % 30 == 0) shouldrepeat = true;
								} else {
									if (frame % 80 == 0) shouldrepeat = true;
								}
							}
						}
						if (Visible()) {
							if (shouldrepeat) root->OnKeyPress((ButtonCode_t)i, true);
						}
					}
				}
			}
		}

		int nx = g_IInputSystem->GetAnalogValue(AnalogCode_t::MOUSE_X);
		int ny = g_IInputSystem->GetAnalogValue(AnalogCode_t::MOUSE_Y);

		mouse_dx = nx - mouse_x;
		mouse_dy = ny - mouse_y;

		mouse_x = nx;
		mouse_y = ny;

		if (!keys_init) keys_init = 1;
		if (!root->IsVisible())
			root->Show();
		root->Update();
		if (!m_bShowTooltip && m_pTooltip->IsVisible()) m_pTooltip->Hide();
		root->Draw(0, 0);
		if (Visible()) {
			draw::DrawRect(mouse_x - 5, mouse_y - 5, 10, 10, colors::Transparent(colors::white));
			draw::OutlineRect(mouse_x - 5, mouse_y - 5, 10, 10, GUIColor());
		}
		if (gui_draw_bounds) {
			root->DrawBounds(0, 0);
		}
		/*if (gui_visible) {
			if (!root->IsVisible())
				root->Show();
			root->Update();
			if (!m_bShowTooltip && m_pTooltip->IsVisible()) m_pTooltip->Hide();
			root->Draw(0, 0);
			draw::DrawRect(m_iMouseX - 5, m_iMouseY - 5, 10, 10, colors::Transparent(colors::white));
			draw::OutlineRect(m_iMouseX - 5, m_iMouseY - 5, 10, 10, GUIColor());
			if (gui_draw_bounds) {
				root->DrawBounds(0, 0);
			}
		} else {
			if (root->IsVisible())
				root->Hide();
		}*/
	} catch (std::exception& ex) {
		logging::Info("ERROR: %s", ex.what());
	}

}

bool CatGUI::ConsumesKey(ButtonCode_t key) {
	CBaseWindow* root = gui_nullcore ? dynamic_cast<CBaseWindow*>(root_nullcore) : dynamic_cast<CBaseWindow*>(m_pRootWindow);
	if (root->IsVisible())
		return root->ConsumesKey(key);
	else return false;
}

RootWindow* CatGUI::GetRootWindow() {
	return m_pRootWindow;
}

CatGUI* g_pGUI = 0;
