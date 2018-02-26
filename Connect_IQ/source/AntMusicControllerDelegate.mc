using Toybox.WatchUi as Ui;

class AntMusicControllerDelegate extends Ui.BehaviorDelegate {

    function initialize() {
        BehaviorDelegate.initialize();
    }
    
    // Select button pressed: opens an ANT channel
    function onSelect() {
        var channel = new AntChannel();
        return true;
    }
}