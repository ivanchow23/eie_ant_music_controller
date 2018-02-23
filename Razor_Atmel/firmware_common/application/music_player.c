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

  - boolean MusicPlayerTogglePlayPause(void)
      Toggles play or pause. If song is currently playing, pause it, and vice-versa.
**********************************************************************************************************************/

#include "configuration.h"

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define SONG_LIST_SIZE  ( sizeof( song_list ) / sizeof( song_list[0] ) )

/***********************************************************************************************************************
Existing variables (defined in other files -- should all contain the "extern" keyword)
***********************************************************************************************************************/
extern volatile u32 G_u32SystemTime1ms;         /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;          /* From board-specific source file */

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
/* Holds song information */
typedef struct
{
  const char* title;
  const char* artist;
  const u16*  note_right;           /* Array of note frequencies for right buzzer */
  const u16*  note_duration_right;  /* Array of note durations for right buzzer*/
  const u16*  note_type_right;      /* Array of note types for right buzzer */
  const u16   num_notes_right;      /* Number of notes for right buzzer */
  const u16*  note_left;            /* Array of note frequencies for left buzzer */
  const u16*  note_duration_left;   /* Array of note durations for left buzzer*/
  const u16*  note_type_left;       /* Array of note types for left buzzer */
  const u16   num_notes_left;       /* Number of notes for left buzzer */
} SongInfoType;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
***********************************************************************************************************************/
static fnCode_type MusicPlayer_StateMachine;    /* The state machine function pointer */

/* Song #1 */
static const u16 song1_note_right[]          = { F5, F5, F5, F5, F5, E5, D5, E5, F5, G5, A5, A5, A5, A5, A5, G5, F5, G5, A5, A5S, C6, F5, F5, D6, C6, A5S, A5, G5, F5, NO, NO };
static const u16 song1_note_duration_right[] = { QN, QN, HN, EN, EN, EN, EN, EN, EN, QN, QN, QN, HN, EN, EN, EN, EN, EN, EN, QN,  HN, HN, EN, EN, EN, EN,  QN, QN, HN, HN, FN };
static const u16 song1_note_type_right[]     = { RT, RT, HT, RT, RT, RT, RT, RT, RT, RT, RT, RT, HT, RT, RT, RT, RT, RT, RT, RT,  RT, HT, RT, RT, RT, RT,  RT, RT, RT, HT, HT };
static const u16 song1_note_left[]           = { F4, F4, A4, A4, D4, D4, F4, F4, A3S, A3S, D4, D4, C4, C4, E4, E4 };
static const u16 song1_note_duration_left[]  = { EN, EN, EN, EN, EN, EN, EN, EN, EN,  EN,  EN, EN, EN, EN, EN, EN };
static const u16 song1_note_type_left[]      = { RT, RT, RT, RT, RT, RT, RT, RT, RT,  RT,  RT, RT, RT, RT, RT, RT };

static const SongInfoType song1 = { "Heart and Soul", "Hoagy Carmichael",
                                    song1_note_right, song1_note_duration_right, song1_note_type_right, sizeof( song1_note_right ) / sizeof( song1_note_right[0] ),
                                    song1_note_left, song1_note_duration_left, song1_note_type_left, sizeof( song1_note_left ) / sizeof( song1_note_left[0] )
                                  };

/* Song #2 */
static const u16 song2_note_right[]          = { 330, 294, 262, NO, 330, 294, 262, NO, 262, 262, 262, 262, 294, 294, 294, 294, 330, 294, 262, NO };
static const u16 song2_note_duration_right[] = { QN,  QN,  QN,  QN, QN,  QN,  QN,  QN, EN,  EN,  EN,  EN,  EN,  EN,  EN,  EN,  QN,  QN,  QN,  HN };
static const u16 song2_note_type_right[]     = { RT,  RT,  RT,  RT, RT,  RT,  RT,  RT, RT,  RT,  RT,  RT,  RT,  RT,  RT,  RT,  RT,  RT,  RT,  RT };
static const u16 song2_note_left[]           = { NO };
static const u16 song2_note_duration_left[]  = { HN };
static const u16 song2_note_type_left[]      = { RT };

