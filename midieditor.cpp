// midieditor.cpp - basic script based MIDI file editor
// Copyright (C) 2018 John Davies
//
// Usage:
//
// midieditor <options> <input file>
//
// where options:
//	-h/--help = prints help message
// 	-i/--info = print data about file
//	-t/--track_name <track>,<name> = sets track name
//	-c/--channel <track>,<channel> = specifies MIDI channel for track
//	-p/--program-change <track>,<program> = adds a program change message
//											to track
//	-v/--volume <track>,<amount> = modifies volume for notes in track
//  -n/--note-set <track,<note> = set all notes of a track to the specified value
//	-o/--output-file <name> = write data to <name> otherwise overwrite
//							  input file
// Note: MIDI channels are specified as 1 to 16

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

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <getopt.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include "MidiFile.h"
#include "midieditor.hpp"

using namespace std;

//=========================================================================

MidiFile midifile;

//=========================================================================

// String splitter
// Based on https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
vector<string> split( const string inputString, char delimiter )
{
  vector<string> params;
  stringstream inputStream;
  inputStream.str( inputString );
  std::string item;
  while( getline( inputStream, item, delimiter ) )
  {
    // Note: exclude \r because files may come from Windows systems
    // and will create an extra parameter if not filtered out
    if( item != "" && item != "\r" )
    {
      params.push_back( item );
    }
  }

  return params;
}

//=========================================================================

bool get_1_int_1_string( string input, int& value1, string& value2  )
{
    bool result = true;
    vector<string> params = split( input, ',' );

    try
    {
        value1 = stoi( params.at(0) );
        value2 = params.at(1);
    }
    catch( const std::invalid_argument& e )
    {
        // Conversion failed so mark result as invalid
        result = false;
    }
    catch( const std::out_of_range& e )
    {
        // Conversion failed so mark result as invalid
        result = false;
    }

    return result;
}

//=========================================================================

bool get_2_ints( string input, int& value1, int& value2  )
{
    bool result = true;
    vector<string> params = split( input, ',' );

    try
    {
        value1 = stoi( params.at(0) );
        value2 = stoi( params.at(1) );
    }
    catch( const std::invalid_argument& e )
    {
        // Conversion failed so mark result as invalid
        result = false;
    }
    catch( const std::out_of_range& e )
    {
        // Conversion failed so mark result as invalid
        result = false;
    }

    return result;
}

//=========================================================================

bool get_1_int_1_float( string input, int& value1, float& value2  )
{
    bool result = true;
    vector<string> params = split( input, ',' );

    try
    {
        value1 = stoi( params.at(0) );
        value2 = stof( params.at(1) );
    }
    catch( const std::invalid_argument& e )
    {
        // Conversion failed so mark result as invalid
        result = false;
    }
    catch( const std::out_of_range& e )
    {
        // Conversion failed so mark result as invalid
        result = false;
    }

    return result;
}

//=========================================================================

void print_help()
{
  cout << "Program usage:" << endl;
  cout << "  midieditor <options> <input file>" << endl;
  cout << "where:" << endl;
  cout << "  -h/--help = Print this information" << endl;
  cout << "  -i/--info = Print information about this file" << endl;
  cout << "  -t/--track-name <track>,<name> = sets track name" << endl;
  cout << "  -c/--channel <track>,<channel> = specifies MIDI channel for track" << endl;
  cout << "  -p/--program-change <track>,<program> = adds a program change message to start of track" << endl;
  cout << "  -n/--note-set <track,<note> = set all notes of a track to the specified value" << endl;
  cout << "  -v/--volume <track>,<multiplier> = modifies volume for notes in track" << endl;
  cout << "  -o/--output-file [name] = write data to [name] otherwise overwrite input file" << endl;
  cout << " Note: MIDI channels are specified as 1 to 16" << endl;
}

//=========================================================================

void print_info()
{
  string track_name;
  map <int, int> c;
  map <int, int> n;
  int program_change = -1;

  cout << "File info: " << endl;
  cout << "Tracks: " << midifile.getTrackCount() << " [name/prog change/channel(s)/note(s)]" << endl;
  // Step through the tracks
  for( int i=0; i<midifile.getTrackCount(); i++)
  {
    cout << i << " : ";
    // Step through the events
    track_name.clear();
    c.clear();
    n.clear();
    program_change = -1;

    for( int j=0; j<midifile[i].getSize(); j++)
    {
      // If it's a meta event
      if( midifile[i][j].isMeta() )
      {
        // and it's a track name message
        if( midifile[i][j][1] == 3 )
        {
          // Build up name string
          for( int k=0; k<midifile[i][j][2]; k++)
          {
            // Filter out non-printable ascii characters
            if( ( midifile[i][j][k+3] >= 0x20 ) &&
                ( midifile[i][j][k+3] < 0x7f ) )
            {
              track_name += midifile[i][j][k+3];
            }
            else
            {
              track_name += '.';
            }
          }
        }
      }
      // If it's a program change event
      if( ( midifile[i][j][0] & MIDI_MESSAGE_MASK ) == PROGRAM_CHANGE )
      {
        program_change = midifile[i][j][1];
      }
      // If it's an event that can specify a channel
      if( ( midifile[i][j][0] & MIDI_MESSAGE_MASK ) != SYSTEM_EXCLUSIVE )
      {
        c[ midifile[i][j][0] & MIDI_CHANNEL_MASK ] = 1;
      }
      // If it's a note on event
      if( ( midifile[i][j][0] & MIDI_MESSAGE_MASK ) == NOTE_ON )
      {
        n[ midifile[i][j][1] ] = 1;
      }
    }
    // Output the data
    cout << track_name << " / " << program_change << " / ";
    for (auto const& x : c )
    {
        std::cout << x.first + 1 << ' ';
    }
    cout << "/ ";
    for (auto const& x : n )
    {
        std::cout << x.first << ' ';
    }
    cout << "/ " << endl;
  }
}

