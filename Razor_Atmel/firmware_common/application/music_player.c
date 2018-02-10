/**********************************************************************************************************************
File: music_player.c
Description: Module that is responsible for controlling the buzzers for playing music.
------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
  - void MusicPlayerInitialize(void)
      Runs required initialzation for the task. Should only be called once in main init section.

  - void MusicPlayerRunActiveState(void)
      Runs current task state. Should only be called once in main loop.
**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Existing variables (defined in other files -- should all contain the "extern" keyword)
***********************************************************************************************************************/
extern volatile u32 G_u32SystemTime1ms;         /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;          /* From board-specific source file */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
***********************************************************************************************************************/
static fnCode_type MusicPlayer_StateMachine;    /* The state machine function pointer */

/* Note constants */
static u16 au16_notes_right[]     = {F5, F5, F5, F5, F5, E5, D5, E5, F5, G5, A5, A5, A5, A5, A5, G5, F5, G5, A5, A5S, C6, F5, F5, D6, C6, A5S, A5, G5, F5, NO, NO};
static u16 au16_duration_right[]  = {QN, QN, HN, EN, EN, EN, EN, EN, EN, QN, QN, QN, HN, EN, EN, EN, EN, EN, EN, QN,  HN, HN, EN, EN, EN, EN,  QN, QN, HN, HN, FN};
static u16 au16_note_type_right[] = {RT, RT, HT, RT, RT, RT, RT, RT, RT, RT, RT, RT, HT, RT, RT, RT, RT, RT, RT, RT,  RT, HT, RT, RT, RT, RT,  RT, RT, RT, HT, HT};

static u16 au16_notes_left[]     = {F4, F4, A4, A4, D4, D4, F4, F4, A3S, A3S, D4, D4, C4, C4, E4, E4};
static u16 au16_duration_left[]  = {EN, EN, EN, EN, EN, EN, EN, EN, EN,  EN,  EN, EN, EN, EN, EN, EN};
static u16 au16_note_type_left[] = {RT, RT, RT, RT, RT, RT, RT, RT, RT,  RT,  RT, RT, RT, RT, RT, RT};

/* Right buzzer variables */
static u8 u8_index_right = 0;
static u32 u32_right_timer = 0;
static u16 u16_current_duration_right = 0;
static u16 u16_note_silent_duration_right = 0;
static bool b_note_active_next_right = TRUE;

/* Left buzzer variables */
static u8 u8_index_left = 0;
static u32 u32_left_timer = 0;
static u16 u16_current_duration_left = 0;
static u16 u16_note_silent_duration_left = 0;
static bool b_note_active_next_left = TRUE;

static u8 u8_current_index = 0;

/***********************************************************************************************************************
Local functions
***********************************************************************************************************************/
static void PlayNote(void);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void MusicPlayerSM_Play(void);
static void MusicPlayerSM_Pause(void);
static void MusicPlayerSM_Error(void);

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

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
  MusicPlayer_StateMachine = MusicPlayerSM_Play;
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

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: PlayNote

Description:
  Runs the algorithm that determines which note to play on each buzzer and for how long.
  Adapted from the EiE "buzzers advanced" firmware module.
