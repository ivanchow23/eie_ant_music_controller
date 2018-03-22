// ControllerDelegate.mc
// Description: Handles input from the ControllerView class.
// By: Ivan Chow
// Date: March 12, 2018

using Toybox.WatchUi as Ui;

class ControllerDelegate extends Ui.BehaviorDelegate {

    hidden var antChannel;  // Reference to an ANT channel
    hidden var itemCounter; // Reference to an item counter object

    function initialize(refChannel, refCounter) {
        BehaviorDelegate.initialize();
        antChannel = refChannel;
        itemCounter = refCounter;
    }
    
    // Increment counter to show the next music control icon
    function onNextPage() {
        itemCounter.increment();
        Ui.requestUpdate();
        return true;
    }
    
    // Decrement counter to show the previous music control icon
    function onPreviousPage() {
        itemCounter.decrement();
        Ui.requestUpdate();
        return true;
    }
    
    // Acts as a menu item and selects the current item choice
    // Selected item will get sent as an ANT message
    function onSelect() {
        var count = itemCounter.getCount();
        
        // Move into gesture controls page if this is what is selected
        if(count == CONTROL_VIEW_GESTURE_CONTROLS_INDEX) {
            Ui.pushView(new GestureControlView(antChannel), new GestureControlDelegate(antChannel), Ui.SLIDE_IMMEDIATE);
            return true;
        }
        
        // Conveniently, the item counter should be perfectly mapped to the ANT send message method
        antChannel.sendMessage(count);
    }
    
    // Back to the main title page
    function onBack() {
        antChannel.release();
        Ui.popView(Ui.SLIDE_IMMEDIATE);
        return true;
    }
}