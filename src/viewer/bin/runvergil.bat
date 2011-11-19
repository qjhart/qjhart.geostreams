@echo off
setlocal

@rem set RDEBUG=-Xdebug  -Xnoagent -Xrunjdwp:transport=dt_socket,address=2358,server=y,suspend=y 

call "%~dp0setenv.bat"
set CP=%PROJECT_HOME%\_generated\BUILD\classes

set JAVA_HOME=\Program Files\Java\jdk1.5.0_06
PATH=%JAVA_HOME%\bin;%PATH%
set PTII=C:\software\Ptolemy\ptII5.0.1

set JAIHOME=C:\jai-1_1_2_01
set CP=%CP%;%JAIHOME%\lib\jai_core.jar
set CP=%CP%;%JAIHOME%\lib\jai_codec.jar

set JMFHOME=C:\JMF2.1.1e
set CP=%CP%;%JMFHOME%\lib\jmf.jar

set CP=%CP%;%PTII%

@rem set CP=%CP%;%PTII%\lib\chic.jar
@rem set CP=%CP%;%PTII%\lib\diva.jar
@rem set CP=%CP%;%PTII%\lib\jasminclasses.jar
@rem set CP=%CP%;%PTII%\lib\joystickWindows.jar
@rem set CP=%CP%;%PTII%\lib\jython.jar
@rem set CP=%CP%;%PTII%\lib\mapss.jar
@rem set CP=%CP%;%PTII%\lib\matlab.jar
@rem set CP=%CP%;%PTII%\lib\matlabSunOS.jar
@rem set CP=%CP%;%PTII%\lib\matlabWindows.jar
set CP=%CP%;%PTII%\lib\ptCal.jar
set CP=%CP%;%PTII%\lib\ptcolt.jar
@rem set CP=%CP%;%PTII%\lib\ptjacl.jar
@rem set CP=%CP%;%PTII%\lib\saxon7.jar
@rem set CP=%CP%;%PTII%\lib\sootclasses.jar
@rem set CP=%CP%;%PTII%\lib\synthesis.jar

set CP=%CP%;%PTII%\ptolemy\ptsupport.jar

echo CP=%CP%
set CLASSPATH=%CP%

@rem -classpath "%CP%" 
java %RDEBUG% -ea -Xms60M -Xmx250M ptolemy.vergil.VergilApplication %*

endlocal
