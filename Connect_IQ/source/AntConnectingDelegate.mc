// AntConnectingDelegate.mc
// Description: Handles input from the ANT connection view.
// By: Ivan Chow
// Date: February 26, 2018

using Toybox.WatchUi as Ui;

class AntConnectingDelegate extends Ui.BehaviorDelegate {

    hidden var antChannel; // Reference to an ANT channel

    function initialize(refChannel) {
    
        BehaviorDelegate.initialize();
        antChannel = refChannel;
    }
    
    // Back button pressed: Close and release the ANT channel and go back to main menu
    function onBack() {
    
        antChannel.release();
        Ui.popView(Ui.SLIDE_IMMEDIATE);
        return true;
    }
}