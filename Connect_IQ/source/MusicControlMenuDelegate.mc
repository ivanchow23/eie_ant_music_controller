using Toybox.WatchUi as Ui;

class MusicControlMenuDelegate extends Ui.MenuInputDelegate {

    hidden var antChannel; // Reference to an ANT channel

    function initialize(refChannel) {
        MenuInputDelegate.initialize();
        
        antChannel = refChannel;
    }
    
    // Handles a menu item selection
    function onMenuItem(item) {
    
        if(item == :TOGGLE_PLAY_PAUSE) {
            System.println("Toggle play/pause selected.");
        }
        
        else if(item == :PREV_SONG) { 
            System.println("Previous song selected.");
        }
        
        else if(item == :NEXT_SONG) {
            System.println("Next song selected.");
        }
    }
}