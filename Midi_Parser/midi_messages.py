# midi_messages.py
# Description: Simply prints the messages and information of an input MIDI screen to console.
#              Recommended to redirect console output to a text file using the > operator in command line.
# By: Ivan Chow
# March 6, 2018

import argparse
import mido
from mido import MidiFile, MidiTrack

# Prints messages in a MIDI file to screen
def print_messages(mid_file):
    mid = MidiFile(mid_file)

    print('File length: {} sec.'.format(mid.length))
    print('Ticks per Beat: {}\n'.format(mid.ticks_per_beat))

    for i, track in enumerate(mid.tracks):
        print('Track {}: {}'.format(i, track.name))
        for msg in track:
            print(msg)

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("mid_file", help="Input MIDI file to parse")
args = parser.parse_args()

print("\nPrinting Messages in: " + args.mid_file)

# Parse the MIDI file
print_messages(args.mid_file)