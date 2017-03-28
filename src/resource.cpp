/*
 * resource.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: nullifiedcat
 */

#include "resource.hpp"
#include "common.h"

Texture::Texture(unsigned char* start, unsigned w, unsigned h)
	: start_addr(start), w(w), h(h) {}

Texture::~Texture() {
	if (id) {
		g_ISurface->DeleteTextureByID(id);
	}
}

void Texture::Load() {
	id = g_ISurface->CreateNewTextureID(true);
	g_ISurface->DrawSetTextureRGBA(id, start_addr, w, h, 0, 0);
}

void Texture::Draw(int x, int y, int sw, int sh) {
	if (!g_ISurface->IsTextureIDValid(id)) throw std::runtime_error("Invalid texture ID!");
	g_ISurface->DrawSetColor(255, 255, 255, 255);
	g_ISurface->DrawSetTexture(id);
	g_ISurface->DrawTexturedRect(x, y, sw, sh);
}