static const SongInfoType song2 = { "Hot Cross Buns", "?",
                                    song2_note_right, song2_note_duration_right, song2_note_type_right, sizeof( song2_note_right ) / sizeof( song2_note_right[0] ),
                                    song2_note_left, song2_note_duration_left, song2_note_type_left, sizeof( song2_note_left ) / sizeof( song2_note_left[0] )
                                  };

/* Song #3 */
static const u16 song3_note_right[]          = { E4, E4, E4, E4, C4, C4, G4, G4, G4, G4, G4, G4, G4, G4, D4, D4 };
static const u16 song3_note_duration_right[] = { QN, QN, QN, QN, HN, HN, EN, EN, EN, EN, EN, EN, EN, EN, HN, HN };
static const u16 song3_note_type_right[]     = { RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT };
static const u16 song3_note_left[]           = { E4, E4, E4, E4, C4, C4, G4, G4, G4, G4, G4, G4, G4, G4, D4, D4 };
static const u16 song3_note_duration_left[]  = { QN, QN, QN, QN, HN, HN, EN, EN, EN, EN, EN, EN, EN, EN, HN, HN };
static const u16 song3_note_type_left[]      = { RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT };

static const SongInfoType song3 = { "Song #3", "Placeholder",
                                    song3_note_right, song3_note_duration_right, song3_note_type_right, sizeof( song3_note_right ) / sizeof( song3_note_right[0] ),
                                    song3_note_left, song3_note_duration_left, song3_note_type_left, sizeof( song3_note_left ) / sizeof( song3_note_left[0] )
                                  };

/* List of songs */
static const SongInfoType* song_list[] = { &song1, &song2, &song3 };

/* Index of the current song */
static u8 current_song_index = 0;

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

/***********************************************************************************************************************
Local functions
***********************************************************************************************************************/
static void PlayNote(void);
static void ResetBuzzerVariables(void);
static void PreviousSong(void);
static void NextSong(void);

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
  return current_song_index;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerGetCurrentSongTitle

Description:
  Returns the title of the current song.