*/
static void PlayNote(void)
{
  // Right buzzer
  if( IsTimeUp( &u32_right_timer, (u32)u16_current_duration_right) )
  {
    // Get the current time and note index
    u32_right_timer = G_u32SystemTime1ms;
    u8_current_index = u8_index_right;

    // If the note is currently active
    if( b_note_active_next_right )
    {
      // Check RT, ST, HT notes
      if( au16_note_type_right[u8_current_index] == RT )
      {
        // RT notes have a short silent period
        u16_current_duration_right = au16_duration_right[u8_current_index] - REGULAR_NOTE_ADJUSTMENT;
        u16_note_silent_duration_right = REGULAR_NOTE_ADJUSTMENT;
        b_note_active_next_right = FALSE;
      }
      else if( au16_note_type_right[u8_current_index] == ST )
      {
        // ST notes are brief, so they have a fixed note time
        u16_current_duration_right = STACCATO_NOTE_TIME;
        u16_note_silent_duration_right = au16_duration_right[u8_current_index] - STACCATO_NOTE_TIME;
        b_note_active_next_right = FALSE;
      }
      else if( au16_note_type_right[u8_current_index] == HT )
      {
        // Hold notes don't have silent periosd - note is played for the full duration
        u16_current_duration_right = au16_duration_right[u8_current_index];
        u16_note_silent_duration_right = 0;
        b_note_active_next_right = TRUE;

        u8_index_right++;
        if( u8_index_right == ( sizeof( au16_notes_right ) / sizeof( u16 ) ) )
        {
          u8_index_right = 0;
        }
      }

      // Set the note frequency and turn the audio on or handle the special case of a "NO" note
      if( au16_notes_right[u8_current_index] != NO )
      {
        PWMAudioSetFrequency( BUZZER1, au16_notes_right[u8_current_index] );
        PWMAudioOn( BUZZER1 );
      }
      else
      {
        PWMAudioOff( BUZZER1 );
      }
    }
    // Note is not active - silent period of the note
    else
    {
      // Turn off the buzzer for this period
      PWMAudioOff( BUZZER1 );

      // Set currentDuration = silentDuration
      u32_right_timer = G_u32SystemTime1ms;
      u16_current_duration_right = u16_note_silent_duration_right;
      b_note_active_next_right = TRUE;

      u8_index_right++;
      if( u8_index_right == ( sizeof( au16_notes_right ) / sizeof( u16 ) ) )
      {
        u8_index_right = 0;
      }
    }
  }

   // Left buzzer
  if( IsTimeUp( &u32_left_timer, (u32)u16_current_duration_left ) )
  {
    u32_left_timer = G_u32SystemTime1ms;
    u8_current_index = u8_index_left;

    // Set up to play current note
    if( b_note_active_next_left )
    {
      if( au16_note_type_left[u8_current_index] == RT )
      {
        u16_current_duration_left = au16_duration_left[u8_current_index] - REGULAR_NOTE_ADJUSTMENT;
        u16_note_silent_duration_left = REGULAR_NOTE_ADJUSTMENT;
        b_note_active_next_left = FALSE;
      }

      else if( au16_note_type_left[u8_current_index] == ST )
      {
        u16_current_duration_left = STACCATO_NOTE_TIME;
        u16_note_silent_duration_left = au16_duration_left[u8_current_index] - STACCATO_NOTE_TIME;
        b_note_active_next_left = FALSE;
      }

      else if( au16_note_type_left[u8_current_index] == HT )
      {
        u16_current_duration_left = au16_duration_left[u8_current_index];
        u16_note_silent_duration_left = 0;
        b_note_active_next_left = TRUE;

        u8_index_left++;
        if( u8_index_left == sizeof( au16_notes_left) / sizeof(u16) )
        {
          u8_index_left = 0;
        }
      }

      // Set the buzzer frequency for the note (handle NO special case)
      if( au16_notes_left[u8_current_index] != NO )
      {
        PWMAudioSetFrequency(BUZZER2, au16_notes_left[u8_current_index]);
        PWMAudioOn( BUZZER2 );
      }
      else
      {
        PWMAudioOff( BUZZER2 );
      }
    }
    else
    {
      PWMAudioOff( BUZZER2 );
      u32_left_timer = G_u32SystemTime1ms;
      u16_current_duration_left = u16_note_silent_duration_left;
      b_note_active_next_left = TRUE;

      u8_index_left++;
      if( u8_index_left == sizeof( au16_notes_left ) / sizeof( u16 ) )
      {
        u8_index_left = 0;
      }
    } // end else if(b_note_active_next_left)
  } // end if(IsTimeUp(&u32_left_timer, (u32)u16_current_duration_left))
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
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Turns buzzers off, and waits for prompt to go back to play. */
static void MusicPlayerSM_Pause(void)
{
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void MusicPlayerSM_Error(void)
{
}