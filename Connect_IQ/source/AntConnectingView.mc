using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

class AntConnectingView extends Ui.View {
    
    function initialize(refChannel) {
        View.initialize();    
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        // TODO: IC - Placeholder text for connecting page
        dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_TINY, "Placeholder Page", Gfx.TEXT_JUSTIFY_CENTER);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_XTINY, "Press Next Page for Menu", Gfx.TEXT_JUSTIFY_CENTER);
    }
}