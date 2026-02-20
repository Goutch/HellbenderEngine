@echo off
setlocal

:: Set the submodule path (Change this to your actual submodule path)
set SUBMODULE_PATH=dependencies/utils-collection

:: Check if a commit message is provided
if "%~1"=="" (
    echo Usage: commit_submodule.bat "commit message"
    exit /b 1
)

set COMMIT_MESSAGE=%~1

:: Check if the submodule exists
if not exist "%SUBMODULE_PATH%\.git" (
    echo Error: %SUBMODULE_PATH% is not a valid Git submodule.
    exit /b 1
)

:: Change to the submodule directory
cd /d "%SUBMODULE_PATH%" || exit /b 1

:: Ensure we are on the master branch
git checkout master 2>nul
if %errorlevel% neq 0 (
    echo Switching to master branch failed. Trying to create it from remote...
    git checkout -b master origin/master
)

:: Check for unstaged or uncommitted changes
git diff --quiet || set NEED_STASH=1
git diff --cached --quiet || set NEED_STASH=1

if defined NEED_STASH (
    echo Stashing local changes before pulling...
    git stash push -m "Temp stash before pull"
)

:: Pull latest changes from remote with rebase
git pull --rebase origin master
if %errorlevel% neq 0 (
    echo Failed to pull latest changes. Resolve conflicts if needed.
    exit /b 1
)

:: Restore stashed changes if they were saved
if defined NEED_STASH (
    echo Applying stashed changes...
    git stash pop
)

:: Add and commit changes
git add .
git commit -m "%COMMIT_MESSAGE%"

:: Push changes
git push origin master
if %errorlevel% neq 0 (
    echo Failed to push submodule changes.
    exit /b 1
)

:: Move back to the main repository
cd /d ..

:: Stage and commit the updated submodule reference
git add "%SUBMODULE_PATH%"
git commit -m "Updated HBE: %COMMIT_MESSAGE%"
git push origin master

echo Submodule updated successfully!
exit /b 0