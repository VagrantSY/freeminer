// Copyright (C) 2002-2012 Nikolaus Gebhardt
// Copyright (C) 2016 Nathanaël Courant:
//   Modified the functions to use EnrichedText instead of string.
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "static_text.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include <vector>
#include <string>
#include <iostream>
#include <IGUISkin.h>
#include <IGUIEnvironment.h>
#include <IGUIFont.h>
#include <IVideoDriver.h>
#include <rect.h>
#include <SColor.h>

#if USE_FREETYPE
	#include "cguittfont/xCGUITTFont.h"
#endif

#include "util/string.h"

namespace irr
{

#if USE_FREETYPE

namespace gui
{
//! constructor
StaticText::StaticText(const EnrichedString &text, bool border,
			IGUIEnvironment* environment, IGUIElement* parent,
			s32 id, const core::rect<s32>& rectangle,
			bool background)
: IGUIStaticText(environment, parent, id, rectangle),
	HAlign(EGUIA_UPPERLEFT), VAlign(EGUIA_UPPERLEFT),
	Border(border), OverrideColorEnabled(false), OverrideBGColorEnabled(false), WordWrap(false), Background(background),
	RestrainTextInside(true), RightToLeft(false),
	OverrideColor(video::SColor(101,255,255,255)), BGColor(video::SColor(101,210,210,210)),
	OverrideFont(0), LastBreakFont(0)
{
	#ifdef _DEBUG
	setDebugName("StaticText");
	#endif

	Text = text.c_str();
	cText = text;
	if (environment && environment->getSkin())
	{
		BGColor = environment->getSkin()->getColor(gui::EGDC_3D_FACE);
	}
}


//! destructor
StaticText::~StaticText()
{
	if (OverrideFont)
		OverrideFont->drop();
}

//! draws the element and its children
void StaticText::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> frameRect(AbsoluteRect);

	// draw background

	if (Background)
	{
		if ( !OverrideBGColorEnabled )	// skin-colors can change
			BGColor = skin->getColor(gui::EGDC_3D_FACE);

		driver->draw2DRectangle(BGColor, frameRect, &AbsoluteClippingRect);
	}

	// draw the border

	if (Border)
	{
		skin->draw3DSunkenPane(this, 0, true, false, frameRect, &AbsoluteClippingRect);
		frameRect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X);
	}

	// draw the text
	if (cText.size())
	{
		IGUIFont* font = getActiveFont();

		if (font)
		{
			if (!WordWrap)
			{
				// TODO: add colors here
				if (VAlign == EGUIA_LOWERRIGHT)
				{
					frameRect.UpperLeftCorner.Y = frameRect.LowerRightCorner.Y -
						font->getDimension(L"A").Height - font->getKerningHeight();
				}
				if (HAlign == EGUIA_LOWERRIGHT)
				{
					frameRect.UpperLeftCorner.X = frameRect.LowerRightCorner.X -
						font->getDimension(cText.c_str()).Width;
				}

				irr::gui::CGUITTFont *tmp = static_cast<irr::gui::CGUITTFont*>(font);
				tmp->draw(cText, frameRect,
					OverrideColorEnabled ? OverrideColor : skin->getColor(isEnabled() ? EGDC_BUTTON_TEXT : EGDC_GRAY_TEXT),
					HAlign == EGUIA_CENTER, VAlign == EGUIA_CENTER, (RestrainTextInside ? &AbsoluteClippingRect : NULL));
			}
			else
			{
				if (font != LastBreakFont)
					breakText();

				core::rect<s32> r = frameRect;
				s32 height = font->getDimension(L"A").Height + font->getKerningHeight();
				s32 totalHeight = height * BrokenText.size();
				if (VAlign == EGUIA_CENTER)
				{
					r.UpperLeftCorner.Y = r.getCenter().Y - (totalHeight / 2);
				}
				else if (VAlign == EGUIA_LOWERRIGHT)
				{
					r.UpperLeftCorner.Y = r.LowerRightCorner.Y - totalHeight;
				}

				irr::video::SColor previous_color(255, 255, 255, 255);
				for (u32 i=0; i<BrokenText.size(); ++i)
				{
					if (HAlign == EGUIA_LOWERRIGHT)
					{
						r.UpperLeftCorner.X = frameRect.LowerRightCorner.X -
							font->getDimension(BrokenText[i].c_str()).Width;
					}

					std::vector<irr::video::SColor> colors;
					//std::wstring str;
					EnrichedString str = BrokenText[i];

					str = colorizeText(BrokenText[i].c_str(), colors, previous_color);
					if (!colors.empty())
						previous_color = colors[colors.size() - 1];

					irr::gui::CGUITTFont *tmp = static_cast<irr::gui::CGUITTFont*>(font);
					tmp->draw(str, r,
						previous_color, // FIXME
						HAlign == EGUIA_CENTER, false, (RestrainTextInside ? &AbsoluteClippingRect : NULL));

					r.LowerRightCorner.Y += height;
					r.UpperLeftCorner.Y += height;
				}
			}
		}
	}

