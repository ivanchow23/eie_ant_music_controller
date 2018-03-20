// AccelDebugView.mc
// Description: The view for showing a debug screen with accelerometer values
// By: Ivan Chow
// Date: March 18, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;
using Toybox.Sensor as Sensor;

class AccelDebugView extends Ui.View {

    hidden var sampleTimer;
    hidden var accelX;
    hidden var accelY;
    hidden var accelZ;
    
    const SAMPLE_PERIOD_MS = 50;

    function initialize() {
        View.initialize();
        
        sampleTimer = new Timer.Timer();
        sampleTimer.start(method(:sample), SAMPLE_PERIOD_MS, true);
        
        accelX = 0.0;
        accelY = 0.0;
        accelZ = 0.0;
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 3, Gfx.FONT_SMALL, "Sensor Debug page", Gfx.TEXT_JUSTIFY_CENTER);

        // Display accel values to screen
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_XTINY, "X: " + accelX.toString(), Gfx.TEXT_JUSTIFY_CENTER);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 3 / 5, Gfx.FONT_XTINY, "Y: " + accelY.toString(), Gfx.TEXT_JUSTIFY_CENTER);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_XTINY, "Z: " + accelZ.toString(), Gfx.TEXT_JUSTIFY_CENTER);
    }
    
    // Takes a sample of the accelerometer and storex X, Y, and Z-axis values
    static function sample() {
        var sensorInfo = Sensor.getInfo();
        var accel;

        if ( (sensorInfo has :accel) && (sensorInfo.accel != null) )
        {
            accel = sensorInfo.accel;

            accelX = accel[0];
            accelY = accel[1];
            accelZ = accel[2];
        }
        
        Ui.requestUpdate();
    }
}