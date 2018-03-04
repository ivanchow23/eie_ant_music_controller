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