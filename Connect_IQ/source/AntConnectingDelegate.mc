using Toybox.WatchUi as Ui;

class AntConnectingDelegate extends Ui.BehaviorDelegate {

    hidden var antChannel; // Reference to an ANT channel

    function initialize(refChannel) {
    
        BehaviorDelegate.initialize();
        antChannel = refChannel;
    }
    
    // Back button pressed: Close the ANT channel and go back to main menu
    function onBack() {
    
        antChannel.close();
        Ui.popView(Ui.SLIDE_IMMEDIATE);
        return true;
    }
    
    // Next button pressed: Open menu options
    function onNextPage() {
    
        Ui.pushView(new Rez.Menus.MusicControlMenu(), new MusicControlMenuDelegate(antChannel), Ui.SLIDE_UP);
        return true;
    }
}