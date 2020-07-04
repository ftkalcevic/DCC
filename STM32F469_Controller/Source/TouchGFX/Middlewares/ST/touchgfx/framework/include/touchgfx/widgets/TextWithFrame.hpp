/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.13.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef TEXTWITHFRAME_HPP
#define TEXTWITHFRAME_HPP

#include <touchgfx/Font.hpp>
#include <touchgfx/FontManager.hpp>
#include <touchgfx/TypedText.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * @class TextWithFrame TextWithFrame.hpp touchgfx/widgets/TextWithFrame.hpp
 *
 * @brief Similar to TextArea, but will draw a solid rectangle underneath.  Also takes a string, not a resource.  Can disable.
 *
 */
class TextWithFrame : public Widget
{
public:
	TextWithFrame();

    virtual Rect getSolidRect() const
    {
	    if ( alpha == 255 )
            return getRect();
	    else
            return Rect(0, 0, 0, 0);
    }

    inline void setColor(colortype color)
    {
        this->color = color;
    }

    inline colortype getColor() const
    {
        return color;
    }

    inline void setBoxColor(colortype color)
    {
        this->boxColor = color;
    }

    inline colortype getBoxColor() const
    {
        return boxColor;
    }

    inline void setDisabledColor(colortype color)
    {
        this->disabledColor = color;
    }

    inline colortype getDisabledColor() const
    {
        return disabledColor;
    }

    inline void setDisabledBoxColor(colortype color)
    {
        this->disabledBoxColor = color;
    }

    inline colortype getDisabledBoxColor() const
    {
        return disabledBoxColor;
    }
    void setAlpha(uint8_t alpha)
    {
        this->alpha = alpha;
    }

    uint8_t getAlpha() const
    {
        return alpha;
    }
    void setFontId(FontId id)
    {
	    this->fontId = id;
    }

    uint8_t getFontId() const
    {
        return fontId;
    }

    void setAlignment(Alignment align)
    {
	    this->alignment = align;
    }

    uint8_t getAlignment() const
    {
        return alignment;
    }
	void setText(const char16_t *text)
	{
		this->text = (const Unicode::UnicodeChar *)text;
	}
	void setText(const Unicode::UnicodeChar *text)
	{
		this->text = text;
	}
	const Unicode::UnicodeChar *getText() const
	{
		return this->text;
	}
    virtual void setBaselineY(int16_t baselineY)
    {
		Font *font = FontManager::getFont(fontId);
		if (font != nullptr)
            setY(baselineY - font->getFontHeight());
    }

    virtual void setXBaselineY(int16_t x, int16_t baselineY)
    {
        setX(x);
        setBaselineY(baselineY);
    }

    inline void setLinespacing(int16_t space)
    {
        linespace = space;
    }

    inline int16_t getLinespacing() const
    {
        return linespace;
    }

    inline void setEnabled(bool e)
    {
        enabled = e;
    }

    inline bool getEnabled() const
    {
        return enabled;
    }
	
		
    /**
     * @fn inline void TextWithFrame::setIndentation(uint8_t indent)
     *
     * @brief Sets the indentation for the text.
     *
     *        Sets the indentation for the text. This is very useful when a font is an italic
     *        font where letters such as "j" and "g" extend a lot to the left under the
     *        previous characters. if a line starts with a "j" or "g" this letter would either
     *        have to be pushed to the right to be able to see all of it, e.g. using spaces
     *        which would ruin a multi line text which is left aligned. This could be solved by
     *        changing a TextWithFrame.setPosition(50,50,100,100) to TextWithFrame.setPosition(45,50,110,
     *        100) followed by a TextWithFrame.setIndentation(5). Characters that do not extend to
     *        the left under the previous characters will be drawn in the same position in
     *        either case, but "j" and "g" will be aligned with other lines.
     *
     *        The function getMaxPixelsLeft() will give you the maximum number of pixels any
     *        glyph in the font extends to the left.
     *
     * @param indent The indentation from left (when left aligned text) and right (when right
     *               aligned text).
     *
     * @see getMaxPixelsLeft
     */
    inline void setIndentation(uint8_t indent)
    {
        indentation = indent;
    }

    inline uint8_t getIndentation()
    {
        return indentation;
    }

