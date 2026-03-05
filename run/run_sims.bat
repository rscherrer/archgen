@echo on
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

set "EXECUTABLE=archgen.exe"
set "PARAM_DIR=."

echo Current directory: %CD%
echo.

for %%F in (%PARAM_DIR%\sim_*_parameters.txt) do (

    if exist "%%F" (

        echo ----------------------------------------
        echo Running simulation with:
        echo %%F
        echo.

        REM Extract simulation number from filename
        set "FILENAME=%%~nxF"

        for /f "tokens=2 delims=_." %%A in ("!FILENAME!") do (
            set "SIMNUM=%%A"
        )

        echo Simulation number: !SIMNUM!
        echo.

        "%EXECUTABLE%" "%%F"

        echo Returned errorlevel: !errorlevel!
        echo.

        if errorlevel 1 (
            echo Simulation failed.
            pause
            exit /b 1
        )

        REM Rename output files
        for %%O in (traits genotypes alleles architecture paramlog) do (
            for %%X in (%%O*) do (
                if exist "%%X" (
                    set "NEWNAME=sim_!SIMNUM!_%%X"
                    move /Y "%%X" "!NEWNAME!"
                    echo Renamed %%X to !NEWNAME!
                )
            )
        )

        echo.
    )
)

echo ----------------------------------------
echo All simulations completed.
echo.
pause
exit /b 0