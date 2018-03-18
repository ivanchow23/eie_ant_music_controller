/**********************************************************************************************************************
File: music_player.c
Description: Module that is responsible for controlling the buzzers for playing music.
Author: Ivan Chow
Date: February 9, 2018
------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
  - void MusicPlayerInitialize(void)
      Runs required initialzation for the task. Should only be called once in main init section.

  - void MusicPlayerRunActiveState(void)
      Runs current task state. Should only be called once in main loop.

  - u8 MusicPlayerGetCurrentSongIndex(void)
      Returns the index of the song currently playing

  - const char* MusicPlayerGetCurrentSongTitle(void)
      Returns a pointer to the title string of the song currently playing

  - const char* MusicPlayerGetCurrentSongArtist(void)
      Returns a pointer to the artist string of the song currently playing

  - void MusicPlayerTogglePlayPause(void)
      Toggles play or pause. If song is currently playing, pause it, and vice-versa.

  - void MusicPlayerPreviousSong(void)
      Plays the previous song in the list

  - void MusicPlayerNextSong(void)
      Plays the next song in the list
**********************************************************************************************************************/

#include "configuration.h"
#include "songs.h"

/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define LED_WHITE_FREQ_THRESHOLD    200
#define LED_PURPLE_FREQ_THRESHOLD   400
#define LED_BLUE_FREQ_THRESHOLD     600
#define LED_CYAN_FREQ_THRESHOLD     800
#define LED_GREEN_FREQ_THRESHOLD    1000
#define LED_YELLOW_FREQ_THRESHOLD   1500
#define LED_ORANGE_FREQ_THRESHOLD   2000

/***********************************************************************************************************************
Macros
***********************************************************************************************************************/
#define LED_ON( LED )               LedPWM( LED, LED_PWM_5 )

/***********************************************************************************************************************
Existing variables (defined in other files -- should all contain the "extern" keyword)
***********************************************************************************************************************/
extern volatile u32 G_u32SystemTime1ms;         /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;          /* From board-specific source file */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
***********************************************************************************************************************/
static fnCode_type MusicPlayer_StateMachine;    /* The state machine function pointer */

/* Index of song currently playing from song_list */
static u8 song_index = 0;

/* Right buzzer variables */
static u32 buzzer_right_timer = 0;
static u16 current_note_duration_right = 0;
static u32 note_right_index = 0;

/* Left buzzer variables */
static u32 buzzer_left_timer = 0;
static u16 current_note_duration_left = 0;
static u32 note_left_index = 0;

/***********************************************************************************************************************
Local functions
***********************************************************************************************************************/
static void PlayNote(void);
static void ResetBuzzerVariables(void);
static void PreviousSong(void);
static void NextSong(void);
static void FlashLed(u16 note_freq_right, u16 note_freq_left);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void MusicPlayerSM_Play(void);
static void MusicPlayerSM_Pause(void);

/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerInitialize

Description:
  Initializes the State Machine and its variables.
