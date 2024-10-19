del *.bak /s
del *.ddk /s
del *.edk /s
del *.lst /s
del *.lnp /s
del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s
::del *.opt /s ::删除JLINK配置文件
del *.plg /s
del *.rpt /s
del *.tmp /s
del *.__i /s
del *.crf /s
del *.o /s
del *.d /s
del *.axf /s
del *.tra /s
del *.dep /s           
del JLinkLog.txt /s

del *.iex /s
del *.htm /s
del *.map /s
del *.scvd /s
del *.orig /s
del *.uvguix.* /s
del *.dbgconf /s

del *.html /s
::删除所有空文件夹
for /f "tokens=*" %%a in ('dir /b /ad /s^|sort /r') do rd "%%a" 2>nul
exit
