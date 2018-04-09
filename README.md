# midieditor
This is a command line based editor for doing bulk edits to MIDI files. Its original use was for tweaking the output of LMMS ( https://lmms.io/ ) MIDI export to allocate tracks to MIDI channels etc.

## Usage

Usage is as follows:

```
midieditor <options> <input file>

where:
-h/--help
    Print this information
-i/--info
    Print information about this file
-t/--track-name <track>,<name>
    Sets track name
-c/--channel <track>,<channel>
    Specifies MIDI channel for track
-p/--program-change <track>,<program>
    Adds a program change message to start of track
-n/--note-set <track,<note>
    Set all notes of a track to the specified value
-v/--volume <track>,<multiplier>
    Modifies volume for notes in track
-o/--output-file [name]
    Write data to [name] otherwise overwrite input file
```

Note: MIDI channels are specified as 1 to 16

## Build instructions

*Important note - midieditor uses the "Midifile: C++ MIDI file parsing library" ( https://github.com/craigsapp/midifile ) to do the low level file manipulation. However some extra features are needed which are currently only available from a branch on my fork ( https://github.com/john-davies/midifile/tree/add_remove_event ). Until this functionality is merged into the original then this branch should be used or midieditor will not build correctly.*

1. Check out the midifile branch referenced above and build it.

2. Check out the midieditor code

3. Copy the following files from the midifile directory to the midieditor directory:
  * libmidifile.a
  * MidiEvent.h
  * MidiEventList.h
  * MidiFile.h
  * MidiMessage.h

4. Type `make` and a single executable called `midieditor` should be created.

5. Run `./midieditor -h` to show the help file

## Examples

1. `./midieditor -i test.midi` shows information about the different tracks in `test.midi`
2. `./midieditor -c 1,2 test.midi` allocates track 1 in the MIDI file to MIDI channel 2 and overwrites the original file.
3. `./midieditor -p 3,65 -o new.midi test.midi` adds a program change event ( 65 decimal ) to the start of track 3 and writes the output to a new file called `new.midi`.
4. `./midieditor -n 4,76 test.midi` sets all notes in track 4 to be value 76 decimal. ( This is mainly useful for editing percussion tracks. )
