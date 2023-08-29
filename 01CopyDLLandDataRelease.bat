REM this for lunching out of visual studio

copy lib\x64\zlib.dll Jazz2
copy lib\x64\deflate.dll Jazz2
copy lib\x64\glfw3.dll Jazz2
copy lib\x64\glew.dll Jazz2
copy lib\x64\openal.dll Jazz2
copy lib\x64\ogg.dll Jazz2
copy lib\x64\vorbis.dll Jazz2
copy lib\x64\vorbisfile.dll Jazz2
copy lib\x64\openmpt.dll Jazz2
xcopy  Jazz2Data\Content x64\Release\Content /e /i /h /y
xcopy  Jazz2Data\Source x64\Release\Source  /e /i /h /y
