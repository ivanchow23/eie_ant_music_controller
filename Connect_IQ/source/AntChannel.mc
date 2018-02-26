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
        GenericChannel.open();
        
        System.println("Channel open.");
    }
    
    // Handles when a message is going to be broadcast
    static function onMessage(msg) {
        
        // TODO - IC: Print payload for debugging only
        var payload = msg.getPayload();

        System.print(msg.messageId + " ");

        for ( var i = 0; i < 8; i++ )
        {
            System.print(payload[i] + " ");
        }

        System.println("");
    }
}