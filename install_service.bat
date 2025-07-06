@echo off
set SERVICE_NAME=CacheCleanerService
set SERVICE_PATH="H:\Devel\C++\CacheCleaner\CacheCleanerService.exe"

echo.
echo Installing service "%SERVICE_NAME%" ...
sc stop %SERVICE_NAME% >nul 2>&1
sc delete %SERVICE_NAME% >nul 2>&1

sc create %SERVICE_NAME% binPath= %SERVICE_PATH% start= demand DisplayName= "CacheCleaner Dienst"
sc description %SERVICE_NAME% "Dienst zum Leeren des Standby-Cache per Named Pipe."

echo.
echo Starting service...
sc start %SERVICE_NAME%

pause


