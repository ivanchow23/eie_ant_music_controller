#!/usr/bin/env python

# midi_track_code_generator.py
# Description: Takes 2 input MIDI track files and generates code for the EiE firmware.
#              - 2 input MIDI files (ideally each containing a single track) for 2 buzzers on the EiE board.
#              - If an input is left blank, a "NO" note will be generated for that buzzer.
#              - Notes are translated into frequencies.
#              - Note times are re-calculated into milliseconds.
# By: Ivan Chow
# March 9, 2018

import argparse
import os
import mido
from mido import MidiFile, MidiTrack

LOW_NOTE_FREQ_THRESHOLD = 100

# Parses the given MIDI file and returns a list of notes (in frequency) and note durations (in milliseconds)
# If MIDI file is not specified, returns an empty list
def parse_notes_and_duration(input_mid_file, note_shift):
    # No input file
    if input_mid_file is None:
        return []

    # Open MIDI file and get some of its information
    in_mid = MidiFile(input_mid_file)
    ticks_per_beat = in_mid.ticks_per_beat
    tempo = find_tempo(in_mid)

    notes_list = []

    # Parse messages
    for i, track in enumerate(in_mid.tracks):
        for msg in track:
            # Convert message to dictionary type
            msg_dict = msg.dict()

            # Look for "note on" or "note off" messages
            if msg.type is 'note_on':
                # Look for delta time for this message, convert to ms
                delta_time_ticks = int(msg_dict['time'])
                delta_time_ms = midi_ticks_to_ms(delta_time_ticks, ticks_per_beat, tempo)

                # 0 velocity is the same as "note off"
                if int(msg_dict['velocity']) is 0:
                    notes_list.append( (0, delta_time_ms) )
                else:
                    # Convert note message number into frequency
                    note_val = int(msg_dict['note'])
                    note_freq = midi_note_to_freq(note_val, note_shift)

                    notes_list.append( (note_freq, delta_time_ms) )

            elif msg.type is 'note_off':
                # Look for delta time for this message, convert to ms
                delta_time_ticks = int(msg_dict['time'])
                delta_time_ms = midi_ticks_to_ms(delta_time_ticks, ticks_per_beat, tempo)

                notes_list.append( (0, delta_time_ms) )

    return notes_list

# A fairly inefficient way to get the first tempo message of the entire MIDI file
def find_tempo(midi_file):
    for i, track in enumerate(midi_file.tracks):
        for msg in track:
            msg_dict = msg.dict()

            if 'tempo' in msg_dict:
                return msg_dict['tempo']

# Converts the number of ticks into milliseconds based on the given tempo (in us/beat) and ticks per beat
# Rounded to the nearest integer
# A = Number of ticks, B = ticks per beat, C = tempo (us/beat)
#
#                   beat      C us     1 ms
# t_ms = A ticks x ------- x ------ x -------
#                  B ticks    beat    1000 us
#
def midi_ticks_to_ms(ticks, ticks_per_beat, tempo):
    t_ms = ( ticks * ( 1.0 / ticks_per_beat ) * ( tempo ) ) / 1000.0
    return int(round(t_ms))

# Converts a MIDI note value into frequency
# Rounded to the nearest integer
# Reference: https://newt.phys.unsw.edu.au/jw/notes.html
def midi_note_to_freq(m, note_shift):
    freq = ( 2**( ( (m + note_shift) - 69 ) / 12.0 ) ) * 440
    return int(round(freq))

# Generates code with the given input information
# Takes in the song number, song title and artist strings, and list of notes and corresponding durations for each buzzer
def generate_code(song_num, song_title, song_artist, notes_right, notes_left):
    file_name = "song" + str(song_num) + ".h"
    out = open(file_name, 'w')

    # Auto-generate the header
    out.write("// File: {}\n".format(file_name))
    out.write("// Auto-generated song information using: midi_track_code_generator.py\n")
    out.write("// !! DO NOT EDIT !!\n\n")
    out.write("// Instructions:\n")
    out.write("//   1. Copy and paste the following code into songs.h\n")
    out.write("//   2. Add the \"song#\" variable into the song_list[] variable\n\n")

    out.write("/* Song #{} */\n".format(song_num))

    # Check lengths of each song based on delta note times
    right_duration = get_note_array_duration_ms(notes_right)
    left_duration = get_note_array_duration_ms(notes_left)
    right_buzzer_pad_duration_ms = 1000
    left_buzzer_pad_duration_ms = 1000

    # If the right buzzer finishes early, pad the end array so it syncs up with the left buzzer
    if(right_duration < left_duration):
        right_buzzer_pad_duration_ms = right_buzzer_pad_duration_ms + (left_duration - right_duration)

    # Same idea if the left buzzer finishes early
    if(left_duration < right_duration):
        left_buzzer_pad_duration_ms = left_buzzer_pad_duration_ms + (right_duration - left_duration)

    # Generate notes array for right buzzer
    out.write("static const u16 song{}_note_right[] = ".format(song_num))
    num_notes = write_notes_to_formatted_array(out, notes_right)
    print("\nRight buzzer: {} notes.".format(num_notes))

    out.write("static const u16 song{}_note_duration_right[] = ".format(song_num))
    num_notes = write_notes_duration_to_formatted_array(out, notes_right, right_buzzer_pad_duration_ms)
    print("Right buzzer: {} note durations.".format(num_notes))

    # Generate notes array for left buzzer
    out.write("static const u16 song{}_note_left[] = ".format(song_num))
    num_notes = write_notes_to_formatted_array(out, notes_left)
    print("Left buzzer: {} notes.".format(num_notes))

    out.write("static const u16 song{}_note_duration_left[] = ".format(song_num))
    num_notes = write_notes_duration_to_formatted_array(out, notes_left, left_buzzer_pad_duration_ms)
    print("Left buzzer: {} note durations.".format(num_notes))

    # Generate information structure for this song
    song_prefix_str = "song" + str(song_num)
    out.write("\nstatic const SongInfoType {} = {{ \"{}\", \"{}\", ".format(song_prefix_str, song_title, song_artist))
    out.write("{}_note_right, {}_note_duration_right, sizeof( {}_note_right ) / sizeof( {}_note_right[0] ), ". format(song_prefix_str, song_prefix_str, song_prefix_str, song_prefix_str))
    out.write("{}_note_left, {}_note_duration_left, sizeof( {}_note_left ) / sizeof( {}_note_left[0] ) }};\n".format(song_prefix_str, song_prefix_str, song_prefix_str, song_prefix_str))

    out.close()

