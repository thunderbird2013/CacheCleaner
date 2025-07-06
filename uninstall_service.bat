@echo off
set SERVICE_NAME=CacheCleanerService

echo.
echo Stopping and deleting service "%SERVICE_NAME%" ...
sc stop %SERVICE_NAME% >nul 2>&1
sc delete %SERVICE_NAME% >nul 2>&1

echo Fertig.
pause

