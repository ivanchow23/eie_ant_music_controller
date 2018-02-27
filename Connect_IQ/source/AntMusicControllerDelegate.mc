using Toybox.WatchUi as Ui;

class AntMusicControllerDelegate extends Ui.BehaviorDelegate {

    hidden var antChannel; // Reference to an ANT channel

    function initialize() {
        BehaviorDelegate.initialize();
    }
    
    // Select button pressed: opens an ANT channel
    function onSelect() {
        antChannel = new AntChannel();
        return true;
    }
    
    function onNextPage() {
        Ui.pushView(new Rez.Menus.MusicControlMenu(), new MusicControlMenuDelegate(antChannel), Ui.SLIDE_UP);
        return true;
    }
}