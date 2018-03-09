// AntChannel.mc
// Description: Class for establishing an ANT channel with the EiE music board.
// By: Ivan Chow
// Date: February 25, 2018

using Toybox.Ant as Ant;
using Toybox.System as System;
using Toybox.WatchUi as Ui;

// ENUM of ANT messages to send
enum {
    ANT_MSG_TOGGLE_PLAY_PAUSE = 0,
    ANT_MSG_PREVIOUS_SONG,
    ANT_MSG_NEXT_SONG
}

// ENUM of corresponding indices in the ANT message payload for each ANT message
enum {
    ANT_MSG_TOGGLE_PLAY_PAUSE_INDEX = 1,
    ANT_MSG_PREVIOUS_SONG_INDEX,
    ANT_MSG_NEXT_SONG_INDEX
}

class AntChannel extends Ant.GenericChannel {

    // Constants
    const DEVICE_NUMBER     = 36976; // 0x9070
    const DEVICE_TYPE       = 15;
    const TRANSMISSION_TYPE = 77;
    const CHANNEL_PERIOD    = 8192;  // In 32768 scale
    const RADIO_FREQUENCY   = 66;    // 2466 Mhz
     
    const CHANNEL_TYPE = Ant.CHANNEL_TYPE_TX_NOT_RX; // Master channel
    const NETWORK_TYPE = Ant.NETWORK_PUBLIC;         // Default network type
    const MAGIC_NUMBER = 197;                        // 0xC5
    
    // Expected acknowledge payload 
    const ackPayload = [0, MAGIC_NUMBER, 0, MAGIC_NUMBER, 0, MAGIC_NUMBER, 0, MAGIC_NUMBER];
    
    // Latest ANT message payload data
    static hidden var data;
    
    // Connection status
    static hidden var connectedFlag;
    
    // Called when a new instance of this object is created
    // Configures and opens the ANT channel as a master device
    function initialize() {
   
        var channelAssignment;
        var deviceConfig;
        
        // Assign the channel
        channelAssignment = new Ant.ChannelAssignment(CHANNEL_TYPE, NETWORK_TYPE);
        
        // Set device configuration
        deviceConfig = new Ant.DeviceConfig( { :deviceNumber => DEVICE_NUMBER,
                                               :deviceType => DEVICE_TYPE,
                                               :transmissionType => TRANSMISSION_TYPE,
                                               :messagePeriod => CHANNEL_PERIOD,
                                               :radioFrequency => RADIO_FREQUENCY 
                                              } 
                                           );
        
        // Initialize channel and set configurations
        GenericChannel.initialize(method(:onMessage), channelAssignment);
        GenericChannel.setDeviceConfig(deviceConfig);
        
        connectedFlag = false;
        
        // Open ANT channel
        if( GenericChannel.open() ) {
            System.println("Channel open.");   
        }
        else {
            System.println("Channel failed to open.");
            Test.assert(true);
        }
        
        // Set initial broadcast message to be all 0's, except first byte (magic number)
        data = new[Ant.Message.DATA_PAYLOAD_LENGTH];
        data[0] = MAGIC_NUMBER;
        
        for(var i = 1; i < Ant.Message.DATA_PAYLOAD_LENGTH; i++) {
            data[i] = 0;
        }
        
        updateAntMsgPayload();
    }
    
    // Sends a message to the ANT channel
    // Increments the value of the byte corresponding to the message (representing sequence number)
    function sendMessage(msg) {
        
        if(msg == ANT_MSG_TOGGLE_PLAY_PAUSE) {
            System.println("Sending toggle play/pause msg.");         
            data[ANT_MSG_TOGGLE_PLAY_PAUSE_INDEX]++;
        }
        
        else if(msg == ANT_MSG_PREVIOUS_SONG) {
            System.println("Sending previous song msg.");
            data[ANT_MSG_PREVIOUS_SONG_INDEX]++;
        }
        
        else if(msg == ANT_MSG_NEXT_SONG) {
            System.println("Sending next song msg.");
            data[ANT_MSG_NEXT_SONG_INDEX]++;
        }
        
        // Send latest data payload to ANT channel
        updateAntMsgPayload();
    }
    
    // Returns true if the ANT channel is connected
    // i.e.: Slave device is able to receive the data, and the app. received its acknowledge
    function isConnected() {
        return connectedFlag;
    }
    
    // Closes and releases the ANT channel
    function release() {
        GenericChannel.release();
        System.println("Channel released.");
    }
    
    // Handles when an incoming message from the channel
    static function onMessage(msg) {
    
        // Check if we got an acknowledge message from slave device
        // This will determine if the 2 devices are "connected"
        if(msg.messageId == 0x4F) {
            
            // Only check if we are currently not connected to save processing time
            if(!connectedFlag) {
            
                var payload = msg.getPayload();
                connectedFlag = checkAckPayload(payload);
                
                // U.I. changes depend on the connection status
                Ui.requestUpdate();
            }
        }
    }

    // Checks the received acknowledge message payload to determine if slave device is connected    
    static function checkAckPayload(payload) {
    
        // Make sure payload matches what we expect
        for( var i = 0; i < Ant.Message.DATA_PAYLOAD_LENGTH; i++ ) {
        
            if( payload[i] != ackPayload[i] ) {
                return false;
            }
        }
        
        return true; 
    }
    
    // Sends the latest ANT message payload to the channel
    // Should be called when an update is made to the data and you want to broadcast it immediately
    static function updateAntMsgPayload() {
    
        // Package data into a message
        var message = new Ant.Message();
        message.setPayload(data);
        
        // Update master channel broadcast
        GenericChannel.sendBroadcast(message);
    }
}