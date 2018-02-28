using Toybox.Ant as Ant;
using Toybox.System as System;

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
    
    // Latest ANT message payload data
    static var data;
    
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
    
    // Handles when a message is going to be broadcast
    static function onMessage(msg) {
        // Do nothing
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