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

#ifndef UNCOVERTRANSITION_HPP
#define UNCOVERTRANSITION_HPP

#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/transitions/Transition.hpp>
#include <touchgfx/widgets/SnapshotWidget.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
class Container;
class Widget;

/**
 * @class CoverTransition CoverTransition.hpp touchgfx/transitions/CoverTransition.hpp
 *
 * @brief A Transition that slides from one screen to the next.
 *
 *        A Transition that slides the new screen over the previous.
 *
 * @tparam templateDirection Type of the template direction.
 *
 * @see Transition
 */
template <Direction templateDirection>
class UncoverTransition : public Transition
{
public:

    /**
    * @class FullSolidRect
    *
    * @brief A Widget that returns a solid rect of the same size
    *        as the application.
    */
    class FullSolidRect : public Widget
    {
    public:
        FullSolidRect() : Widget()
        {}

        virtual ~FullSolidRect() {}

        virtual Rect getSolidRect() const
        {
            return Rect(0U, 0U, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
        }

        virtual void draw(const Rect& area) const { }
    };

    /**
     * @fn UncoverTransition::UncoverTransition(const uint8_t transitionSteps = 20) : Transition(), handleTickCallback(this, &UncoverTransition::tickMoveDrawable), direction(templateDirection), animationSteps(transitionSteps), animationCounter(0), calculatedValue(0), movedToPos(0)
     *
     * @brief Constructor.
     *
     *        Constructor.
     *
     * @param transitionSteps Number of steps in the transition animation.
     */
    UncoverTransition(const uint8_t transitionSteps = 20)
        : Transition(),
          handleTickCallback(this, &UncoverTransition::tickMoveDrawable),
          snapshot(),
          snapshotPtr(&snapshot),
          direction(templateDirection),
          animationSteps(transitionSteps),
          animationCounter(0),
          calculatedValue(0),
          movedToPos(0),
          solid()
    {
//        snapshot.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
//        snapshot.makeSnapshot();
	        
        switch (direction)
        {
        case EAST:
            targetValue = -HAL::DISPLAY_WIDTH;
            break;
        case WEST:
            targetValue = HAL::DISPLAY_WIDTH;
            break;
        case NORTH:
            targetValue = HAL::DISPLAY_HEIGHT;
            break;
        case SOUTH:
            targetValue = -HAL::DISPLAY_HEIGHT;
            break;
        default:
            done = true;
            // Nothing to do here
            break;
        }

        //Ensure that the solid area covers the entire screen
        solid.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
    }

    /**
     * @fn virtual UncoverTransition::~UncoverTransition()
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~UncoverTransition()
    {
    }

    /**
     * @fn virtual void CoverTransition::handleTickEvent()
     *
     * @brief Handles the tick event when transitioning.
     *
     *        Handles the tick event when transitioning. It moves the
     *        contents of the Screen's container. The direction of the
     *        transition determines the direction the contents of the
     *        container moves.
     */
    virtual void handleTickEvent()
    {
        Transition::handleTickEvent();

        // Calculate new position or stop animation
        animationCounter++;
        if (animationCounter <= animationSteps)
        {
            // Calculate value in [0;targetValue]
            calculatedValue = EasingEquations::cubicEaseOut(animationCounter, 0, targetValue, animationSteps);

            // Note: Result of "calculatedValue & 1" is compiler dependent for negative values of calculatedValue
            if (calculatedValue % 2)
            {
                // Optimization: calculatedValue is odd, add 1/-1 to move drawables modulo 32 bits in framebuffer
                calculatedValue += (calculatedValue > 0 ? 1 : -1);
            }
        }
        else
        {
            // Final step: stop the animation
            done = true;
            animationCounter = 0;
            return;
        }
//
//        // Move snapshot
//        switch (direction)
//        {
//        case EAST:
//        case WEST:
//            // Convert to delta value relative to current X
//            calculatedValue -= snapshot.getX();
//            snapshot.moveRelative(calculatedValue, 0);
//            break;
//        case NORTH:
//        case SOUTH:
//            // Convert to delta value relative to current Y
//            calculatedValue -= snapshot.getY();
//            snapshot.moveRelative(0, calculatedValue);
//            break;
//        default:
//            done = true;
//            // Nothing to do here
//            break;
//        }
	    
        // Get the currently displayed framebuffer
        uint16_t* tftFb = HAL::getInstance()->getTFTFrameBuffer();

        Rect source;
        source.x = 0;
        source.y = 0;
        source.width = HAL::DISPLAY_WIDTH;
        source.height = HAL::DISPLAY_HEIGHT-calculatedValue;

        //Copy rect from tft to client framebuffer
        //HAL::getInstance()->lcd().blitCopy((const uint16_t*)tftFb, source, rect, 255, false);
	    
        // Move children with delta value for X or Y
        screenContainer->forEachChild(&handleTickCallback);
    }