	IGUIElement::draw();
}


//! Sets another skin independent font.
void StaticText::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont == font)
		return;

	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();

	breakText();
}

//! Gets the override font (if any)
IGUIFont * StaticText::getOverrideFont() const
{
	return OverrideFont;
}

//! Get the font which is used right now for drawing
IGUIFont* StaticText::getActiveFont() const
{
	if ( OverrideFont )
		return OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if (skin)
		return skin->getFont();
	return 0;
}

//! Sets another color for the text.
void StaticText::setOverrideColor(video::SColor color)
{
	OverrideColor = color;
	OverrideColorEnabled = true;
}


//! Sets another color for the text.
void StaticText::setBackgroundColor(video::SColor color)
{
	BGColor = color;
	OverrideBGColorEnabled = true;
	Background = true;
}


//! Sets whether to draw the background
void StaticText::setDrawBackground(bool draw)
{
	Background = draw;
}


//! Gets the background color
video::SColor StaticText::getBackgroundColor() const
{
	return BGColor;
}


//! Checks if background drawing is enabled
bool StaticText::isDrawBackgroundEnabled() const
{
#if defined(_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX)
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
#endif
	return Background;
}


//! Sets whether to draw the border
void StaticText::setDrawBorder(bool draw)
{
	Border = draw;
}


//! Checks if border drawing is enabled
bool StaticText::isDrawBorderEnabled() const
{
#if defined(_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX)
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
#endif
	return Border;
}


void StaticText::setTextRestrainedInside(bool restrainTextInside)
{
	RestrainTextInside = restrainTextInside;
}


bool StaticText::isTextRestrainedInside() const
{
	return RestrainTextInside;
}


void StaticText::setTextAlignment(EGUI_ALIGNMENT horizontal, EGUI_ALIGNMENT vertical)
{
	HAlign = horizontal;
	VAlign = vertical;
}


#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR <= 7
const video::SColor& StaticText::getOverrideColor() const
#else
video::SColor StaticText::getOverrideColor() const
#endif
{
	return OverrideColor;
}


//! Sets if the static text should use the overide color or the
//! color in the gui skin.
void StaticText::enableOverrideColor(bool enable)
{
	OverrideColorEnabled = enable;
}


bool StaticText::isOverrideColorEnabled() const
{

#if defined(_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX)
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
#endif
	return OverrideColorEnabled;
}


//! Enables or disables word wrap for using the static text as
//! multiline text control.
void StaticText::setWordWrap(bool enable)
{
	WordWrap = enable;
	breakText();
}


bool StaticText::isWordWrapEnabled() const
{
#if defined(_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX)
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
#endif
	return WordWrap;
}


void StaticText::setRightToLeft(bool rtl)
{
	if (RightToLeft != rtl)
	{
		RightToLeft = rtl;
		breakText();
	}
}


bool StaticText::isRightToLeft() const
{
	return RightToLeft;
}


