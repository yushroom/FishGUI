#pragma once

#include "nanovg.h"

//struct NVGcontext;

void DrawTabButton(NVGcontext* ctx, const char* text, int x, int y, int w, int h, bool active);

void DrawSearchBox(NVGcontext* ctx, const char* text, float x, float y, float w, float h);

void DrawDropDown(NVGcontext* ctx, const char* text, float x, float y, float w, float h);

void DrawLabel(NVGcontext* ctx, const char* text, float x, float y, float w, float h, int alignment = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

void DrawEditBox(NVGcontext* ctx, const char* text, float x, float y, float w, float h);

void DrawCheckBox(NVGcontext* ctx, float x, float y, float w, float h);

void DrawButton(NVGcontext* ctx, int preicon, const char* text, float x, float y, float w, float h);

void DrawSlider(NVGcontext* ctx, float pos, float x, float y, float w, float h);

void DrawLine(NVGcontext* ctx, float x1, float y1, float x2, float y2);
void DrawRect(NVGcontext* ctx, float x, float y, float w, float h, const NVGcolor & color);
void DrawRoundedRect(NVGcontext* ctx, float x, float y, float w, float h, float r, const NVGcolor & color);

