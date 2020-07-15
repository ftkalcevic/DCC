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

#ifndef MODALBOXWINDOW_HPP
#define MODALBOXWINDOW_HPP

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/BoxWithBorder.hpp>
#include <touchgfx/widgets/Image.hpp>

namespace touchgfx
{
/**
 * @class   ModalBoxWindow ModalBoxWindow.hpp include/gui/common/ModalBoxWindow.hpp
 *
 * @brief   Like ModalWindow, but background is a BoxWithBorder
 */
class ModalBoxWindow : public Container
{
public:

    /**
     * @fn ModalBoxWindow::ModalBoxWindow();
     *
     * @brief Default constructor.
     *
     *        Default constructor.
     */
    ModalBoxWindow();

    /**
     * @fn virtual ModalBoxWindow::~ModalBoxWindow();
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~ModalBoxWindow();

	void setWindowSize(const uint16_t width, const uint16_t height);
	void setWindowColor(colortype color);
	void setWindowBorderColor(colortype color);
	void setWindowBorderSize(uint8_t size);
	
    /**
     * @fn virtual void ModalBoxWindow::add(Drawable& d);
     *
     * @brief Adds a drawable to the ModalBoxWindow.
     *
     *        Adds a drawable to the ModalBoxWindow. The drawable will be placed relative to the
     *        background image.
     *
     * @param [in] d The drawable to add.
     */
    virtual void add(Drawable& d);

    /**
     * @fn virtual void ModalBoxWindow::remove(Drawable& d);
     *
     * @brief Removes the drawable from the ModalBoxWindow.
     *
     *        Removes the drawable from the ModalBoxWindow.
     *
     * @param [in] d The drawable to remove.
     */
    virtual bool remove(Drawable& d);

    /**
     * @fn virtual void ModalBoxWindow::setShadeAlpha(uint8_t alpha);
     *
     * @brief Sets the alpha value of the background shade.
     *
     *        Sets the alpha value of the background shade. Default = 96.
     *
     * @param alpha The new alpha.
     */
    virtual void setShadeAlpha(uint8_t alpha);

    /**
     * @fn virtual uint8_t ModalBoxWindow::getShadeAlpha() const;
     *
     * @brief Gets the alpha value of the background shade.
     *
     *        Gets the alpha value of the background shade.
     *
     * @return The background shades alpha.
     */
    virtual uint8_t getShadeAlpha() const;

    /**
     * @fn virtual void ModalBoxWindow::setShadeColor(colortype color);
     *
     * @brief Sets the color of the background shade.
     *
     *        Sets the color of the background shade. Default = Black.
     *
     * @param color The new color.
     */
    virtual void setShadeColor(colortype color);

    /**
     * @fn virtual colortype ModalBoxWindow::getShadeColor() const;
     *
     * @brief Gets the color of the background shade.
     *
     *        Gets the color of the background shade.
     *
     * @return The color of the background shade.
     */
    virtual colortype getShadeColor() const;

    /**
     * @fn virtual void ModalBoxWindow::show();
     *
     * @brief Make the ModalBoxWindow visible.
     *
     *        Make the ModalBoxWindow visible.
     */
    virtual void show();

    /**
     * @fn virtual void ModalBoxWindow::hide();
     *
     * @brief Make the ModalBoxWindow invisible.
     *
     *        Make the ModalBoxWindow invisible.
     */
    virtual void hide();

    /**
     * @fn virtual bool ModalBoxWindow::isShowing();
     *
     * @brief Query if this ModalBoxWindow is showing.
     *
     *        Query if this ModalBoxWindow is showing.
     *
     * @return true if showing, false if not.
     */
    virtual bool isShowing() const;

protected:
    Box backgroundShade;        ///< The background shade
    Container windowContainer;  ///< The window container that defines the active container area where both the windowBackground and added drawables are placed.
    BoxWithBorder windowBox;     ///< The window background
	colortype windowColor;
	colortype windowBorderColor;
	uint8_t windowBorderSize;
};
}

#endif // MODALBOXWINDOW_HPP
