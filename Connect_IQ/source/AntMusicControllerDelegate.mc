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
    
    // Next page button pressed: opens a menu for music options
    function onNextPage() {
        Ui.pushView(new Rez.Menus.MusicControlMenu(), new MusicControlMenuDelegate(antChannel), Ui.SLIDE_UP);
        return true;
    }
    
    // Back button pressed: close the ANT channel before exiting
    function onBack() {
        antChannel.close();
    }
}