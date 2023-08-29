mkdir bin

copy lib\x64\zlib.dll bin
copy lib\x64\deflate.dll bin
copy lib\x64\glfw3.dll bin
copy lib\x64\glew.dll bin
copy lib\x64\openal.dll bin
copy lib\x64\ogg.dll bin
copy lib\x64\vorbis.dll bin
copy lib\x64\vorbisfile.dll bin
copy lib\x64\openmpt.dll bin
xcopy  Jazz2Data\Content bin\Content /e /i /h /y
xcopy  Jazz2Data\Source bin\Source  /e /i /h /y


copy x64\Release\Jazz2.exe bin