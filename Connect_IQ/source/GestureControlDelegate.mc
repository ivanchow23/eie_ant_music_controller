// GestureControlDelegate.mc
// Description: Handles input from the GestureControlView class.
// By: Ivan Chow
// Date: March 18, 2018

using Toybox.WatchUi as Ui;

class GestureControlDelegate extends Ui.BehaviorDelegate {

    function initialize() {
        BehaviorDelegate.initialize();
    }
    
    function onBack() {
        Ui.popView(Ui.SLIDE_IMMEDIATE);
        return true;
    }
}

