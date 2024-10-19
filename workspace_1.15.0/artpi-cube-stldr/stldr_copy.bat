@echo off
::设置要复制的文件目录路径(空即为当前目录)
set SrcDir=
::设置保存复制后的文件路径(必填项)
set SavDir=D:\ST\STM32CubeIDE_1.11.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.0.200.202202231230\tools\bin\ExternalLoader
::设置要复制的文件格式，支持多格式，每个格式直接用英文逗号隔开
set FileExt=*.stldr
::if defined SrcDir cd /d "%SrcDir%"
if exist SrcDir (cd /d "%SrcDir%")
for %%a in (%FileExt%) do copy "%%~a" "%SavDir%\"
::pause
exit