/**********************************************************************************************************************
File: ant_channel.c
Description: Module that is responsible for opening an ANT channel to communicate with a master device.
Author: Ivan Chow
Date: February 12, 2018
------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
  - void AntChannelInitialize(void)
      Runs required initialzation for the task. Should only be called once in main init section.

  - void AntChannelRunActiveState(void)
      Runs current task state. Should only be called once in main loop.
**********************************************************************************************************************/

#include "configuration.h"
#include <stdio.h>

/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define ANT_CHANNEL_NUMBER              ANT_CHANNEL_0
#define ANT_CHANNEL_TYPE                CHANNEL_TYPE_SLAVE
#define ANT_CHANNEL_PERIOD_LO_BYTE      (u8)( 0x00 )        /* Interpret as: 0x<LO_BYTE><HI_BYTE> */
#define ANT_CHANNEL_PERIOD_HI_BYTE      (u8)( 0x20 )
#define ANT_CHANNEL_DEVICE_ID_LO_BYTE   (u8)( 0x70 )        /* Interpret as: 0x<HI_BYTE><LO_BYTE> */
#define ANT_CHANNEL_DEVICE_ID_HI_BYTE   (u8)( 0x90 )
#define ANT_CHANNEL_DEVICE_TYPE         (u8)( 15 )
#define ANT_CHANNEL_TRANSMISSION_TYPE   (u8)( 77 )
#define ANT_CHANNEL_RADIO_FREQUENCY     (u8)( 66 )
#define ANT_CHANNEL_TX_POWER            RADIO_TX_POWER_4DBM

#define ANT_CHANNEL_INITIAL_DELAY_MS    500
#define ANT_CHANNEL_OPEN_TIMEOUT_MS     2000

#define ANT_MESSAGE_MAGIC_NUMBER        (u8)( 0xC5 )

/***********************************************************************************************************************
Existing variables (defined in other files -- should all contain the "extern" keyword)
***********************************************************************************************************************/
extern volatile u32 G_u32SystemTime1ms;         /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;          /* From board-specific source file */

/* From ANT API */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/
/* Index for bytes in ANT message */
typedef enum
{
  ANT_MESSAGE_INDEX_MAGIC_NUMBER = 0,
  ANT_MESSAGE_INDEX_PLAY_PAUSE,
  ANT_MESSAGE_INDEX_PREV_SONG,
  ANT_MESSAGE_INDEX_NEXT_SONG
};

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
***********************************************************************************************************************/
static fnCode_type AntChannel_StateMachine;     /* The state machine function pointer */

static AntAssignChannelInfoType channel_info;   /* Structure holding ANT channel configuration information */
static u32 ant_channel_initial_delay_timer = 0;
static u32 ant_channel_open_timer = 0;

/* Sequence numbers for keeping master and slave device messages in sync */
static u8 ant_msg_play_pause_sequence_number;
static u8 ant_msg_prev_song_sequence_number;
static u8 ant_msg_next_song_sequence_number;

/***********************************************************************************************************************
Local functions
***********************************************************************************************************************/
static void ResetSequenceNumbers(void);
static void ProcessAntMessage(void);
static void AntMessageBytesToString(char* output_string, u8 max_size);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void AntChannelSM_InitialDelay(void);
static void AntChannelSM_Idle(void);
static void AntChannelSM_WaitChannelOpen(void);
static void AntChannelSM_ChannelOpen(void);
static void AntChannelSM_ChannelClosing(void);
static void AntChannelSM_Error(void);

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: AntChannelInitialize

Description:
  Initializes the State Machine and its variables.
*/
void AntChannelInitialize(void)
{
  channel_info.AntChannel          = ANT_CHANNEL_NUMBER;
  channel_info.AntChannelType      = ANT_CHANNEL_TYPE;
  channel_info.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_BYTE;
  channel_info.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_BYTE;
  channel_info.AntDeviceIdLo       = ANT_CHANNEL_DEVICE_ID_LO_BYTE;
  channel_info.AntDeviceIdHi       = ANT_CHANNEL_DEVICE_ID_HI_BYTE;
  channel_info.AntDeviceType       = ANT_CHANNEL_DEVICE_TYPE;
  channel_info.AntTransmissionType = ANT_CHANNEL_TRANSMISSION_TYPE;
  channel_info.AntFrequency        = ANT_CHANNEL_RADIO_FREQUENCY;
  channel_info.AntTxPower          = ANT_CHANNEL_TX_POWER;

  for( u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++ )
  {
    channel_info.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }

  ResetSequenceNumbers();

  // Attempt to configure channel
  if( AntAssignChannel( &channel_info ) )
  {
    // Delay to allow time for ANT channel to be configured before opening
    ant_channel_initial_delay_timer = G_u32SystemTime1ms;

    DebugPrintf( "ANT channel configured\r\n" );
    AntChannel_StateMachine = AntChannelSM_InitialDelay;
  }
  else
  {
    DebugPrintf( "Failed to configure ANT channel." );
    DebugLineFeed();
    LedBlink( RED, LED_4HZ );
    AntChannel_StateMachine = AntChannelSM_Error;
  }
}

