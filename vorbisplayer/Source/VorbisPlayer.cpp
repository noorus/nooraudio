#include <windows.h>
#include <stdio.h>
#include <nooraudio.h>
#include "VorbisAudioSource.h"

using namespace nooraudio;

std::wstring printTime( double time )
{
  WCHAR s[32];
  swprintf_s( s, 32, L"%01.0f:%02.0f:%02.2f\r\n",
    floor( time / 3600.0 ), floor( fmod( time, 3600.0 ) / 60.0 ), fmod( time, 60.0 )
  );
  return s;
}

int wmain( int argc, wchar_t *argv[], wchar_t *envp[] )
{
  if ( argc < 2 )
  {
    wprintf_s( L"Error: No filename given!\r\n" );
    return EXIT_FAILURE;
  }
  std::wstring file = argv[1];
  bool comInitialized = false;
  VorbisAudioSource* source = NULL;
  WASAPIAudioPlayer* player = NULL;
  try
  {
    if ( FAILED( CoInitializeEx( NULL, COINIT_MULTITHREADED ) ) )
      throw new std::exception( "CoInitializeEx failed" );
    comInitialized = true;

    source = new VorbisAudioSource( file, SampleSize_16bit );
    source->setLooping( true );

    StringMap tagMap = source->getTagMap();
    for ( StringMap::iterator it = tagMap.begin(); it != tagMap.end(); ++it )
    {
      if ( boost::iequals( (*it).first, L"ARTIST" ) )
        wprintf_s( L"- Artist: %s\r\n", (*it).second.c_str() );
      else if ( boost::iequals( (*it).first, L"TITLE" ) )
        wprintf_s( L"- Title: %s\r\n", (*it).second.c_str() );
    }

    wprintf_s( L"- Channels: %d\r\n- Samplerate: %dHz\r\n- Bitrate: %dkBps\r\n- Length: %s\r\n", 
      source->getChannels(), source->getSampleRate(), source->getBitRate() / 1024,
      printTime( source->getTotalTime() ).c_str() );

    player = new WASAPIAudioPlayer( source );
    wprintf_s( L"Press any key to stop.\r\n" );
    player->play();
    HANDLE input = GetStdHandle( STD_INPUT_HANDLE );
    SetConsoleMode( input, ENABLE_ECHO_INPUT );
    INPUT_RECORD rec;
    DWORD num;
    bool play = true;
    while ( play )
    {
      player->run();
      if ( WaitForSingleObject( input, 0 ) == WAIT_OBJECT_0 )
      {
        ReadConsoleInput( input, &rec, 1, &num );
        if ( num > 0 && rec.EventType == KEY_EVENT )
          play = false;
      }
    }
    player->stop();
  }
  catch( std::exception& e )
  {
    printf_s( "Exception: %s\r\n", e.what() );
  }

  SAFE_DELETE( player );
  SAFE_DELETE( source );

  if ( comInitialized )
    CoUninitialize();

  return EXIT_SUCCESS;
}