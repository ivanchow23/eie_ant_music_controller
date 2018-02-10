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
  DebugPrintf("Hello world!\n");
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

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Plays the current song. */
static void MusicPlayerSM_Play(void)
{
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