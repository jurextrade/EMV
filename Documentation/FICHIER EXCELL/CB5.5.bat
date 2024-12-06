@ECHO OFF &SETLOCAL
set "LineNr="
for /f "tokens=1*delims=[]" %%a in ('^<"%~1" find /i /n "%~2"') do if not defined LineNr (
    set "LineNr=%%a"
    SET "Line=%%b"
)
if not defined LineNr (
    set "LineNr=1"
    SET "Row=1"
    GOTO :launch
)
CALL SET "Right=%~2%%Line:*%~2=%%"
CALL SET "Line=%%Line:%Right%=%%"
FOR /f "delims=:" %%a in ('"(echo("%Line%"&@echo()|findstr /o $"') do SET /a Row=%%a-4
:launch
START /b CMD /c ""%ProgramFiles(x86)%\Notepad++\notepad++.exe" -n%LineNr% -c%Row% "%~1""
