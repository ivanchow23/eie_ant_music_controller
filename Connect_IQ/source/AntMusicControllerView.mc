// AntMusicControllerView.mc
// Description: The view for the start of the application.
// By: Ivan Chow
// Date: February 25, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

class AntMusicControllerView extends Ui.View {
    
    hidden var musicalNotesBitmap;
    hidden var antLogoBitmap;

    function initialize() {
        View.initialize();
        musicalNotesBitmap = new Ui.Bitmap( { :rezId=>Rez.Drawables.MusicalNotes } );
        antLogoBitmap = new Ui.Bitmap( { :rezId=>Rez.Drawables.AntLogo } );
    }

    // Load your resources here
    function onLayout(dc) {
        setLayout(Rez.Layouts.MainLayout(dc));
    }

    // Called when this View is brought to the foreground. Restore
    // the state of this View and prepare it to be shown. This includes
    // loading resources into memory.
    function onShow() {
    }

    // Update the view - Show title screen
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        // Show the title
        dc.setColor(Gfx.COLOR_GREEN, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_XTINY, "ANT MUSIC CONTROLLER", Gfx.TEXT_JUSTIFY_CENTER);
        
        dc.setColor(Gfx.COLOR_YELLOW, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 10, dc.getHeight() / 2, Gfx.FONT_XTINY, "By: Ivan Chow", Gfx.TEXT_JUSTIFY_LEFT);
        
        var versionString = "V" + Version.VERSION_MAJOR + "." + Version.VERSION_MINOR + "." + Version.VERSION_MICRO;
        dc.drawText(dc.getWidth() - (dc.getWidth() / 10), dc.getHeight() / 2, Gfx.FONT_XTINY, versionString, Gfx.TEXT_JUSTIFY_RIGHT);
        
        // Show prompt to begin app.
        dc.setColor(Gfx.COLOR_GREEN, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_XTINY, "Press Start", Gfx.TEXT_JUSTIFY_CENTER);
        
        // Draw symbols
        // Note: Location of bitmaps are relative to its top left corner
        var bitmapDimensions = musicalNotesBitmap.getDimensions();
        var bitmapLocX = (dc.getWidth() / 2) - (bitmapDimensions[0] / 2) + (dc.getWidth() / 10);
        var bitmapLocY = ( (dc.getHeight() / 2) - bitmapDimensions[1] ) / 2;

        musicalNotesBitmap.setLocation(bitmapLocX, bitmapLocY);
        musicalNotesBitmap.draw(dc);
        
        bitmapDimensions = antLogoBitmap.getDimensions();
        bitmapLocX = (dc.getWidth() / 2) - (bitmapDimensions[0] / 2) - (dc.getWidth() / 5);
        bitmapLocY = ( (dc.getHeight() / 2) - bitmapDimensions[1] ) / 2;
        
        antLogoBitmap.setLocation(bitmapLocX, bitmapLocY);
        antLogoBitmap.draw(dc);
    }

    // Called when this View is removed from the screen. Save the
    // state of this View here. This includes freeing resources from
    // memory.
    function onHide() {
    }

}
