@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM This script runs multiple simulations using the specified executable and parameter files.
REM The first argument is the executable path (default: archgen.exe).
REM The second argument is the directory containing parameter files (default: current directory).
REM Parameter files must be named: sim_<number>_parameters.txt.
REM If present, sim_<number>_architecture.txt is copied to architecture.txt before that simulation.
REM Output files with prefixes traits, genotypes, alleles, architecture, and paramlog are renamed
REM to sim_<number>_<original_name>, overwriting existing targets.
REM architecture.txt is only renamed when it was restored from sim_<number>_architecture.txt.

set "EXECUTABLE=%~1"
if "%EXECUTABLE%"=="" set "EXECUTABLE=archgen.exe"

set "PARAM_DIR=%~2"
if "%PARAM_DIR%"=="" set "PARAM_DIR=."

if not exist "%EXECUTABLE%" (
	echo Error: executable not found: %EXECUTABLE% 1>&2
	echo Usage: %~nx0 [executable_path] [parameters_directory] 1>&2
	exit /b 1
)

set "PARAM_FILES_COUNT=0"
for /f "delims=" %%F in ('dir /b /a-d /on "%PARAM_DIR%\sim_*_parameters.txt" 2^>nul') do (
	set /a PARAM_FILES_COUNT+=1
)

if %PARAM_FILES_COUNT% EQU 0 (
	echo Error: no files matching sim_*_parameters.txt found in: %PARAM_DIR% 1>&2
	exit /b 1
)

echo Running %PARAM_FILES_COUNT% simulation^(s^) with: %EXECUTABLE%

for /f "delims=" %%F in ('dir /b /a-d /on "%PARAM_DIR%\sim_*_parameters.txt" 2^>nul') do (
	echo ----------------------------------------
	set "PARAM_FILE=%PARAM_DIR%\%%F"
	echo Running with: !PARAM_FILE!
	set "PARAM_BASENAME=%%~nxF"

	set "TOK1="
	set "TOK2="
	set "TOK3="
	set "TOK4="
	for /f "tokens=1,2,3,4 delims=_." %%a in ("!PARAM_BASENAME!") do (
		set "TOK1=%%a"
		set "TOK2=%%b"
		set "TOK3=%%c"
		set "TOK4=%%d"
	)

	if /I not "!TOK1!"=="sim" (
		echo Error: parameter file name must match sim_^<number^>_parameters.txt: !PARAM_BASENAME! 1>&2
		exit /b 1
	)
	if /I not "!TOK3!"=="parameters" (
		echo Error: parameter file name must match sim_^<number^>_parameters.txt: !PARAM_BASENAME! 1>&2
		exit /b 1
	)
	if /I not "!TOK4!"=="txt" (
		echo Error: parameter file name must match sim_^<number^>_parameters.txt: !PARAM_BASENAME! 1>&2
		exit /b 1
	)
	echo(!TOK2!| findstr /r "^[0-9][0-9]*$" >nul
	if errorlevel 1 (
		echo Error: parameter file name must match sim_^<number^>_parameters.txt: !PARAM_BASENAME! 1>&2
		exit /b 1
	)
	set "SIM_INDEX=!TOK2!"

	set "ARCHITECTURE_RESTORED=0"
	set "EXISTING_ARCH_FILE=sim_!SIM_INDEX!_architecture.txt"
	if exist "!EXISTING_ARCH_FILE!" (
		copy /Y "!EXISTING_ARCH_FILE!" "architecture.txt" >nul
		if errorlevel 1 (
			echo Error: failed to prepare architecture.txt from !EXISTING_ARCH_FILE! 1>&2
			exit /b 1
		)
		set "ARCHITECTURE_RESTORED=1"
		echo Prepared architecture.txt from !EXISTING_ARCH_FILE!
	)

	"%EXECUTABLE%" "!PARAM_FILE!"
	if errorlevel 1 (
		echo Error: simulation failed for !PARAM_FILE! 1>&2
		exit /b 1
	)

	for %%P in (traits genotypes alleles architecture paramlog) do (
		for %%O in (%%P*) do (
			if exist "%%~fO" (
				set "OUTPUT_BASENAME=%%~nxO"
				if /I "!OUTPUT_BASENAME!"=="architecture.txt" if "!ARCHITECTURE_RESTORED!"=="0" (
					echo Leaving architecture.txt untouched ^(no sim-specific architecture restored for sim_!SIM_INDEX!^).
				) else (
					set "RENAMED_FILE=sim_!SIM_INDEX!_!OUTPUT_BASENAME!"
					move /Y "%%~fO" "!RENAMED_FILE!" >nul
					if errorlevel 1 (
						echo Error: failed to rename !OUTPUT_BASENAME! to !RENAMED_FILE! 1>&2
						exit /b 1
					)
					echo Renamed: !OUTPUT_BASENAME! -^> !RENAMED_FILE!
				)
			)
		)
	)
)

echo ----------------------------------------
echo All simulations completed successfully.
exit /b 0
