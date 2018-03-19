// AccelDebugView.mc
// Description: The view for showing a debug screen with accelerometer values
// By: Ivan Chow
// Date: March 18, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

class AccelDebugView extends Ui.View {

    function initialize() {
        View.initialize();
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_SMALL, "Debug Page.", Gfx.TEXT_JUSTIFY_CENTER);
    }
}