*/
const char* MusicPlayerGetCurrentSongTitle(void)
{
  return song_list[current_song_index]->title;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MusicPlayerGetCurrentSongArtist

Description:
  Returns the title of the current artist.
*/
const char* MusicPlayerGetCurrentSongArtist(void)
{
  return song_list[current_song_index]->artist;
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
  if( IsTimeUp( &u32_right_timer, (u32)u16_current_duration_right ) )
  {
    // Get the current time and note index
    u32_right_timer = G_u32SystemTime1ms;
    u8 u8_current_index = u8_index_right;

    // If the note is currently active
    if( b_note_active_next_right )
    {
      // Check RT, ST, HT notes
      if( song_list[current_song_index]->note_type_right[u8_current_index] == RT )
      {
        // RT notes have a short silent period
        u16_current_duration_right = song_list[current_song_index]->note_duration_right[u8_current_index] - REGULAR_NOTE_ADJUSTMENT;
        u16_note_silent_duration_right = REGULAR_NOTE_ADJUSTMENT;
        b_note_active_next_right = FALSE;
      }
      else if( song_list[current_song_index]->note_type_right[u8_current_index] == ST )
      {
        // ST notes are brief, so they have a fixed note time
        u16_current_duration_right = STACCATO_NOTE_TIME;
        u16_note_silent_duration_right = song_list[current_song_index]->note_duration_right[u8_current_index] - STACCATO_NOTE_TIME;
        b_note_active_next_right = FALSE;
      }
      else if( song_list[current_song_index]->note_type_right[u8_current_index] == HT )
      {
        // Hold notes don't have silent periosd - note is played for the full duration
        u16_current_duration_right = song_list[current_song_index]->note_duration_right[u8_current_index];
        u16_note_silent_duration_right = 0;
        b_note_active_next_right = TRUE;

        u8_index_right++;
        if( u8_index_right >= song_list[current_song_index]->num_notes_right )
        {
          u8_index_right = 0;
        }
      }

      // Set the note frequency and turn the audio on or handle the special case of a "NO" note
      if( song_list[current_song_index]->note_right[u8_current_index] != NO )
      {
        PWMAudioSetFrequency( BUZZER1, song_list[current_song_index]->note_right[u8_current_index] );
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
      if( u8_index_right >= song_list[current_song_index]->num_notes_right )
      {
        u8_index_right = 0;
      }
    }
  }

  // Left buzzer
  if( IsTimeUp( &u32_left_timer, (u32)u16_current_duration_left ) )
  {
    u32_left_timer = G_u32SystemTime1ms;
    u8 u8_current_index = u8_index_left;

    // Set up to play current note
    if( b_note_active_next_left )
    {
      if( song_list[current_song_index]->note_type_left[u8_current_index] == RT )
      {
        u16_current_duration_left = song_list[current_song_index]->note_duration_left[u8_current_index] - REGULAR_NOTE_ADJUSTMENT;
        u16_note_silent_duration_left = REGULAR_NOTE_ADJUSTMENT;
        b_note_active_next_left = FALSE;
      }

      else if( song_list[current_song_index]->note_type_left[u8_current_index] == ST )
      {
        u16_current_duration_left = STACCATO_NOTE_TIME;
        u16_note_silent_duration_left = song_list[current_song_index]->note_duration_left[u8_current_index] - STACCATO_NOTE_TIME;
        b_note_active_next_left = FALSE;
      }

      else if( song_list[current_song_index]->note_type_left[u8_current_index] == HT )
      {
        u16_current_duration_left = song_list[current_song_index]->note_duration_left[u8_current_index];
        u16_note_silent_duration_left = 0;
        b_note_active_next_left = TRUE;

        u8_index_left++;
        if( u8_index_left >= song_list[current_song_index]->num_notes_left )
        {
          u8_index_left = 0;
        }
      }

      // Set the buzzer frequency for the note (handle NO special case)
      if( song_list[current_song_index]->note_left[u8_current_index] != NO )
      {
        PWMAudioSetFrequency( BUZZER2, song_list[current_song_index]->note_left[u8_current_index] );
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
      if( u8_index_left >= song_list[current_song_index]->num_notes_left )
      {
        u8_index_left = 0;
      }
    } // end else if(b_note_active_next_left)
  } // end if(IsTimeUp(&u32_left_timer, (u32)u16_current_duration_left))
}

/*----------------------------------------------------------------------------------------------------------------------
Function: ResetBuzzerVariables

Description:
  Resets variables for the buzzer.
  Called when a song is switched, or on initialization.
*/
static void ResetBuzzerVariables(void)
{
  u8_index_right = 0;
  u32_right_timer = 0;
  u16_current_duration_right = 0;
  u16_note_silent_duration_right = 0;
  b_note_active_next_right = TRUE;

  u8_index_left = 0;
  u32_left_timer = 0;
  u16_current_duration_left = 0;
  u16_note_silent_duration_left = 0;
  b_note_active_next_left = TRUE;
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
  if( current_song_index == 0 )
  {
    current_song_index = SONG_LIST_SIZE - 1;
  }
  else
  {
    current_song_index--;
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
  if( current_song_index == ( SONG_LIST_SIZE - 1 ) )
  {
    current_song_index = 0;
  }
  else
  {
    current_song_index++;
  }

  // Reset relevant variables
  ResetBuzzerVariables();

  // Automatically move to new state
  MusicPlayer_StateMachine = MusicPlayerSM_Play;
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

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void MusicPlayerSM_Error(void)
{
}