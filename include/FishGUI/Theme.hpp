#pragma once

#include "nanovg.h"

namespace FishGUI
{
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
		float standardFontSize               = 15;
		int buttonFontSize                   = 20;
		int textBoxFontSize                  = 20;
		int windowCornerRadius               = 2;
		int windowHeaderHeight               = 18;
		int windowDropShadowSize             = 10;
		int buttonCornerRadius               = 2;
		float tabBorderWidth                 = 0.75f;
		int tabInnerMargin                   = 5;
		int tabMinButtonWidth                = 20;
		int tabMaxButtonWidth                = 160;
		int tabControlWidth                  = 20;
		int tabButtonHorizontalPadding       = 10;
		int tabButtonVerticalPadding         = 2;
		
		int buttonHeight = 20;
		
		NVGcolor dropShadow                       = Color(0, 128);
		NVGcolor transparent                      = Color(0, 0);
		NVGcolor borderDark                       = Color(29, 255);
		NVGcolor borderLight                      = Color(133, 255);
		NVGcolor borderMedium                     = Color(35, 255);
	//	NVGcolor textColor                        = Color(255, 160);
		NVGcolor textColor = Color(0, 255);
		NVGcolor disabledTextColor                = Color(255, 80);
		NVGcolor textColorShadow                  = Color(0, 160);
		NVGcolor iconColor                        = textColor;
		
	//	NVGcolor buttonGradientTopFocused         = Color(64, 255);
	//	NVGcolor buttonGradientBotFocused         = Color(48, 255);
	//	NVGcolor buttonGradientTopUnfocused       = Color(74, 255);
	//	NVGcolor buttonGradientBotUnfocused       = Color(58, 255);
	//	NVGcolor buttonGradientTopPushed          = Color(41, 255);
	//	NVGcolor buttonGradientBotPushed          = Color(29, 255);

		NVGcolor buttonGradientTopFocused         = Color(64, 255);
		NVGcolor buttonGradientBotFocused         = Color(48, 255);
		NVGcolor buttonGradientTopUnfocused       = Color(240, 255);
		NVGcolor buttonGradientBotUnfocused       = Color(190, 255);
		NVGcolor buttonGradientTopPushed          = Color(41, 255);
		NVGcolor buttonGradientBotPushed          = Color(29, 255);
		
		/* Window-related */
		NVGcolor windowBackgroundColor = Color(162, 255);
	//	NVGcolor windowFocusedHeaderColor = Color(230, 255);
		NVGcolor tabContentBackgroundColor = Color(194, 255);
		
		float tabHeaderFontSize = 16.0f;
		int tabHeaderCellWidth = 80;
		NVGcolor tabHeaderActiveColor = tabContentBackgroundColor;
		
		NVGcolor windowFillUnfocused              = Color(43, 230);
		NVGcolor windowFillFocused                = Color(45, 230);
		NVGcolor windowTitleUnfocused             = Color(220, 160);
		NVGcolor windowTitleFocused               = Color(255, 190);
		
		NVGcolor windowHeaderGradientTop          = buttonGradientTopUnfocused;
		NVGcolor windowHeaderGradientBot          = buttonGradientBotUnfocused;
		NVGcolor windowHeaderSepTop               = borderLight;
		NVGcolor windowHeaderSepBot               = borderDark;
		
		NVGcolor windowPopup                      = Color(50, 255);
		NVGcolor windowPopupTransparent           = Color(50, 0);
		
		NVGcolor scrollBarColor					= Color(119, 137, 163, 255);
		NVGcolor scrollBarBackColor				= Color(230, 255);
		
		NVGcolor selectionHighlightColor 		= Color(62, 125, 231, 255);
		NVGcolor selectionColor 				= Color(143, 255);
		
		static Theme* GetDefaultTheme()
		{
			static Theme theme;
			return &theme;
		}
		
		static Theme* GetCurrent()
		{
			return s_current!=nullptr ? s_current : GetDefaultTheme();
		}
		
	private:
		static Theme* s_current;
	};
}
