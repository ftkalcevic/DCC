Path=D:\Projects\winavr\bin;D:\Projects\winavr\utils\bin;%%Path%%
make.exe %1 >c:\tempfile 2>&1 c:\tempfile
set >> c:\tempfile
