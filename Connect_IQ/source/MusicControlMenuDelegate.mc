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
            antChannel.sendMessage(ANT_MSG_TOGGLE_PLAY_PAUSE);
        }
        
        else if(item == :PREV_SONG) { 
            antChannel.sendMessage(ANT_MSG_PREVIOUS_SONG);
        }
        
        else if(item == :NEXT_SONG) {
            antChannel.sendMessage(ANT_MSG_NEXT_SONG);
        }
    }
}