// midieditor.hpp - header file for midieditor.cpp
// Copyright (C) 2018 John Davies
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//=========================================================================

#ifndef MIDIEDITOR_H
#define MIDIEDITOR_H

// From https://www.csie.ntu.edu.tw/~r92092/ref/midi/
// MIDI Message definitions

#define NOTE_OFF              0x80
#define NOTE_ON               0x90
#define POLYPHONIC_PRESSURE   0xA0
#define CONTROLLER_CHANGE     0xB0
#define PROGRAM_CHANGE        0xC0
#define CHANNEL_KEY_PRESSURE  0xD0
#define PITCH_BEND            0xE0
#define SYSTEM_EXCLUSIVE      0xF0

#define MIDI_MESSAGE_MASK     0xF0
#define MIDI_CHANNEL_MASK     0x0F

// Meta Event definitions

#define SEQUENCE_NUMBER     0x00
#define TEXT_EVENT          0x01
#define COPYRIGHT_NOTICE    0x02
#define SEQUENCE_TRACK_NAME 0x03
#define INSTRUMENT_NAME     0x04
#define LYRIC               0x05
#define MARKER              0x06
#define CUE_POINT           0x07
#define MIDI_CHANNEL_PREFIX 0x20
#define END_OF_TRACK        0x2F
#define SET_TEMPO           0x51
#define SMTPE_OFFSET        0x54
#define TIME_SIGNATURE      0x85
#define KEY_SIGNATURE       0x59
#define SEQUENCER_SPECIFIC  0x7F


#endif