//! Breaks the single text line.
void StaticText::breakText()
{
	if (!WordWrap)
		return;

	BrokenText.clear();

	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = getActiveFont();
	if (!font)
		return;

	LastBreakFont = font;

	EnrichedString line;
	EnrichedString word;
	EnrichedString whitespace;
	s32 size = cText.size();
	s32 length = 0;
	s32 elWidth = RelativeRect.getWidth();
	if (Border)
		elWidth -= 2*skin->getSize(EGDS_TEXT_DISTANCE_X);
	wchar_t c;

	//std::vector<irr::video::SColor> colors;

	// We have to deal with right-to-left and left-to-right differently
	// However, most parts of the following code is the same, it's just
	// some order and boundaries which change.
	if (!RightToLeft)
	{
		// regular (left-to-right)
		for (s32 i=0; i<size; ++i)
		{
			c = cText.getString()[i];
			bool lineBreak = false;

			if (c == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				//if (Text[i+1] == L'\n') // Windows breaks
				//{
				//	Text.erase(i+1);
				//	--size;
				//}
				c = '\0';
			}
			else if (c == L'\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			bool isWhitespace = (c == L' ' || c == 0);
			if ( !isWhitespace )
			{
				// part of a word
				//word += c;
				word.addChar(cText, i);
			}

			if ( isWhitespace || i == (size-1))
			{
				if (word.size())
				{
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const s32 whitelgth = font->getDimension(whitespace.c_str()).Width;
					//const std::wstring sanitized = removeEscapes(word.c_str());
					const s32 wordlgth = font->getDimension(word.c_str()).Width;

					if (wordlgth > elWidth)
					{
						// This word is too long to fit in the available space, look for
						// the Unicode Soft HYphen (SHY / 00AD) character for a place to
						// break the word at
						int where = core::stringw(word.c_str()).findFirst( wchar_t(0x00AD) );
						if (where != -1)
						{
							EnrichedString first = word.substr(0, where);
							EnrichedString second = word.substr(where, word.size() - where);
							first.addCharNoColor(L'-');
							BrokenText.push_back(line + first);
							const s32 secondLength = font->getDimension(second.c_str()).Width;

							length = secondLength;
							line = second;
						}
						else
						{
							// No soft hyphen found, so there's nothing more we can do
							// break to next line
							if (length)
								BrokenText.push_back(line);
							length = wordlgth;
							line = word;
						}
					}
					else if (length && (length + wordlgth + whitelgth > elWidth))
					{
						// break to next line
						BrokenText.push_back(line);
						length = wordlgth;
						line = word;
					}
					else
					{
						// add word to line
						line += whitespace;
						line += word;
						length += whitelgth + wordlgth;
					}

					word.clear();
					whitespace.clear();
				}

				if ( isWhitespace && c != 0)
				{
					whitespace.addChar(cText, i);
				}

				// compute line break
				if (lineBreak)
				{
					line += whitespace;
					line += word;
					BrokenText.push_back(line);
					line.clear();
					word.clear();
					whitespace.clear();
					length = 0;
				}
			}
		}

		line += whitespace;
		line += word;
		BrokenText.push_back(line);
	}
	else
	{
		// right-to-left
		for (s32 i=size; i>=0; --i)
		{
			c = cText.getString()[i];
			bool lineBreak = false;

			if (c == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				//if ((i>0) && Text[i-1] == L'\n') // Windows breaks
				//{
				//	Text.erase(i-1);
				//	--size;
				//}
				c = '\0';
			}
			else if (c == L'\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			if (c==L' ' || c==0 || i==0)
			{
				if (word.size())
				{
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const s32 whitelgth = font->getDimension(whitespace.c_str()).Width;
					const s32 wordlgth = font->getDimension(word.c_str()).Width;

					if (length && (length + wordlgth + whitelgth > elWidth))
					{
						// break to next line
						BrokenText.push_back(line);
						length = wordlgth;
						line = word;
					}
					else
					{
						// add word to line
						line = whitespace + line;
						line = word + line;
						length += whitelgth + wordlgth;
					}

					word.clear();
					whitespace.clear();
				}

				if (c != 0)
				//	whitespace = core::stringw(&c, 1) + whitespace;
				whitespace = cText.substr(i, 1) + whitespace;

				// compute line break
				if (lineBreak)
				{
					line = whitespace + line;
					line = word + line;
					BrokenText.push_back(line);
					line.clear();
					word.clear();
					whitespace.clear();
					length = 0;
				}
			}
			else
			{
				// yippee this is a word..
				//word = core::stringw(&c, 1) + word;
				word = cText.substr(i, 1) + word;
			}
		}

		line = whitespace + line;
		line = word + line;
		BrokenText.push_back(line);
	}
}


//! Sets the new caption of this element.
void StaticText::setText(const wchar_t* text)
{
	setText(EnrichedString(text));
}

//! Sets the new caption of this element.
void StaticText::setText(const EnrichedString &text)
{
	IGUIElement::setText(text.c_str());
	cText = text;
	if (text.hasBackground()) {
		setBackgroundColor(text.getBackground());
	}
	breakText();
}


void StaticText::updateAbsolutePosition()
{
	IGUIElement::updateAbsolutePosition();
	breakText();
}


//! Returns the height of the text in pixels when it is drawn.
s32 StaticText::getTextHeight() const
{
	IGUIFont* font = getActiveFont();
	if (!font)
		return 0;

	s32 height = font->getDimension(L"A").Height + font->getKerningHeight();

	if (WordWrap)
		height *= BrokenText.size();

	return height;
}


s32 StaticText::getTextWidth() const
{
	IGUIFont * font = getActiveFont();
	if(!font)
		return 0;

	if(WordWrap)
	{
		s32 widest = 0;

		for(u32 line = 0; line < BrokenText.size(); ++line)
		{
			s32 width = font->getDimension(BrokenText[line].c_str()).Width;

			if(width > widest)
				widest = width;
		}

		return widest;
	}
	else
	{
		return font->getDimension(cText.c_str()).Width;
	}
}


//! Writes attributes of the element.
//! Implement this to expose the attributes of your element for
//! scripting languages, editors, debuggers or xml serialization purposes.
void StaticText::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	IGUIStaticText::serializeAttributes(out,options);

	out->addBool	("Border",              Border);
	out->addBool	("OverrideColorEnabled",OverrideColorEnabled);
	out->addBool	("OverrideBGColorEnabled",OverrideBGColorEnabled);
	out->addBool	("WordWrap",		WordWrap);
	out->addBool	("Background",          Background);
	out->addBool	("RightToLeft",         RightToLeft);
	out->addBool	("RestrainTextInside",  RestrainTextInside);
	out->addColor	("OverrideColor",       OverrideColor);
	out->addColor	("BGColor",       	BGColor);
	out->addEnum	("HTextAlign",          HAlign, GUIAlignmentNames);
	out->addEnum	("VTextAlign",          VAlign, GUIAlignmentNames);

	// out->addFont ("OverrideFont",	OverrideFont);
}


//! Reads attributes of the element
void StaticText::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIStaticText::deserializeAttributes(in,options);

	Border = in->getAttributeAsBool("Border");
	enableOverrideColor(in->getAttributeAsBool("OverrideColorEnabled"));
	OverrideBGColorEnabled = in->getAttributeAsBool("OverrideBGColorEnabled");
	setWordWrap(in->getAttributeAsBool("WordWrap"));
	Background = in->getAttributeAsBool("Background");
	RightToLeft = in->getAttributeAsBool("RightToLeft");
	RestrainTextInside = in->getAttributeAsBool("RestrainTextInside");
	OverrideColor = in->getAttributeAsColor("OverrideColor");
	BGColor = in->getAttributeAsColor("BGColor");

	setTextAlignment( (EGUI_ALIGNMENT) in->getAttributeAsEnumeration("HTextAlign", GUIAlignmentNames),
                      (EGUI_ALIGNMENT) in->getAttributeAsEnumeration("VTextAlign", GUIAlignmentNames));

	// OverrideFont = in->getAttributeAsFont("OverrideFont");
}

} // end namespace gui

#endif // USE_FREETYPE

} // end namespace irr


#endif // _IRR_COMPILE_WITH_GUI_
