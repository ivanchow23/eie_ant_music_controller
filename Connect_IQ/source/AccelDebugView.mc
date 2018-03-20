// AccelDebugView.mc
// Description: The view for showing a debug screen with accelerometer values
// By: Ivan Chow
// Date: March 18, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;
using Toybox.Sensor as Sensor;

class AccelDebugView extends Ui.View {

    hidden var sampleTimer;
    
    // Accel buffer variables
    hidden var accelXBuffer;
    hidden var accelYBuffer;
    hidden var accelZBuffer;
    hidden var accelXBufferIndex;
    hidden var accelYBufferIndex;
    hidden var accelZBufferIndex;
    
    // Accel average variables
    hidden var accelXAvg;
    hidden var accelYAvg;
    hidden var accelZAvg;
    
    const SAMPLE_PERIOD_MS = 50;
    const BUFFER_SECONDS   = 1;
    const BUFFER_SIZE      = (1000 / SAMPLE_PERIOD_MS) * BUFFER_SECONDS;

    function initialize() {
        View.initialize();
        
        sampleTimer = new Timer.Timer();
        sampleTimer.start(method(:sample), SAMPLE_PERIOD_MS, true);
        
        accelXBuffer = new[BUFFER_SIZE];
        accelYBuffer = new[BUFFER_SIZE];
        accelZBuffer = new[BUFFER_SIZE];
        accelXBufferIndex = 0;
        accelYBufferIndex = 0;
        accelZBufferIndex = 0;
        
        for(var i = 0; i < BUFFER_SIZE; i++) {
            accelXBuffer[i] = 0;
            accelYBuffer[i] = 0;
            accelZBuffer[i] = 0;
        }
        
        accelXAvg = 0;
        accelYAvg = 0;
        accelZAvg = 0;
    }
    
    // Update the view
    function onUpdate(dc) {
    
        // Call the parent onUpdate function to redraw the layout
        View.onUpdate(dc);
        
        dc.setColor(Gfx.COLOR_WHITE, Gfx.COLOR_TRANSPARENT);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 3, Gfx.FONT_SMALL, "Sensor Debug page", Gfx.TEXT_JUSTIFY_CENTER);

        // Display accel values to screen
        dc.drawText(dc.getWidth() / 2, dc.getHeight() / 2, Gfx.FONT_XTINY, "X (Avg): " + accelXAvg.toString(), Gfx.TEXT_JUSTIFY_CENTER);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 3 / 5, Gfx.FONT_XTINY, "Y (Avg): " + accelYAvg.toString(), Gfx.TEXT_JUSTIFY_CENTER);
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 7 / 10, Gfx.FONT_XTINY, "Z (Avg): " + accelZAvg.toString(), Gfx.TEXT_JUSTIFY_CENTER);
    }
    
    // Takes a sample of the accelerometer and stores X, Y, and Z-axis values in their respective buffers
    static function sample() {
    
        var sensorInfo = Sensor.getInfo();
        var accel;

        if( (sensorInfo has :accel) && (sensorInfo.accel != null) )
        {
            accel = sensorInfo.accel;
            
            var accelX = accel[0];
            var accelY = accel[1];
            var accelZ = accel[2];
            
            pushSamples(accelX, accelY, accelZ);
        }
        
        Ui.requestUpdate();
    }
    
    // Adds an accel sample into the buffers and updates the moving average of each axis 
    static function pushSamples(x, y, z) {
    
        accelXBuffer[accelXBufferIndex] = x;
        accelYBuffer[accelYBufferIndex] = y;
        accelZBuffer[accelZBufferIndex] = z;
        
        accelXBufferIndex++;
        if(accelXBufferIndex >= BUFFER_SIZE) {
            accelXBufferIndex = 0;
        }
        
        accelYBufferIndex++;
        if(accelYBufferIndex >= BUFFER_SIZE) {
            accelYBufferIndex = 0;
        }
        
        accelZBufferIndex++;
        if(accelZBufferIndex >= BUFFER_SIZE) {
            accelZBufferIndex = 0;
        }
        
        calculateAccelAverage();
    }
    
    // Calculates a moving average for each accel axis
    static function calculateAccelAverage() {
    
        var accelXSum = 0;
        var accelYSum = 0;
        var accelZSum = 0;
        
        // Accumulate sum
        for(var i = 0; i < BUFFER_SIZE; i++) {
            accelXSum = accelXSum + accelXBuffer[i];
            accelYSum = accelYSum + accelYBuffer[i];
            accelZSum = accelZSum + accelZBuffer[i];
        }
        
        // Average
        accelXAvg = accelXSum / BUFFER_SIZE;
        accelYAvg = accelYSum / BUFFER_SIZE;
        accelZAvg = accelZSum / BUFFER_SIZE;
    }
}