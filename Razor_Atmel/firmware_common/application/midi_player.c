/**********************************************************************************************************************
File: midi_player.c
Description: Prototype for playing MIDI music on the board.
Author: Ivan Chow
Date: March 7, 2018
------------------------------------------------------------------------------------------------------------------------
API:
**********************************************************************************************************************/

#include "configuration.h"

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/

/***********************************************************************************************************************
Existing variables (defined in other files -- should all contain the "extern" keyword)
***********************************************************************************************************************/
extern volatile u32 G_u32SystemTime1ms;         /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;          /* From board-specific source file */

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
typedef struct
{
  const char* title;
  const char* artist;
  const u16*  note_right;           /* Array of note frequencies for right buzzer */
  const u16*  note_duration_right;  /* Array of note durations for right buzzer*/
  const u16   num_notes_right;      /* Number of notes for right buzzer */
  const u16*  note_left;            /* Array of note frequencies for left buzzer */
  const u16*  note_duration_left;   /* Array of note durations for left buzzer*/
  const u16   num_notes_left;       /* Number of notes for left buzzer */
} SongInfoType;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
***********************************************************************************************************************/
static fnCode_type MidiPlayer_StateMachine;    /* The state machine function pointer */

/***********************************************************************************************************************
Local functions
***********************************************************************************************************************/
static void ResetBuzzerVariables(void);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void MidiPlayerSM_Play(void);

static const u16 song1_note_right[] =          { 330, 294, 262, NO, 330, 294, 262, NO, 262, NO, 262, NO, 262, NO, 262, NO, 294, NO, 294, NO, 294, NO, 294, NO, 330, 294, 262, NO };
static const u16 song1_note_duration_right[] = { QN,  QN,  QN,  QN, QN,  QN,  QN,  QN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, QN,  QN,  QN,  HN };
static const u16 song1_note_left[] =           { 330, 294, 262, NO, 330, 294, 262, NO, 262, NO, 262, NO, 262, NO, 262, NO, 294, NO, 294, NO, 294, NO, 294, NO, 330, 294, 262, NO };
static const u16 song1_note_duration_left[] =  { QN,  QN,  QN,  QN, QN,  QN,  QN,  QN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, SN,  SN, QN,  QN,  QN,  HN };

static const SongInfoType song1 = { "Hot Cross Buns", "?",
                                    song1_note_right, song1_note_duration_right, sizeof( song1_note_right ) / sizeof( song1_note_right[0] ),
                                    song1_note_left, song1_note_duration_left, sizeof( song1_note_left ) / sizeof( song1_note_left[0] )
                                  };

/* List of songs */
static const SongInfoType* song_list[] = { &song1 };

static u8 song_index = 0;

/* Right buzzer variables */
static u32 buzzer_right_timer = 0;
static u16 current_note_duration_right = 0;
static u32 note_right_index = 0;

/* Left buzzer variables */
static u32 buzzer_left_timer = 0;
static u16 current_note_duration_left = 0;
static u32 note_left_index = 0;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: MidiPlayerInitialize

Description:
  Initializes the State Machine and its variables.
*/
void MidiPlayerInitialize(void)
{
  ResetBuzzerVariables();
  MidiPlayer_StateMachine = MidiPlayerSM_Play;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: MidiPlayerRunActiveState

Description:
  Selects and runs one iteration of the current state in the state machine.
  All state machines have a TOTAL of 1ms to execute, so on average n state machines
  may take 1ms / n to execute.
*/
void MidiPlayerRunActiveState(void)
{
  MidiPlayer_StateMachine();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Resets buzzer variables. */
static void ResetBuzzerVariables(void)
{
  /* Right buzzer variables */
  buzzer_right_timer = G_u32SystemTime1ms;
  note_right_index = 0;
  current_note_duration_right = song_list[song_index]->note_duration_right[note_right_index];

  /* Left buzzer variables */
  buzzer_left_timer = G_u32SystemTime1ms;
  note_left_index = 0;
  current_note_duration_left = song_list[song_index]->note_duration_left[note_left_index];
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Plays the current song. */
static void MidiPlayerSM_Play(void)
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

    buzzer_left_timer = G_u32SystemTime1ms;
    current_note_duration_left = song_list[song_index]->note_duration_left[note_left_index];
  }
}