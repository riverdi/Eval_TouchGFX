/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef SCREENVIEWBASE_HPP
#define SCREENVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/screen_screen/screenPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <touchgfx/widgets/ButtonWithIcon.hpp>
#include <touchgfx/widgets/ToggleButton.hpp>
#include <touchgfx/widgets/RadioButton.hpp>
#include <touchgfx/containers/Slider.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/RadioButtonGroup.hpp>

class screenViewBase : public touchgfx::View<screenPresenter>
{
public:
    screenViewBase();
    virtual ~screenViewBase() {}
    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::ButtonWithLabel buttonWithLabel2;
    touchgfx::ButtonWithIcon buttonWithIcon1;
    touchgfx::ToggleButton toggleButton1;
    touchgfx::RadioButton radioButton1;
    touchgfx::Slider slider1;
    touchgfx::Image image1;
    touchgfx::RadioButtonGroup<1> radioButtonGroup1;

private:

};

#endif // SCREENVIEWBASE_HPP
