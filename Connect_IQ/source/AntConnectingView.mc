// AntConnectingView.mc
// Description: A placeholder view to show ANT connection status.
// By: Ivan Chow
// Date: March 3, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

class AntConnectingView extends Ui.View {
    
    hidden var antChannel;  // Reference to an ANT channel
    hidden var itemCounter; // Reference to an item counter object
    
    function initialize(refChannel) {
    
        View.initialize();
        antChannel = refChannel;    
        itemCounter = new ItemCounter(CONTROL_VIEW_NUM_ITEMS - 1); // -1 so the counter treats this as an index
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        if( antChannel.isConnected() ) {
            dc.setColor(Gfx.COLOR_GREEN, Gfx.COLOR_TRANSPARENT);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_TINY, "Connected.", Gfx.TEXT_JUSTIFY_CENTER);
            Ui.switchToView(new ControllerView(itemCounter), new ControllerDelegate(antChannel, itemCounter), Ui.SLIDE_IMMEDIATE);
        }
        else {
            dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_TINY, "Connecting...", Gfx.TEXT_JUSTIFY_CENTER);
        }
    }
}