//=========================================================================

bool track_name( int track_no, string name )
{
  // Sanity checks
  if( ( track_no < 0 ) ||
      ( track_no > midifile.getTrackCount() - 1  ) )
  {
    return false;
  }

  // Step through the events
  for( int j=0; j<midifile[track_no].getSize(); j++)
  {
    // If it's a meta event
    if( midifile[track_no][j].isMeta() )
    {
      // and it's a track name message
      if( midifile[track_no][j][1] == 3 )
      {
        // Delete this event
        midifile[track_no].remove( j );
        // Build the new event
        MidiEvent new_name( 0xFF, SEQUENCE_TRACK_NAME );
        int message_length = name.length() + 3;
        new_name.resize( message_length );
        new_name[2] = message_length-3;
        for(int i = 0; i < (int) name.length(); i++)
        {
          new_name[ i + 3 ] = name.at(i);
        }
        midifile[track_no].add( j, new_name );
      }
    }
  }

  return true;
}

//=========================================================================

bool program_change( int track_no, int prog_no )
{
  // Sanity checks
  if( ( track_no < 0 ) ||
      ( track_no > midifile.getTrackCount() - 1  ) )
  {
    return false;
  }

  if( ( prog_no < 0 ) || ( prog_no > 255 ) )
  {
    return false;
  }

  // Build the new event and add at the start of the track
  MidiEvent new_prog( PROGRAM_CHANGE, prog_no );
  midifile[track_no].add( 0, new_prog );

  return true;
}

//=========================================================================

bool note_set( int track_no, int note_no )
{
  // Sanity checks
  if( ( track_no < 0 ) ||
      ( track_no > midifile.getTrackCount() - 1  ) )
  {
    return false;
  }

  if( ( note_no < 0 ) || ( note_no > 127 ) )
  {
    return false;
  }

  // Step through the events
  for( int j=0; j<midifile[track_no].getSize(); j++)
  {
    // If it's a note on or off event then change the value
    if( ( ( midifile[track_no][j][0] & 0xF0 ) == NOTE_OFF ) ||
        ( ( midifile[track_no][j][0] & 0xF0 ) == NOTE_ON ) )
    {
      midifile[track_no][j][1] = note_no;
    }
  }

  return true;
}

//=========================================================================

bool volume_change( int track_no, float mult )
{
  // Sanity checks
  if( ( track_no < 0 ) ||
      ( track_no > midifile.getTrackCount() - 1  ) )
  {
    return false;
  }

  // Step through the events
  for( int j=0; j<midifile[track_no].getSize(); j++)
  {
    // If it's a note on event then change the volume
    if( ( midifile[track_no][j][0] & 0xF0 ) == NOTE_ON )
    {
      int vol = midifile[track_no][j][2];
      vol = (int) ( ( float ) vol * mult );
      // Limit the values
      if( vol > 127 )
      {
        vol = 127;
      }
      else if( vol < 0 )
      {
        vol = 0;
      }
      midifile[track_no][j][2] = vol;
    }
  }

  return true;
}

//=========================================================================

bool channel( int track_no, int channel_no )
{
  // Sanity checks
  if( ( track_no < 0 ) ||
      ( track_no > midifile.getTrackCount() - 1  ) )
  {
    return false;
  }

  if( ( channel_no < 1 ) || ( channel_no > 16 ) )
  {
    return false;
  }

  // Step through the events
  for( int j=0; j<midifile[track_no].getSize(); j++)
  {
    // If it's an event that can specify a channel
    // then set the channel
    if( ( midifile[track_no][j][0] & MIDI_MESSAGE_MASK ) != SYSTEM_EXCLUSIVE )
    {
      midifile[track_no][j][0] = midifile[track_no][j][0] | ( channel_no - 1 );
    }
  }

  return true;
}

//=========================================================================

