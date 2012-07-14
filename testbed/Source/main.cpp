#include <windows.h>
#include <stdio.h>

int wmain( int argc, wchar_t *argv[], wchar_t *envp[] )
{
  wprintf_s( L"Todo\r\n" );
  return EXIT_SUCCESS;
}