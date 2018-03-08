# midi_track_splitter.py
# Description: Takes an input MIDI file and splits each track into its own separate MIDI file.
# By: Ivan Chow
# March 5, 2018

import argparse
import os
import mido
from mido import MidiFile, MidiTrack

# Parses the input mide file into separate MIDI tracks
def parse_midi(mid_file):
    in_mid = MidiFile(mid_file)
    in_mid_basename = os.path.splitext(mid_file)[0]

    # Iterate through each track in file
    for i, in_track in enumerate(in_mid.tracks):
        # Create new MIDI file object
        out_mid = MidiFile()
        out_track = MidiTrack()
        out_mid.tracks.append(out_track)

        # Make ticks per beat consistent
        out_mid.ticks_per_beat = in_mid.ticks_per_beat

        print('Track {}: {}'.format(i, in_track.name))

        # Directly copy input track into output track
        for msg in in_track:
            out_track.append(msg)

        out_mid_file_name = in_mid_basename + "_track" + str(i) + "_" + in_track.name + ".mid"

        try:
            out_mid.save(out_mid_file_name)
        except IOError:
            print("Could not save file: " + out_mid_file_name)

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("mid_file", help="Input MIDI file to parse")
args = parser.parse_args()

print("\nParsing: " + args.mid_file + "\n")

# Parse the MIDI file
parse_midi(args.mid_file)