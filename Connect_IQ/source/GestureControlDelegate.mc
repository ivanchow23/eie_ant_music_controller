// GestureControlDelegate.mc
// Description: Handles input from the GestureControlView class.
// By: Ivan Chow
// Date: March 18, 2018

using Toybox.WatchUi as Ui;

class GestureControlDelegate extends Ui.BehaviorDelegate {

    hidden var antChannel;  // Reference to an ANT channel
    
    function initialize(refChannel) {
        BehaviorDelegate.initialize();
        antChannel = refChannel;
    }
    
    function onSelect() {
        antChannel.sendMessage(ANT_MSG_TOGGLE_PLAY_PAUSE);
        return true;
    }
    
    function onBack() {
        Ui.popView(Ui.SLIDE_IMMEDIATE);
        return true;
    }
}