*/
void MusicPlayerInitialize(void)
{
  ResetBuzzerVariables();
  MusicPlayer_StateMachine = MusicPlayerSM_Pause;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerRunActiveState

Description:
  Selects and runs one iteration of the current state in the state machine.
  All state machines have a TOTAL of 1ms to execute, so on average n state machines
  may take 1ms / n to execute.
*/
void MusicPlayerRunActiveState(void)
{
  MusicPlayer_StateMachine();
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerGetCurrentSongIndex

Description:
  Returns the current song index.
*/
u8 MusicPlayerGetCurrentSongIndex(void)
{
  return song_index;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerGetCurrentSongTitle

Description:
  Returns the title of the current song.
*/
const char* MusicPlayerGetCurrentSongTitle(void)
{
  return song_list[song_index]->title;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerGetCurrentSongArtist

Description:
  Returns the title of the current artist.
*/
const char* MusicPlayerGetCurrentSongArtist(void)
{
  return song_list[song_index]->artist;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerTogglePlayPause

Description:
  Toggles play or pause. If song is currently playing, pause it, and vice-versa.
*/
void MusicPlayerTogglePlayPause(void)
{
  // Use state machine function pointer to determine if we're playing or paused
  if( MusicPlayer_StateMachine == MusicPlayerSM_Play )
  {
    MusicPlayer_StateMachine = MusicPlayerSM_Pause;
  }
  else if( MusicPlayer_StateMachine == MusicPlayerSM_Pause )
  {
    MusicPlayer_StateMachine = MusicPlayerSM_Play;
  }
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerPreviousSong

Description:
  Plays the previous song in the list
*/
void MusicPlayerPreviousSong(void)
{
  PreviousSong();
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerNextSong

Description:
  Plays the next song in the list
*/
void MusicPlayerNextSong(void)
{
  NextSong();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: PlayNote

Description:
  Runs the algorithm that determines which note to play on each buzzer and for how long.
*/
static void PlayNote(void)
{
  // Play right buzzer tone
  PWMAudioSetFrequency( BUZZER1, song_list[song_index]->note_right[note_right_index] );
  PWMAudioOn( BUZZER1 );

  // Play left buzzer tone
  PWMAudioSetFrequency( BUZZER2, song_list[song_index]->note_left[note_left_index] );
  PWMAudioOn( BUZZER2 );

  // Right buzzer timer
  if( IsTimeUp( &buzzer_right_timer, current_note_duration_right ) )
  {
    // Advance to next note
    if( ++note_right_index >= song_list[song_index]->num_notes_right )
    {
      note_right_index = 0;
    }

    // LED control
    FlashLed( song_list[song_index]->note_right[note_right_index], song_list[song_index]->note_left[note_left_index] );

    buzzer_right_timer = G_u32SystemTime1ms;
    current_note_duration_right = song_list[song_index]->note_duration_right[note_right_index];
  }

  // Left buzzer timer
  if( IsTimeUp( &buzzer_left_timer, current_note_duration_left ) )
  {
    // Advance to next note
    if( ++note_left_index >= song_list[song_index]->num_notes_left )
    {
      note_left_index = 0;
    }

    // LED control
    FlashLed( song_list[song_index]->note_right[note_right_index], song_list[song_index]->note_left[note_left_index] );

    buzzer_left_timer = G_u32SystemTime1ms;
    current_note_duration_left = song_list[song_index]->note_duration_left[note_left_index];
  }
}

/*----------------------------------------------------------------------------------------------------------------------
Function: ResetBuzzerVariables

Description:
  Resets variables for the buzzer.
  Called when a song is switched, or on initialization.
*/
static void ResetBuzzerVariables(void)
{
  buzzer_right_timer = G_u32SystemTime1ms;
  note_right_index = -1;
  current_note_duration_right = 0;

  buzzer_left_timer = G_u32SystemTime1ms;
  note_left_index = -1;
  current_note_duration_left = 0;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: PreviousSong

Description:
  Handles the case of playing the song previous to the current song.
  Changes song index, resets variables, and automatically play the song
*/
static void PreviousSong(void)
{
  // If currently playing first song of the list, wrap back around to end of list
  if( song_index == 0 )
  {
    song_index = SONG_LIST_SIZE - 1;
  }
  else
  {
    song_index--;
  }

  // Reset relevant variables
  ResetBuzzerVariables();

  // Automatically move to new state
  MusicPlayer_StateMachine = MusicPlayerSM_Play;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: NextSong

Description:
  Handles the case of playing the song next to the current song.
  Changes song index, resets variables, and automatically play the song.
*/
static void NextSong(void)
{
  // If currently playing last song of the list, wrap back around to beginning of list
  if( song_index == ( SONG_LIST_SIZE - 1 ) )
  {
    song_index = 0;
  }
  else
  {
    song_index++;
  }

  // Reset relevant variables
  ResetBuzzerVariables();

  // Automatically move to new state
  MusicPlayer_StateMachine = MusicPlayerSM_Play;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: FlashLed

Description:
  Flashes an LED based on which notes are being played.
  If one frequency is zero, and the other is non-zero, the zero frequency is ignored.
  If both buzzers frequencies are non-zero, then the frequency is averaged.
  The resulting frequency will be used to determine which LED should be flashed.
*/
static void FlashLed(u16 note_freq_right, u16 note_freq_left)
{
  u8  led_to_turn_on = -1;
  u16 frequency = 0;

  // Use left note frequency if right note is currently zero
  if( note_freq_right == 0 )
  {
    frequency = note_freq_left;
  }
  // Use right note frequency if left note is currently zero
  else if( note_freq_left == 0 )
  {
    frequency = note_freq_right;
  }
  // Take the average of the two buzzer frequencies if both are non-zero
  else if( ( note_freq_right != 0 ) && ( note_freq_left != 0 ) )
  {
    frequency = ( note_freq_right + note_freq_left ) / 2;
  }

  // Determine which LED to turn on
  // If frequency is 0 (both buzzers off), then it should not turn an LED on
  if( frequency != 0 )
  {
    if( frequency < LED_WHITE_FREQ_THRESHOLD )
    {
      led_to_turn_on = WHITE;
    }
    else if( frequency < LED_PURPLE_FREQ_THRESHOLD )
    {
      led_to_turn_on = PURPLE;
    }
    else if( frequency < LED_BLUE_FREQ_THRESHOLD )
    {
      led_to_turn_on = BLUE;
    }
    else if( frequency < LED_CYAN_FREQ_THRESHOLD )
    {
      led_to_turn_on = CYAN;
    }
    else if( frequency < LED_GREEN_FREQ_THRESHOLD )
    {
      led_to_turn_on = GREEN;
    }
    else if( frequency < LED_YELLOW_FREQ_THRESHOLD )
    {
      led_to_turn_on = YELLOW;
    }
    else
    {
      led_to_turn_on = ORANGE;
    }
  }

  // LEDs are ENUMs in the same order as how its tied on the board
  // Skip the red LED since they represent ANT channel status
  for( u8 i = 0; i < RED; i++ )
  {
    if( i == led_to_turn_on )
    {
      LED_ON( (LedNumberType)i );
    }
    else
    {
      LedOff( (LedNumberType)i );
    }
  }
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Plays the current song. */
static void MusicPlayerSM_Play(void)
{
  // Play the next note
  PlayNote();

  // Button 0 pauses the music
  if( WasButtonPressed( BUTTON0 ) )
  {
    ButtonAcknowledge( BUTTON0 );
    MusicPlayer_StateMachine = MusicPlayerSM_Pause;
  }

  // Button 1 goes to "previous" song
  if( WasButtonPressed( BUTTON1 ) )
  {
    ButtonAcknowledge( BUTTON1 );
    PreviousSong();
  }

  // Button 2 goes to "next" song
  if( WasButtonPressed( BUTTON2 ) )
  {
    ButtonAcknowledge( BUTTON2 );
    NextSong();
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Turns buzzers off, and waits for prompt to go back to play. */
static void MusicPlayerSM_Pause(void)
{
  // Turn buzzers off
  PWMAudioOff( BUZZER1 );
  PWMAudioOff( BUZZER2 );

  // Button 0 plays music again
  if( WasButtonPressed( BUTTON0 ) )
  {
    ButtonAcknowledge( BUTTON0 );
    MusicPlayer_StateMachine = MusicPlayerSM_Play;
  }

  // Button 1 goes to "previous" song
  if( WasButtonPressed( BUTTON1 ) )
  {
    ButtonAcknowledge( BUTTON1 );
    PreviousSong();
  }

  // Button 2 goes to "next" song
  if( WasButtonPressed( BUTTON2 ) )
  {
    ButtonAcknowledge( BUTTON2 );
    NextSong();
  }
}