    /**
     * @fn virtual int16_t TextWithFrame::getTextHeight();
     *
     * @brief Gets the total height needed by the text.
     *
     *        Gets the total height needed by the text, taking number of lines and line spacing
     *        into consideration.
     *
     * @return the total height needed by the text.
     */
    virtual int16_t getTextHeight();

    /**
     * @fn virtual uint16_t TextWithFrame::getTextWidth() const;
     *
     * @brief Gets the width in pixels of the current associated text in the current selected
     *        language.
     *
     *        Gets the width in pixels of the current associated text in the current selected
     *        language. In case of multi-lined text the width of the widest line is returned.
     *
     * @return The width in pixels of the current text.
     */
    virtual uint16_t getTextWidth() const;

    /**
     * @fn virtual void TextWithFrame::draw(const Rect& area) const;
     *
     * @brief Draws the text.
     *
     *        Draws the text. Called automatically.
     *
     * @param area The invalidated area.
     */
    virtual void draw(const Rect& area) const;

    /**
     * @fn void TextWithFrame::resizeToCurrentText();
     *
     * @brief Sets the dimensions of the TextWithFrame.
     *
     *        Sets the dimensions of the TextWithFrame to match the width and height of the current
     *        associated text for the current selected language.
     *
     * @see resizeHeightToCurrentText
     */
    void resizeToCurrentText();

    /**
     * @fn void TextWithFrame::resizeToCurrentTextWithAlignment();
     *
     * @brief Sets the dimensions of the TextWithFrame.
     *
     *        Sets the dimensions of the TextWithFrame to match the width and height of the current
     *        associated text for the current selected language.
     *
     *        When setting the width, the position of the TextWithFrame might be changed in order to
     *        keep the text centered or right aligned.
     *
     * @see resizeHeightToCurrentText
     */
    void resizeToCurrentTextWithAlignment();

    /**
     * @fn void TextWithFrame::resizeHeightToCurrentText();
     *
     * @brief Sets the height of the TextWithFrame.
     *
     *        Sets the height of the TextWithFrame to match the height of the current associated
     *        text for the current selected language. This is especially useful for texts with
     *        WordWrap enabled.
     *
     * @see resizeToCurrentText
     * @see setWordWrap
     */
    void resizeHeightToCurrentText();

    /**
     * @fn void TextWithFrame::setWideTextAction(WideTextAction action)
     *
     * @brief Sets wide text action.
     *
     *        Sets wide text action. Defines what to do if a line of text is wider than the
     *        text area. Default action is WIDE_TEXT_NONE which means that text lines are only
     *        broken if there is a newline in the text.
     *
     *        If wrapping is enabled and the text would occupy more lines than the size of the
     *        TextWithFrame, the last line will get an ellipsis to signal that some text is missing.
     *        The character used for ellipsis is taken from the text spreadsheet.
     *
     * @param action The action to perform for wide lines of text.
     *
     * @see WideTextAction
     * @see getWideTextAction
     * @see resizeHeightToCurrentText
     */
    void setWideTextAction(WideTextAction action)
    {
        wideTextAction = action;
    }

    WideTextAction getWideTextAction() const
    {
        return wideTextAction;
    }

    /**
     * @fn int16_t TextWithFrame::calculateTextHeight(const Unicode::UnicodeChar* format, ...) const;
     *
     * @brief Gets the total height needed by the text.
     *
     *        Gets the total height needed by the text. Determined by number of lines and
     *        linespace. The number of wildcards in the text should match the number of values
     *        for the wildcards.
     *
     * @param format The text containing %s wildcards.
     * @param ...    Variable arguments providing additional information.
     *
     * @return the total height needed by the text.
     */
    virtual int16_t calculateTextHeight(const Unicode::UnicodeChar* format, ...) const;

protected:
    Alignment      alignment;
    FontId         fontId;         
    colortype      color;
    colortype      boxColor;
    colortype      disabledColor; 
    colortype      disabledBoxColor; 
    uint8_t        alpha;               ///< The alpha to use for text and box.
	const Unicode::UnicodeChar *text;
	
    int16_t        linespace;      ///< The line spacing to use, in pixels, in case the text contains newlines.
    uint8_t        indentation;    ///< The indentation of the text inside the text area
    WideTextAction wideTextAction; ///< What to do if the text is wider than the text area
	bool           enabled;
};
} // namespace touchgfx

#endif // TEXTWITHFRAME_HPP
