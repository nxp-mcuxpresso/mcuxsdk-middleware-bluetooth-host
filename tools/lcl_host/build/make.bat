@rem Top level script to build wireless_ranging_host libraries
@rem @echo off

@rem Build algorithms python library for windows (_ranging.pyd)
@rem @pushd ..\software\algo\ranging\apps\libpython\win64
@rem @call build.bat
@rem @popd
@rem copy ..\software\algo\ranging\apps\libpython\win64\bin\Release\_ranging.pyd ..\python\Instruments

@rem Build python protected files (.pyc)
@rem @call build_python_pyc.bat

