// AntMusicControllerDelegate.mc
// Description: Handles inputs from the AntMusicControllerView class.
// By: Ivan Chow
// Date: February 25, 2018

using Toybox.WatchUi as Ui;

class AntMusicControllerDelegate extends Ui.BehaviorDelegate {

    hidden var antChannel; // Reference to an ANT channel

    function initialize() {
        BehaviorDelegate.initialize();
    }
    
    // Select button pressed: Switch to a new page and open ANT channel
    function onSelect() {
        antChannel = new AntChannel();
        Ui.pushView(new AntConnectingView(antChannel), new AntConnectingDelegate(antChannel), Ui.SLIDE_IMMEDIATE);
        return true;
    }
}