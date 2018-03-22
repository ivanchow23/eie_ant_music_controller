#!/usr/bin/env python

# midi_track_splitter.py
# Description: Takes an input MIDI file and splits each track into its own separate MIDI file.
# By: Ivan Chow
# March 5, 2018

import argparse
import os
import mido
from mido import MidiFile, MidiTrack, MetaMessage

# Parses the input midi file into separate MIDI tracks
def parse_midi(mid_file, set_tempo):
    in_mid = MidiFile(mid_file)
    in_mid_basename = os.path.splitext(mid_file)[0]

    base_tempo = 0
    
    if(set_tempo == 0):
        # Find the tempo of the MIDI file
        # Note: This is not ideal because there could be changes in tempo during the song
        #       However, this is to try giving tracks without the tempo messages consistency with other tracks that do
        base_tempo = find_tempo(in_mid)
    else:
        base_tempo = set_tempo
        
    print('Using {} us/beat as tempo for tracks without a tempo message.\n'.format(base_tempo))

    # Iterate through each track in file
    for i, in_track in enumerate(in_mid.tracks):
        # Create new MIDI file object
        out_mid = MidiFile()
        out_track = MidiTrack()
        out_mid.tracks.append(out_track)

        # Make ticks per beat consistent
        out_mid.ticks_per_beat = in_mid.ticks_per_beat

        # Check if this current track has a tempo message in it
        # This is so original tracks that don't have a tempo message gets one
        if not has_tempo(in_track):
            out_track.append(MetaMessage('set_tempo', tempo=base_tempo))

        print('Track {}: {}'.format(i, in_track.name))

        # Directly copy input track into output track
        for msg in in_track:
            out_track.append(msg)

        out_mid_file_name = in_mid_basename + "_track" + str(i) + "_" + in_track.name + ".mid"

        try:
            out_mid.save(out_mid_file_name)
        except IOError:
            print("Could not save file: " + out_mid_file_name)

# A fairly inefficient way to get the latest tempo message of the file.
def find_tempo(midi_file):
    tempo = 0
    
    for i, track in enumerate(midi_file.tracks):
        for msg in track:
            msg_dict = msg.dict()

            if 'tempo' in msg_dict:
                tempo = msg_dict['tempo']
                
    return tempo

# A fairly inefficient way to check if a track already has a tempo message in it
def has_tempo(track):
    for msg in track:
        msg_dict = msg.dict()

        if 'tempo' in msg_dict:
            return True

    return False

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("mid_file", help="Input MIDI file to parse")
parser.add_argument("-tempo", type=int, default=0, help="Tempo in us/beat")
args = parser.parse_args()

print("\nParsing: " + args.mid_file)

# Parse the MIDI file
parse_midi(args.mid_file, args.tempo)