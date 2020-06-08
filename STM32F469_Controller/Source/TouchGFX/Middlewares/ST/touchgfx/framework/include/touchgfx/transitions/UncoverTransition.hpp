
#ifndef UNCOVERTRANSITION_HPP
#define UNCOVERTRANSITION_HPP

#include <touchgfx/transitions/SlideTransition.hpp>

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
class UncoverTransition : public SlideTransition<templateDirection>
{
public:

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
        : SlideTransition<templateDirection>(transitionSteps)
    {
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
	    // Nothing to do.  We move the snapshot off of the drawable.  The drawable doesn't move.
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
	    // Nothing to do.  We move the snapshot off of the drawable.  The drawable doesn't move.
    }
	
private:
};
} // namespace touchgfx
#endif // UNCOVERTRANSITION_HPP



