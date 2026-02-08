@echo off
setlocal EnableExtensions EnableDelayedExpansion

echo === Updating all submodules safely ===

git submodule foreach --recursive ^
    "echo --- Updating $name --- && ^
    git fetch origin && ^
    git symbolic-ref --quiet HEAD || (echo ERROR: $name is in detached HEAD! && exit 1) && ^
    git branch --show-current >nul && if errorlevel 1 (echo ERROR: Cannot detect branch! && exit 1) && ^
    set NEED_STASH=0 && ^
    git diff --quiet || set NEED_STASH=1 && ^
    git diff --cached --quiet || set NEED_STASH=1 && ^
    if !NEED_STASH! EQU 1 (echo Stashing changes... && git stash push -u -m 'Auto-stash before update') && ^
    git checkout master || exit 1 && ^
    git pull --ff-only origin master || exit 1 && ^
    if !NEED_STASH! EQU 1 (echo Restoring stashed changes... && git stash pop || echo WARNING: conflicts must be resolved manually)"
    
echo === All submodules updated safely ===
exit /b 0