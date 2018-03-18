-------------------------------- midi_messages.py --------------------------------

Purpose: To print out all the messages contained in a particular MIDI file. 
         Mainly used for debugging purposes.
         
Usage: midi_messages.py <input midi file>

----------------------------- midi_track_splitter.py -----------------------------

Purpose: Takes a MIDI file and splits all tracks into individual MIDI files.
         This is to help listen to which track would sound suitable to be 
         put on the EiE board.
     
Usage: midi_track_splitter.py <input midi file>

-------------------------------- midi_code_gen.py --------------------------------

Purpose: Generates code to be copied and pasted into the EiE firmware based
         on up to 2 MIDI track file inputs. Typically, you would run 
         the midi_track_splitter script, choose the desired tracks to use,
         then run this script. Also allows for pass-in arguments for shifting
         notes, because buzzers on the board can't play lower frequencies.
         
Usage: midi_code_gen.py -b1 <input file for buzzer1> -b2 <input file for buzzer2>
                        -n1 <note shift> -n2 <note shift>