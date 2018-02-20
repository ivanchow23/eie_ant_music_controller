/**********************************************************************************************************************
File: lcd_control.c
Description: Module that controls what is displayed on the LCD screen.
Author: Ivan Chow
Date: February 15, 2018
------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
  - void LcdControlInitialize(void)
      Runs required initialzation for the task. Should only be called once in main init section.

  - void LcdControlRunActiveState(void)
      Runs current task state. Should only be called once in main loop.
**********************************************************************************************************************/

#include "configuration.h"
#include <stdio.h>

/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define TITLE_BUFFER_SIZE           100
#define LCD_SCROLL_UPDATE_TIME_MS   200

/***********************************************************************************************************************
Existing variables (defined in other files -- should all contain the "extern" keyword)
***********************************************************************************************************************/
extern volatile u32 G_u32SystemTime1ms;         /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;          /* From board-specific source file */

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/
/* Holds information about what is displayed on the LCD screen */
typedef struct
{
  u8  title_buffer[TITLE_BUFFER_SIZE]; /* Holds the current song and artist string in a buffer for display */
  u8  title_size;                      /* Holds the size of the string stored in the title buffer */
  u8  title_display_start_index;       /* Start index of title string to display on LCD screen */
  u32 title_scroll_timer;              /* Holds a timer for when to update the scrolling title on LCD screen */
} LcdStateType;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
***********************************************************************************************************************/
static fnCode_type LcdControl_StateMachine;   /* The state machine function pointer */

static u8 lcd_button_banner[LCD_MAX_LINE_DISPLAY_SIZE] = "||>   <<    >>     ";

static u8 current_song_index;   /* Current song index */
static LcdStateType lcd_state;

/***********************************************************************************************************************
Local functions
***********************************************************************************************************************/
static void SetNewTitleString(void);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void LcdControlSM_DisplayInfo(void);
static void LcdControlSM_Error(void);

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: LcdControlInitialize

Description:
  Initializes the State Machine and its variables.
*/
void LcdControlInitialize(void)
{
  // Clear the screen to begin
  LCDCommand( LCD_CLEAR_CMD );

  // Invalidate the current song index to begin
  current_song_index = -1;

  // Begin timer for scrolling LCD feature
  lcd_state.title_scroll_timer = G_u32SystemTime1ms;

  // Display the button banner on LCD
  LCDMessage( LINE2_START_ADDR, lcd_button_banner );

  LcdControl_StateMachine = LcdControlSM_DisplayInfo;
}


/*----------------------------------------------------------------------------------------------------------------------
Function: LcdControlRunActiveState

Description:
  Selects and runs one iteration of the current state in the state machine.
  All state machines have a TOTAL of 1ms to execute, so on average n state machines
  may take 1ms / n to execute.
*/
void LcdControlRunActiveState(void)
{
  LcdControl_StateMachine();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Resets the LCD screen title buffer with the current song title and artist string */
/* Also resets variables to start LCD scrolling from beginning */
static void SetNewTitleString(void)
{
  u8 buf_index = 0;

  // Clear title buffer
  memset( lcd_state.title_buffer, 0, sizeof( lcd_state.title_buffer ) );

  // Store the current song and artist as a string in the title buffer
  buf_index += snprintf( lcd_state.title_buffer, TITLE_BUFFER_SIZE, "%s - %s", MusicPlayerGetCurrentSongTitle(), MusicPlayerGetCurrentSongArtist() );

  // If string is larger than LCD size, pad end with fixed number of spaces for a scrolling effect
  if( buf_index > LCD_MAX_LINE_DISPLAY_SIZE )
  {
    u8 space_pad[] = "     ";
    buf_index += snprintf( &lcd_state.title_buffer[buf_index], TITLE_BUFFER_SIZE, "%s", space_pad );
  }
  // If string is smaller than LCD size, pad end with spaces until it fills the display
  else
  {
    u8 num_space_pads = LCD_MAX_LINE_DISPLAY_SIZE - buf_index;

    for( u8 i = 0; i < num_space_pads; i++ )
    {
      buf_index += snprintf( &lcd_state.title_buffer[buf_index], TITLE_BUFFER_SIZE, "%s", " " );
    }
  }

  // Update variables for tracking which part of the string to show on LCD
  lcd_state.title_size = buf_index;
  lcd_state.title_display_start_index = 0;
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Displays current information, such a song name, play/pause status, ANT status, etc. */
static void LcdControlSM_DisplayInfo(void)
{
  // If the song has changed, we need to change the title being displayed
  if( current_song_index != MusicPlayerGetCurrentSongIndex() )
  {
    // Keep track of current song index
    current_song_index = MusicPlayerGetCurrentSongIndex();

    // Set a new title string to show on LCD screen
    SetNewTitleString();
  }

  // Display the title on LCD
  if( IsTimeUp( &lcd_state.title_scroll_timer, LCD_SCROLL_UPDATE_TIME_MS ) )
  {
    lcd_state.title_scroll_timer = G_u32SystemTime1ms;

    // Fit the whole title on LCD
    if( lcd_state.title_size <= LCD_MAX_LINE_DISPLAY_SIZE )
    {
      LCDMessage( LINE1_START_ADDR, lcd_state.title_buffer );
    }
    // Otherwise, scroll the title
    else
    {
      u8 temp_buffer[LCD_MAX_LINE_DISPLAY_SIZE];
      u8 display_index = lcd_state.title_display_start_index;

      // Display the characters of the title bounded by the size of the LCD
      for( u8 i = 0; i < LCD_MAX_LINE_DISPLAY_SIZE; i++ )
      {
        temp_buffer[i] = lcd_state.title_buffer[display_index];

        if( ++display_index >= lcd_state.title_size )
        {
          display_index = 0;
        }
      }

      LCDMessage( LINE1_START_ADDR, temp_buffer );

      // Update starting index of title for next time we scroll
      if( ++lcd_state.title_display_start_index >= lcd_state.title_size )
      {
        lcd_state.title_display_start_index = 0;
      }
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handles an error */
static void LcdControlSM_Error(void)
{
}