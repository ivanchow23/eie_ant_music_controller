using Toybox.Ant as Ant;
using Toybox.System as System;

class AntChannel extends Ant.GenericChannel {

    const DEVICE_NUMBER     = 36976;
    const DEVICE_TYPE       = 15;
    const TRANSMISSION_TYPE = 77;
    const CHANNEL_PERIOD    = 8192; // In 32768 scale
    const RADIO_FREQUENCY   = 66;   // 2466 Mhz
     
    const CHANNEL_TYPE = Ant.CHANNEL_TYPE_TX_NOT_RX; // Master channel
    const NETWORK_TYPE = Ant.NETWORK_PUBLIC;
    
    const MAGIC_NUMBER = 197; // 0xC5
    
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
        var data    = new[Ant.Message.DATA_PAYLOAD_LENGTH];
        var message = new Ant.Message();
        
        data[0] = MAGIC_NUMBER;
        for(var i = 1; i < Ant.Message.DATA_PAYLOAD_LENGTH; i++) {
            data[i] = 0;
        }
        
        message.setPayload(data);
        GenericChannel.sendBroadcast(message);
    }
    
    // Handles when a message is going to be broadcast
    static function onMessage(msg) {
        
        // TODO - IC: Print payload for debugging only
        var payload = msg.getPayload();

        System.print(msg.messageId + " ");

        for ( var i = 0; i < msg.length; i++ )
        {
            System.print(payload[i] + " ");
        }

        System.println("");
    }
}