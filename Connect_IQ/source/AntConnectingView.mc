using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

class AntConnectingView extends Ui.View {
    
    hidden var antChannel; // Reference to an ANT channel
    
    function initialize(refChannel) {
    
        View.initialize();
        antChannel = refChannel;    
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        if( antChannel.isConnected() ) {
            // TODO: IC - Replace this by switching to new view in future
            dc.setColor(Gfx.COLOR_GREEN, Gfx.COLOR_TRANSPARENT);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_TINY, "Connected.", Gfx.TEXT_JUSTIFY_CENTER);
        }
        else {
            dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_TINY, "Connecting...", Gfx.TEXT_JUSTIFY_CENTER);
        }
    }
}