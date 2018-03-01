using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;

class AntMusicControllerView extends Ui.View {

    function initialize() {
        View.initialize();
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
        dc.setColor(Gfx.COLOR_DK_GREEN, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 2 / 5, Gfx.FONT_XTINY, "ANT WIRELESS", Gfx.TEXT_JUSTIFY_CENTER);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_XTINY, "MUSIC CONTROLLER", Gfx.TEXT_JUSTIFY_CENTER);
        
        dc.setColor(Gfx.COLOR_YELLOW, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_XTINY, "By: Ivan Chow", Gfx.TEXT_JUSTIFY_CENTER);
        
        var versionString = "Ver: " + Version.VERSION_MAJOR + "." + Version.VERSION_MINOR + "." + Version.VERSION_MICRO;
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 4 / 5, Gfx.FONT_XTINY, versionString, Gfx.TEXT_JUSTIFY_CENTER);
    }

    // Called when this View is removed from the screen. Save the
    // state of this View here. This includes freeing resources from
    // memory.
    function onHide() {
    }

}
