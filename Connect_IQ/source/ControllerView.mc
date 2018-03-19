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
    CONTROL_VIEW_COMING_SOON_INDEX,
    
    CONTROL_VIEW_NUM_ITEMS
}

class ControllerView extends Ui.View {

    hidden var itemCounter;         // Reference to a counter object
    hidden var playPauseIconBitmap;
    hidden var prevIconBitmap;
    hidden var nextIconBitmap;

    function initialize(refCounter) {
        View.initialize();
        itemCounter = refCounter;
        
        playPauseIconBitmap = new Ui.Bitmap( { :rezId=>Rez.Drawables.PlayPauseIcon } );
        prevIconBitmap = new Ui.Bitmap( { :rezId=>Rez.Drawables.PrevIcon } );
        nextIconBitmap = new Ui.Bitmap( { :rezId=>Rez.Drawables.NextIcon } );
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
            var bitmapDimensions = playPauseIconBitmap.getDimensions();
            
            playPauseIconBitmap.setLocation(getImageLocationCenteredX(dc, bitmapDimensions[0]) , getImageLocationCenteredY(dc, bitmapDimensions[1]));
            playPauseIconBitmap.draw(dc);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_SMALL, "Play || Pause", Gfx.TEXT_JUSTIFY_CENTER);
        }
        
        // Display previous song icon
        else if(controlIndex == CONTROL_VIEW_PREV_SONG_INDEX) {
            var bitmapDimensions = prevIconBitmap.getDimensions();
        
            prevIconBitmap.setLocation(getImageLocationCenteredX(dc, bitmapDimensions[0]) , getImageLocationCenteredY(dc, bitmapDimensions[1]));
            prevIconBitmap.draw(dc);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_SMALL, "Previous Song", Gfx.TEXT_JUSTIFY_CENTER);
        }
        
        // Display next song icon
        else if(controlIndex == CONTROL_VIEW_NEXT_SONG_INDEX) {
            var bitmapDimensions = nextIconBitmap.getDimensions();
            
            nextIconBitmap.setLocation(getImageLocationCenteredX(dc, bitmapDimensions[0]) , getImageLocationCenteredY(dc, bitmapDimensions[1]));
            nextIconBitmap.draw(dc);
            dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_SMALL, "Next Song", Gfx.TEXT_JUSTIFY_CENTER);
        }  
        
        // TODO: IC - Used for debugging for now
        else if(controlIndex == CONTROL_VIEW_COMING_SOON_INDEX) {
            dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_SMALL, "Coming Soon!", Gfx.TEXT_JUSTIFY_CENTER);
        }
    }
    
    // Returns point along the X-axis which will make the image centered with the device screen
    static function getImageLocationCenteredX(dc, imageDimensionX) {
        return ( (dc.getWidth() / 2) - (imageDimensionX / 2) );
    }
    
    // Returns point along the Y-axis which will make the image centered with the device screen
    static function getImageLocationCenteredY(dc, imageDimensionY) {
        return ( (dc.getHeight() / 2) - (imageDimensionY / 2) );
    }
}