int main( int argc, char *argv[] )
{

  cout << "MIDI Editor, V1.0" << endl;

	// Parse command line arguments
  int c;
  int option_index = 0;
  static struct option long_options[] =
  {
      { "help", no_argument, 0, 'h' },
      { "info", no_argument, 0, 'i' },
      { "track-name", required_argument, 0, 't' },
      { "channel", required_argument, 0, 'c' },
      { "program-change", required_argument, 0, 'p' },
      { "note-set", required_argument, 0, 'n' },
      { "volume", required_argument, 0, 'v' },
      { "output-file", required_argument, 0, 'o' },
      { 0, 0, 0, 0 }
  };

  // Program control variables
  bool help_flag = false;
  bool info_flag = false;
  bool track_name_flag = false;
  string track_name_option;
  bool channel_flag = false;
  string channel_option;
  bool program_change_flag = false;
  string program_change_option;
  bool note_set_flag = false;
  string note_set_option;
  bool volume_flag = false;
  string volume_option;
  bool output_file_flag = false;
  string output_file_option;

  while( 1 )
  {
    c = getopt_long( argc, argv, "hit:c:p:n:v:o:", long_options, &option_index );
    //Check for exit
    if( c == -1 )
    {
      break;
    }
    switch( c )
    {
      case 'h':
        help_flag = true;
        break;

      case 'i':
        info_flag = true;
        break;

      case 't':
        track_name_flag = true;
        track_name_option = optarg;
        break;

      case 'c':
        channel_flag = true;
        channel_option = optarg;
        break;

      case 'p':
        program_change_flag = true;
        program_change_option = optarg;
        break;

      case 'n':
        note_set_flag = true;
        note_set_option = optarg;
        break;

      case 'v':
        volume_flag = true;
        volume_option = optarg;
        break;

      case 'o':
        output_file_flag = true;
        output_file_option = optarg;
        break;

      case '?':
        // Error message already printed so ignore
        break;

      default:
        return( EXIT_FAILURE );
        break;
    }
  }

  // If help is needed then print here and exit
  if( help_flag == true )
  {
    print_help();
    return( EXIT_SUCCESS );
  }

  // Everything else needs the file for reading, file name is the last option
  // There needs to be at least three command line options so ignore if not
  if( argc < 3 )
  {
    // Error message
    cout << "ERROR - not enough command line options" << endl;
    return( EXIT_FAILURE );
  }

  // Attempt to open input file and read in
  string file_name( argv[argc-1] );
  if( midifile.read( file_name ) == 0 )
  {
    cout << "ERROR - unable to open file: " << file_name << endl;
    return( EXIT_FAILURE );
  }

  // Carry out options
  if( info_flag == true )
  {
    print_info();
    return( EXIT_SUCCESS );
  }

  if( track_name_flag == true )
  {
    int track_no;
    string name;
    // Get track value
    if( get_1_int_1_string( track_name_option, track_no, name  ) == false )
    {
      // Can't parse the command line option so exit
      cout << "ERROR - invalid track number: " << track_name_option << endl;
      return( EXIT_FAILURE );
    }
    else
    {
      if( track_name( track_no, name ) == false )
      {
        // Something went wrong so exit
        return( EXIT_FAILURE );
      }
    }
  }

  if( channel_flag == true )
  {
    int track_no;
    int channel_no;

    if( get_2_ints( channel_option, track_no, channel_no  ) == false )
    {
      // Can't parse the command line option so exit
      cout << "ERROR - invalid track number or channel number: " << channel_option << endl;
      return( EXIT_FAILURE );
    }

    if( channel( track_no, channel_no ) == false )
    {
      // Something went wrong so exit
      return( EXIT_FAILURE );
    }

  }

  if( program_change_flag == true )
  {
    int track_no;
    int prog_no;

    if( get_2_ints( program_change_option, track_no, prog_no  ) == false )
    {
      // Can't parse the command line option so exit
      cout << "ERROR - invalid track number or program change: " << program_change_option << endl;
      return( EXIT_FAILURE );
    }

    if( program_change( track_no, prog_no ) == false )
    {
      // Something went wrong so exit
      return( EXIT_FAILURE );
    }
  }

  if( note_set_flag == true )
  {
    int track_no;
    int note_no;

    if( get_2_ints( note_set_option, track_no, note_no  ) == false )
    {
      // Can't parse the command line option so exit
      cout << "ERROR - invalid track number or note value: " << program_change_option << endl;
      return( EXIT_FAILURE );
    }

    if( note_set( track_no, note_no ) == false )
    {
      // Something went wrong so exit
      return( EXIT_FAILURE );
    }
  }

  if( volume_flag == true )
  {
    int track_no;
    float mult;

    if( get_1_int_1_float( volume_option, track_no, mult  ) == false )
    {
      // Can't parse the command line option so exit
      cout << "ERROR - invalid track number or multiplier: " << program_change_option << endl;
      return( EXIT_FAILURE );
    }

    if( volume_change( track_no, mult ) == false )
    {
      // Something went wrong so exit
      return( EXIT_FAILURE );
    }

  }

  // Write out file
  if( output_file_flag == true )
  {
    midifile.write( output_file_option );
  }
  else
  {
    midifile.write( file_name );
  }

  return( EXIT_SUCCESS );
}
