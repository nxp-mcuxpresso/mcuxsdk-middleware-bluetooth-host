@rem This file compiles pythno folders that should not be delivered in clear source
@rem We use python compiler to produce bytecode files (.pyc)

@set root=..
@set outpath=%root%

@call :compile_python_pkg %outpath%\python\Instruments\quality_indicator_pkg
@call :compile_python_pkg %outpath%\python\Instruments\range_estimator_pkg
@call :compile_python_pkg %outpath%\python\Instruments\tracking_pkg

@pushd %outpath%
@popd
@goto:eof

:compile_python_pkg
    @echo Compiling %~1
    py -3.6 -m compileall -b %~1
    IF %ERRORLEVEL% NEQ 0 (
        @echo Build error in: %~1
    ) else (
        @echo Build success in: %~1
    )
    @goto:eof