    /**
     * @fn virtual void UncoverTransition::tearDown()
     *
     * @brief Tear down.
     *
     *        Tear down.
     *
     * @see Transition::tearDown()
     */
    virtual void tearDown()
    {
        screenContainer->remove(solid);
    }

    /**
     * @fn virtual void UncoverTransition::init()
     *
     * @brief Initializes this object.
     *
     *        Initializes this object.
     *
     * @see Transition::init()
     */
    virtual void init()
    {
        Transition::init();
        Callback<UncoverTransition, Drawable&> initCallback(this, &UncoverTransition::initMoveDrawable);
        screenContainer->forEachChild(&initCallback);
        screenContainer->add(solid);
    }

protected:

    /**
     * @fn virtual void UncoverTransition::initMoveDrawable(Drawable& d)
     *
     * @brief Moves the Drawable to its initial position.
     *
     *        Moves the Drawable to its initial position outside of
     *        the visible area.
     *
     * @param [in] d The Drawable to move.
     */
    virtual void initMoveDrawable(Drawable& d)
    {
        switch (direction)
        {
        case EAST:
            d.moveRelative(HAL::DISPLAY_WIDTH, 0);
            break;
        case WEST:
            d.moveRelative(-HAL::DISPLAY_WIDTH, 0);
            break;
        case NORTH:
            d.moveRelative(0, -HAL::DISPLAY_HEIGHT);
            break;
        case SOUTH:
            d.moveRelative(0, HAL::DISPLAY_HEIGHT);
            break;
        default:
            // Nothing to do here
            break;
        }
    }

    /**
     * @fn virtual void UncoverTransition::tickMoveDrawable(Drawable& d)
     *
     * @brief Moves the Drawable.
     *
     *        Moves the Drawable.
     *
     * @param [in] d The Drawable to move.
     */
    virtual void tickMoveDrawable(Drawable& d)
    {
        switch (direction)
        {
        case EAST:
        case WEST:
            d.moveRelative(calculatedValue, 0);
            break;
        case NORTH:
        case SOUTH:
            d.moveRelative(0, calculatedValue);
            break;
        default:
            // Special case, do not move. Class NoTransition can be used instead.
            done = true;
            break;
        }
    }

private:
    Callback<UncoverTransition, Drawable&> handleTickCallback;    ///< Callback used for tickMoveDrawable().

    Direction     direction;        ///< The direction of the transition.
    const uint8_t animationSteps;   ///< Number of steps the transition should move per complete animation.
    uint8_t       animationCounter; ///< Current step in the transition animation.
    int16_t       targetValue;      ///< The target value for the transition animation.
    int16_t       calculatedValue;  ///< The calculated X or Y value for the snapshot and the children.
    int16_t       movedToPos;
    FullSolidRect solid;            ///< A solid rect that covers the entire screen to avoid copying elements outside

    SnapshotWidget  snapshot;    ///< The SnapshotWidget that is moved when transitioning.
    SnapshotWidget* snapshotPtr; ///< Pointer pointing to the snapshot used in this transition.The snapshot pointer
};
} // namespace touchgfx
#endif // UNCOVERTRANSITION_HPP
