// AccelDebugView.mc
// Description: The view for showing a debug screen with accelerometer values
// By: Ivan Chow
// Date: March 18, 2018

using Toybox.WatchUi as Ui;
using Toybox.Graphics as Gfx;
using Toybox.Sensor as Sensor;

// Contains gesture control states
enum {
    GESTURE_UNKNOWN = 0,
    GESTURE_RAISED_STATIC = 1
}

class AccelDebugView extends Ui.View {

    // Timer to take a sample
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
    
    // State machine variable
    hidden var state;
    
    // Debug counter for how many times "raised static" gesture was detected
    hidden var raisedCount;
    
    // Sampling variables
    const SAMPLE_PERIOD_MS = 50;
    const BUFFER_SECONDS   = 1;
    const BUFFER_SIZE      = (1000 / SAMPLE_PERIOD_MS) * BUFFER_SECONDS;
    
    // "Raised static" state thresholds
    const X_AXIS_RAISED_STATIC_DC_THRESHOLD       = 0;     // X-axis DC 
    const X_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR = 200;   // Tolerance in X-axis (+/-)
    const Y_AXIS_RAISED_STATIC_DC_THRESHOLD       = 0;     // Y-axis DC
    const Y_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR = 200;   // Tolerance in Y-axis (+/-)
    const Z_AXIS_RAISED_STATIC_DC_THRESHOLD       = -1000; // Z-axis DC threshold
    const Z_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR =  100;  // Tolerance in Z-axis (+/-) 

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
        
        state = GESTURE_UNKNOWN;
        raisedCount = 0;
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
        
        // Display count on screen
        dc.drawText(dc.getWidth() / 2, dc.getHeight() * 4 / 5, Gfx.FONT_XTINY, raisedCount.toString(), Gfx.TEXT_JUSTIFY_CENTER);
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
            updateGestureState();
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
    
    // Algorithm that uses the data in the accel buffers 
    static function updateGestureState() {
    
        // Currently in an unknown state - searching for "raised static" gesture
        // ie: Holding arm up in front of you, with the watch facing up
        if(state == GESTURE_UNKNOWN) {
        
            // Switch states if gesture is detected
            if( isWithinBounds(accelXAvg, X_AXIS_RAISED_STATIC_DC_THRESHOLD, X_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR) &&
                isWithinBounds(accelYAvg, Y_AXIS_RAISED_STATIC_DC_THRESHOLD, Y_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR) &&
                isWithinBounds(accelZAvg, Z_AXIS_RAISED_STATIC_DC_THRESHOLD, Z_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR) ) {
                
                raisedCount++;
                state = GESTURE_RAISED_STATIC;
            }
        }
        // Currently in "raised static" gesture state
        else if(state == GESTURE_RAISED_STATIC) {
            // Break out this state when the thresholds are not satisified
            if( !( isWithinBounds(accelXAvg, X_AXIS_RAISED_STATIC_DC_THRESHOLD, X_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR) &&
                   isWithinBounds(accelYAvg, Y_AXIS_RAISED_STATIC_DC_THRESHOLD, Y_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR) &&
                   isWithinBounds(accelZAvg, Z_AXIS_RAISED_STATIC_DC_THRESHOLD, Z_AXIS_RAISED_STATIC_DC_THRESHOLD_ERROR) ) ) {
                
                state = GESTURE_UNKNOWN;
            }
        }
    }
    
    // Returns true if a value is within the bounds of the threshold with some given tolerance
    // ie: (threshold-tolerance) <= val <= (threshold+tolerance)
    static function isWithinBounds(val, threshold, tolerance) {
        var lowThreshold = threshold - tolerance;
        var highThreshold = threshold + tolerance;
        
        if( (val >= lowThreshold) && (val <= highThreshold) ) {
            return true;
        }
        else {
            return false;
        }
    }
}