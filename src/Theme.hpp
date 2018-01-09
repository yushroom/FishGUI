#pragma once

#include "nanovg.h"

constexpr NVGcolor Color(int intensity, int alpha)
{
	return NVGcolor{{{intensity/255.0f, intensity/255.0f, intensity/255.0f, alpha/255.0f}}};
}

constexpr NVGcolor Color(int r, int g, int b, int alpha)
{
	return NVGcolor{{{r/255.0f, g/255.0f, b/255.0f, alpha/255.0f}}};
}

struct Theme
{
	int mStandardFontSize                 = 15;
	int mButtonFontSize                   = 20;
	int mTextBoxFontSize                  = 20;
	int mWindowCornerRadius               = 2;
	int mWindowHeaderHeight               = 18;
	int mWindowDropShadowSize             = 10;
	int mButtonCornerRadius               = 2;
	float mTabBorderWidth                 = 0.75f;
	int mTabInnerMargin                   = 5;
	int mTabMinButtonWidth                = 20;
	int mTabMaxButtonWidth                = 160;
	int mTabControlWidth                  = 20;
	int mTabButtonHorizontalPadding       = 10;
	int mTabButtonVerticalPadding         = 2;
	
	NVGcolor mDropShadow                       = Color(0, 128);
	NVGcolor mTransparent                      = Color(0, 0);
	NVGcolor mBorderDark                       = Color(29, 255);
	NVGcolor mBorderLight                      = Color(133, 255);
	NVGcolor mBorderMedium                     = Color(35, 255);
//	NVGcolor mTextColor                        = Color(255, 160);
	NVGcolor mTextColor = Color(0, 255);
	NVGcolor mDisabledTextColor                = Color(255, 80);
	NVGcolor mTextColorShadow                  = Color(0, 160);
	NVGcolor mIconColor                        = mTextColor;
	
	NVGcolor mButtonGradientTopFocused         = Color(64, 255);
	NVGcolor mButtonGradientBotFocused         = Color(48, 255);
	NVGcolor mButtonGradientTopUnfocused       = Color(74, 255);
	NVGcolor mButtonGradientBotUnfocused       = Color(58, 255);
	NVGcolor mButtonGradientTopPushed          = Color(41, 255);
	NVGcolor mButtonGradientBotPushed          = Color(29, 255);
	
	/* Window-related */
	NVGcolor mWindowBackgroundColor = Color(162, 255);
//	NVGcolor mWindowFocusedHeaderColor = Color(230, 255);
	NVGcolor mTabContentBackgroundColor = Color(194, 255);
	
	float mTabHeaderFontSize = 16.0f;
	int mTabHeaderCellWidth = 80;
	NVGcolor mTabHeaderActiveColor = mTabContentBackgroundColor;
	
	NVGcolor mWindowFillUnfocused              = Color(43, 230);
	NVGcolor mWindowFillFocused                = Color(45, 230);
	NVGcolor mWindowTitleUnfocused             = Color(220, 160);
	NVGcolor mWindowTitleFocused               = Color(255, 190);
	
	NVGcolor mWindowHeaderGradientTop          = mButtonGradientTopUnfocused;
	NVGcolor mWindowHeaderGradientBot          = mButtonGradientBotUnfocused;
	NVGcolor mWindowHeaderSepTop               = mBorderLight;
	NVGcolor mWindowHeaderSepBot               = mBorderDark;
	
	NVGcolor mWindowPopup                      = Color(50, 255);
	NVGcolor mWindowPopupTransparent           = Color(50, 0);
	
	NVGcolor mScrollBarColor					= Color(119, 137, 163, 255);
	NVGcolor mScrollBarBackColor				= Color(230, 255);
};

static Theme theme;
