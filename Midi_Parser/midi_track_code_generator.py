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

# Parses the given MIDI file and returns a list of notes (in frequency) and note durations (in milliseconds)
# If MIDI file is not specified, returns a list containing 0 frequency and some abitrary duration
def parse_notes_and_duration(input_mid_file):
    # No input file
    if input_mid_file is None:
        return [(0, 1000)]

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
                    note_freq = midi_note_to_freq(note_val)

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
def midi_note_to_freq(m):
    freq = ( 2**( (m - 69) / 12.0 ) ) * 440
    return int(round(freq))

# Generates code with the given input information
# Takes in the song number, song title and artist strings, and list of notes and corresponding durations for each buzzer
def generate_code(song_num, song_title, song_artist, notes_right, notes_left):
    file_name = "song" + str(song_num) + ".c"
    out = open(file_name, 'a')

    # Generate notes array for right buzzer
    # Note: Need to pad beginning with silent note to line-up delta times
    out.write("static const u16 song{}_note_right[] = {{ 0".format(song_num))
    for note_info in notes_right:
        out.write(", {}".format(note_info[0]))

    out.write(" };\n")

    # Generate notes duration array for right buzzer
    # Note: Need to pad ending with an arbitrary duration to line-up delta times
    out.write("static const u16 song{}_note_duration_right[] = {{ ".format(song_num))
    for note_info in notes_right:
        out.write("{}, ".format(note_info[1]))

    out.write("1000 };\n")

    # Generate notes array for left buzzer
    # Note: Need to pad ending with an arbitrary duration to line-up delta times
    out.write("static const u16 song{}_note_left[] = {{ 0".format(song_num))
    for note_info in notes_left:
        out.write(", {}".format(note_info[0]))

    out.write(" };\n") # Pad ending

    # Generate notes duration array for left buzzer
    # Note: Need to pad beginning with silent note to line-up delta times
    out.write("static const u16 song{}_note_duration_left[] = {{ ".format(song_num))
    for note_info in notes_left:
        out.write("{}, ".format(note_info[1]))

    out.write("1000 };\n") # Pad ending

    # Generate information structure for this song
    song_prefix_str = "song" + str(song_num)
    out.write("\nstatic const SongInfoType {} = {{ \"{}\", \"{}\", ".format(song_prefix_str, song_title, song_artist))
    out.write("{}_note_right, {}_note_duration_right, sizeof( {}_note_right ) / sizeof( {}_note_right[0] ), ". format(song_prefix_str, song_prefix_str, song_prefix_str, song_prefix_str))
    out.write("{}_note_left, {}_note_duration_left, sizeof( {}_note_left ) / sizeof( {}_note_left[0] ) }};\n".format(song_prefix_str, song_prefix_str, song_prefix_str, song_prefix_str))

    out.close()

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("-b1", help="Input MIDI track file to be played on buzzer 1 (right buzzer) of the EiE board")
parser.add_argument("-b2", help="Input MIDI track file to be played on buzzer 2 (left buzzer) of the EiE board")
args = parser.parse_args()

if args.b1 is not None:
    print("\nParsing: {} for buzzer 1 (right buzzer)".format(args.b1))
else:
    print("\nBuzzer 1 (right buzzer) will not play anything.")

if args.b2 is not None:
    print("Parsing: {} for buzzer 2 (left buzzer)".format(args.b2))
else:
    print("Buzzer 2 (left buzzer) will not play anything.")

# Parse the MIDI files for notes and corresponding durations into a list
# If an input is not specified, it will return a list containing: 0 frequency and some arbitrary time (so buzzer will not play when loaded in firmware)
notes_list_right = parse_notes_and_duration(args.b1)
notes_list_left = parse_notes_and_duration(args.b2)

# Prompt user to enter song information for generating code variables
song_title = raw_input("Enter the song title: ")
song_artist = raw_input("Enter the song artist: ")
song_number = int(input("Enter the song number (for generating code variables): "))

generate_code(song_number, song_title, song_artist, notes_list_right, notes_list_left)
print("\nDone.")