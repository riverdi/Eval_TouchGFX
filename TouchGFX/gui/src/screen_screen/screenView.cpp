#include <gui/screen_screen/screenView.hpp>

screenView::screenView()
{

}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();
}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}

void screenView::handleTickEvent()
{
    // Make the gauge move from min to max value
    static int incr = 1;

    if (toggleButton1.getState())
    {
        if (gauge1.getValue() <= 1)
        {
            incr = +1;
        }
        else if (gauge1.getValue() >= 100)
        {
            incr = -1;
        }

        gauge1.setValue(gauge1.getValue() + incr);



        circleProgress1.setValue(gauge1.getValue() + incr);
    }

}
