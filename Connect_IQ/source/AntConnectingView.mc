// AntConnectingView.mc
// Description: A placeholder view to show ANT connection status.
// By: Ivan Chow
// Date: March 3, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;
using Toybox.Timer as Timer;

class AntConnectingView extends Ui.View {
    
    hidden var antChannel;      // Reference to an ANT channel
    hidden var itemCounter;     // Reference to an item counter object
    hidden var switchViewTimer; 
    
    const VIEW_SWITCH_TIMER_MS = 3000; // Time until we switch to the next view
    
    function initialize(refChannel) {
    
        View.initialize();
        antChannel = refChannel;    
        itemCounter = new ItemCounter(CONTROL_VIEW_NUM_ITEMS - 1); // -1 so the counter treats this as an index
        
        switchViewTimer = new Timer.Timer();
        switchViewTimer.start(method(:timerHandler), VIEW_SWITCH_TIMER_MS, false);
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_TINY, "Connecting...", Gfx.TEXT_JUSTIFY_CENTER);
    }
    
    // Timer handler to switch to a new view after a the given time is up
    static function timerHandler() {
        Ui.switchToView(new ControllerView(itemCounter), new ControllerDelegate(antChannel, itemCounter), Ui.SLIDE_IMMEDIATE);
    }
}