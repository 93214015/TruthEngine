@echo off
echo "The project needs boost library. please enter the path of include files"
echo.
set /P BoostIncPath=
echo.
echo. "please enter the lib path"
echo.
set /P BoostLibPath=


CALL "Dependencies\bin\premake5.exe" vs2019 --BoostIncPath=%BoostIncPath% --BoostLibPath=%BoostLibPath%
PAUSE