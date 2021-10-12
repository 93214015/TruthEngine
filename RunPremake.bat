@echo off
echo "The project needs boost library. please enter the path of include files"
echo.
set /P BoostIncPath=
echo.
echo. "please enter the lib path"
echo.
set /P BoostLibPath=
echo.
echo "Please enter the premake's executable file path"
set /P PremakeExe=


CALL %PremakeExe% vs2019 --BoostIncPath=%BoostIncPath% --BoostLibPath=%BoostLibPath%
PAUSE