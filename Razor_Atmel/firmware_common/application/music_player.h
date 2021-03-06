/**********************************************************************************************************************
File: music_player.h
Description: Header file music_player.c
Author: Ivan Chow
Date: February 9, 2018
**********************************************************************************************************************/

#ifndef __MUSIC_PLAYER_H
#define __MUSIC_PLAYER_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/
void MusicPlayerInitialize(void);
void MusicPlayerRunActiveState(void);
u8 MusicPlayerGetCurrentSongIndex(void);
const char* MusicPlayerGetCurrentSongTitle(void);
const char* MusicPlayerGetCurrentSongArtist(void);
void MusicPlayerTogglePlayPause(void);
void MusicPlayerPreviousSong(void);
void MusicPlayerNextSong(void);

#endif /* __MUSIC_PLAYER_H */
