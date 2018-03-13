// ControllerView.mc
// Description: The view for showing music controls.
// By: Ivan Chow
// Date: March 12, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

// ENUM of the music control item to display
enum {
    CONTROL_VIEW_TOGGLE_PLAY_PAUSE_INDEX = 0,
    CONTROL_VIEW_PREV_SONG_INDEX,
    CONTROL_VIEW_NEXT_SONG_INDEX,
    
    CONTROL_VIEW_NUM_ITEMS
}

class ControllerView extends Ui.View {

    hidden var itemCounter; // Reference to a counter object

    function initialize(refCounter) {
        View.initialize();
        itemCounter = refCounter;
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        // Show a different music control depending on what the counter is at
        // Previous and next page buttons scroll between this index (see controller delegate)
        var controlIndex = itemCounter.getCount();
        
        dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
        
        // Display play/pause icon
        if(controlIndex == CONTROL_VIEW_TOGGLE_PLAY_PAUSE_INDEX) {
            dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_MEDIUM, "Play / Pause", Gfx.TEXT_JUSTIFY_CENTER);
        }
        
        // Display previous song icon
        else if(controlIndex == CONTROL_VIEW_PREV_SONG_INDEX) {
            dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_MEDIUM, "Previous Song", Gfx.TEXT_JUSTIFY_CENTER);
        }
        
        // Display next song icon
        else if(controlIndex == CONTROL_VIEW_NEXT_SONG_INDEX) {
            dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_MEDIUM, "Next Song", Gfx.TEXT_JUSTIFY_CENTER);
        }  
    }
}