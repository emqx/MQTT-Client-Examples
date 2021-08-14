from building import *

# get current directory
cwd     = GetCurrentDir()
# The set of source files associated with this SConscript file.
src     = Glob('src/*.c')

path    = [cwd + '/inc']

group = DefineGroup('EasyFlash', src, depend = ['PKG_USING_EASYFLASH'], CPPPATH = path)

Return('group')
