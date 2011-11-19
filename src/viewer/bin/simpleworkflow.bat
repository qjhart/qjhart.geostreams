@echo off
setlocal

call "%~dp0setenv.bat"
set CLAZZPATH=%PROJECT_HOME%\_generated\BUILD\classes
echo CLAZZPATH=%CLAZZPATH%

@rem set RDEBUG=-Xdebug  -Xnoagent -Xrunjdwp:transport=dt_socket,address=2358,server=y,suspend=y 

set JAVA_HOME=\Program Files\Java\jdk1.5.0_06
PATH=%JAVA_HOME%\bin;%PATH%

java %RDEBUG% -ea -classpath "%CLAZZPATH%" geostreams.workflow.SimpleWorkflow %*

endlocal