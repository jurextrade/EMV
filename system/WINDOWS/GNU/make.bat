set arg1=%1
echo %1
shift
shift

if %arg1% == all (
cd C:\PROJECTS_CROSS\EMV\Client\system\WINDOWS\GNU\EMVClient
mingw32-make clean64
mingw32-make EMVClient64

cd C:\PROJECTS_CROSS\EMV\Server\system\WINDOWS\GNU\EMVServer
mingw32-make clean64
mingw32-make EMVServer64

)