/*----------------------------------------------------------------------------------------------------------------------
Function: AntChannelRunActiveState

Description:
  Selects and runs one iteration of the current state in the state machine.
  All state machines have a TOTAL of 1ms to execute, so on average n state machines
  may take 1ms / n to execute.
*/
void AntChannelRunActiveState(void)
{
  AntChannel_StateMachine();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Resets ANT message sequence numbers back to 0. */
static void ResetSequenceNumbers(void)
{
  ant_msg_play_pause_sequence_number = 0;
  ant_msg_prev_song_sequence_number = 0;
  ant_msg_next_song_sequence_number = 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Parses the latest ANT message */
static void ProcessAntMessage(void)
{
  // Return if message does not contain the magic number
  if( G_au8AntApiCurrentMessageBytes[ANT_MESSAGE_INDEX_MAGIC_NUMBER] != ANT_MESSAGE_MAGIC_NUMBER )
  {
    return;
  }

  // Check if there is a new message to toggle play or pause
  if( ant_msg_play_pause_sequence_number != G_au8AntApiCurrentMessageBytes[ANT_MESSAGE_INDEX_PLAY_PAUSE] )
  {
    ant_msg_play_pause_sequence_number = G_au8AntApiCurrentMessageBytes[ANT_MESSAGE_INDEX_PLAY_PAUSE];
    MusicPlayerTogglePlayPause();
  }

  char byte_string[50];

  AntMessageBytesToString( byte_string, 50 );
  DebugPrintf( "Rx'ed: " );
  DebugPrintf( byte_string );
  DebugLineFeed();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Converts the current contents of the ANT message to a formatted string */
static void AntMessageBytesToString(char* output_string, u8 max_size)
{
  u8 buf_index = 0;

  for( u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++ )
  {
    buf_index += snprintf( &output_string[buf_index], max_size, "%X ", G_au8AntApiCurrentMessageBytes[i] );
  }

  output_string[buf_index] = '\0';
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Initial delay state to allow ANT channel time to configure before opening */
static void AntChannelSM_InitialDelay(void)
{
  if( IsTimeUp( &ant_channel_initial_delay_timer, ANT_CHANNEL_INITIAL_DELAY_MS ) )
  {
    AntChannel_StateMachine = AntChannelSM_Idle;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Attempts to open an ANT channel */
static void AntChannelSM_Idle(void)
{
  // Open the channel and set timeout timer
  if( AntOpenChannelNumber( ANT_CHANNEL_NUMBER ) )
  {
    DebugPrintf( "\r\nAttempting to open ANT channel...\r\n" );
    ant_channel_open_timer = G_u32SystemTime1ms;
    AntChannel_StateMachine = AntChannelSM_WaitChannelOpen;
  }
  // Error opening the channel
  else
  {
    LedBlink( RED, LED_4HZ );
    AntChannel_StateMachine = AntChannelSM_Error;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Waits for the ANT channel to open */
static void AntChannelSM_WaitChannelOpen(void)
{
  // Monitor channel status to check if channel is opened
  if( AntRadioStatusChannel( ANT_CHANNEL_NUMBER ) == ANT_OPEN )
  {
    DebugPrintf( "ANT channel open\r\n" );

    // Slow blinking LED indicates channel open, but no master broadcast received
    LedBlink( ORANGE, LED_1HZ );
    AntChannel_StateMachine = AntChannelSM_ChannelOpen;
  }

  // Check for time-out
  // Go back to idle state to try re-opening channel
  if( IsTimeUp( &ant_channel_open_timer, ANT_CHANNEL_OPEN_TIMEOUT_MS ) )
  {
    DebugPrintf( "ANT channel open timeout\r\n" );
    AntCloseChannelNumber( ANT_CHANNEL_NUMBER );
    AntChannel_StateMachine = AntChannelSM_Idle;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle and process messages while ANT channel is active */
static void AntChannelSM_ChannelOpen(void)
{
  // A slave channel can close on its own, so explicitly check channel status
  if( AntRadioStatusChannel( ANT_CHANNEL_NUMBER ) != ANT_OPEN )
  {
    DebugPrintf( "ANT channel no longer open\r\n" );
    AntChannel_StateMachine = AntChannelSM_ChannelClosing;
  }

  if( AntReadAppMessageBuffer() )
  {
    if( G_eAntApiCurrentMessageClass == ANT_DATA )
    {
      // Handle the latest message
      ProcessAntMessage();

      // Show solid LED to indicate channel is formed and we received data
      LedOn( ORANGE );
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Close the channel and go back to waiting */
static void AntChannelSM_ChannelClosing(void)
{
  // Monitor channel status to check if channel is closed
  // If closed, automatically try re-opening the channel
  if( AntRadioStatusChannel( ANT_CHANNEL_NUMBER ) == ANT_CLOSED )
  {
    DebugPrintf( "ANT channel is closed\r\n" );
    LedOff( ORANGE );
    AntChannel_StateMachine = AntChannelSM_Idle;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void AntChannelSM_Error(void)
{
}