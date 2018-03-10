/**********************************************************************************************************************
File: songs.h
Description: Header file containing song information to be played by the music application.
Author: Ivan Chow
Date: March 10, 2018
**********************************************************************************************************************/

#ifndef __SONGS_H
#define __SONGS_H

#include "configuration.h"

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

/**********************************************************************************************************************
Song Notes, Durations, and Information
**********************************************************************************************************************/

/* Song #1 */
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

#endif /* __SONGS_H */