# Counts the duration of the given notes list based on its delta ticks in milliseconds
def get_note_array_duration_ms(list):
    duration_ms = 0

    for item in list:
        note_dur = item[1]
        duration_ms += int(note_dur)

    return duration_ms

# Converts list into formatted C-array for notes
# Handles padding the first element in array with "NO" note to align delta ticks properly
# Returns number of notes detected from list and written
def write_notes_to_formatted_array(out, list):
    # Pad beginning with 0 to align delta ticks
    out.write("{ 0")

    # Start at 1 to account for note pad at the beginning
    notes_counter = 1

    for item in list:
        note = item[0]
        notes_counter += 1

        # Limit how many notes can be on a line
        if((notes_counter % 1000) == 0):
            out.write(",\n")
            out.write("{}".format(note))
        else:
            out.write(", {}".format(note))

    out.write(" };\n")

    return notes_counter

# Converts list into formatted C-array for note durations
# Handles padding the last element in array with a short duration to align delta ticks properly
# Returns number of notes detected from list and written
def write_notes_duration_to_formatted_array(out, list, end_padding_duration):
    out.write("{ ")

    # Start at 1 to account for note duration pad at the end
    notes_counter = 1

    for item in list:
        note_dur = item[1]
        notes_counter += 1

        # Limit how many notes can be on a line
        if((notes_counter % 1000) == 0):
            out.write("\n")
            out.write("{}, ".format(note_dur))
        else:
            out.write("{}, ".format(note_dur))

    # Pad ending with a short duration to align delta ticks
    out.write("{} }};\n".format(end_padding_duration))

    return notes_counter

# Checks and prints the lowest and highest note frequency in the list
# Prints a warning if frequency falls below the threshold
def check_note_frequencies(notes_list, buzzer_id):
    # Empty list - ignore.
    if not notes_list:
        return

    min = 65535
    max = 0

    for item in notes_list:
        # Ignore intended silent notes
        if(item[0] == 0):
            continue

        if(item[0] < min):
            min = item[0]

        if(item[0] > max):
            max = item[0]

    print("Buzzer {}: Min. Frequency (Not incl. silent notes) = {} Hz. | Max. Frequency = {} Hz.".format(buzzer_id, min, max))

    if(min < LOW_NOTE_FREQ_THRESHOLD):
        print("Warning! Buzzer {} lowest non-zero frequency is below the {} Hz threshold. Note may not play properly.".format(buzzer_id, LOW_NOTE_FREQ_THRESHOLD))


# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("-b1", help="Input MIDI track file to be played on buzzer 1 (right buzzer) of the EiE board")
parser.add_argument("-b2", help="Input MIDI track file to be played on buzzer 2 (left buzzer) of the EiE board")
parser.add_argument("-n1", type=int, default=0, help="Shift MIDI track note for buzzer 1 (right buzzer). Inputs can be 0, 1, 2, -1, etc.")
parser.add_argument("-n2", type=int, default=0, help="Shift MIDI track note for buzzer 2 (left buzzer). Inputs can be 0, 1, 2, -1, etc.")
args = parser.parse_args()

if args.b1 is not None:
    print("\nParsing: {} for buzzer 1 (right buzzer) with note offset {}".format(args.b1, args.n1))
else:
    print("\nParsing: Nothing for buzzer 1. It will not play anything.")

if args.b2 is not None:
    print("Parsing: {} for buzzer 2 (left buzzer) with note offset {}\n".format(args.b2, args.n2))
else:
    print("Parsing: Nothing for buzzer 2. It will not play anything.\n")

# Parse the MIDI files for notes and corresponding durations into a list
# If an input is not specified, it will return an empty list
notes_list_right = parse_notes_and_duration(args.b1, args.n1)
notes_list_left = parse_notes_and_duration(args.b2, args.n2)

# Check to make sure notes are not below a certain frequency - found that the EiE board struggles with playing frequencies below 100 Hz.
check_note_frequencies(notes_list_right, 1)
check_note_frequencies(notes_list_left, 2)

# Prompt user to enter song information for generating code variables
song_title = raw_input("\nEnter the song title: ")
song_artist = raw_input("Enter the song artist: ")
song_number = int(input("Enter the song number (for generating code variables): "))

generate_code(song_number, song_title, song_artist, notes_list_right, notes_list_left)

print("\nDone.")