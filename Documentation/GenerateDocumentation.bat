@echo off

::Variables controlling generated documentation
set GENERATE_ENGINE_DOC=1

::Argument parsing
:parse
	)
::Generate engine doc
if "%1"=="-e" (
		set FINALIZE_LOCKED=1
		call :engine
		shift
		goto :parse
	)
::Generate engine doc
if "%1"=="-a" (
		goto :all
	)
::Empty parameter list - equal to -a
if "%*"=="" (
		goto :all
	)
::Print help
if "%1"=="-h" (
		goto :help
	)
set FINALIZE_LOCKED=0
goto :finalize

:all
if not defined ENGINE_DOC_GENERATED (
	set FINALIZE_LOCKED=1
	call :engine
)
set FINALIZE_LOCKED=0
goto :finalize

:engine
echo Starting engine documentation generation...
if %GENERATE_ENGINE_DOC%==0 (
	echo Engine documentation generation toggled off by GENERATE_ENGINE_DOC variable!
	echo Skipping engine documentation generation step...
) else (
	if not defined ENGINE_DOC_GENERATED (
		doxygen EngineDoxygenConfig >nul
		echo Engine documentation generation finished!
		set ENGINE_DOC_GENERATED=1
	)
)
goto :finalize

:finalize
if %FINALIZE_LOCKED%==0 (
	set ENGINE_DOC_GENERATED=
	echo All documentation successfuly generated!
	pause
	goto :eof
)
goto :eof

:help
echo.
echo Doxygen documentation generation tool
echo =====================================
echo.
echo -h					Display this help dialog
echo -e					Generate engine doc
echo -t					Generate tools doc
echo -a					Generate all docs
echo no parameter				Equal to -a
pause
goto :eof