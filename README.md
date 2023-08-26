![Demo](demo_Trim.gif)

# Live Wallpapers with Windows 8+ support
Raw code of live wallpapers for overview.  
Finds WorkerW window and creates new one as child.  
Probably, it'll help someone in representation of Live Wallpapers for Desktop.  

## Requirements
Included headers:
*  Windows.h
*  Dshow.h

DirectShow requires two additional requirements:  
*  Quartz.lib
*  Strmiids.lib

## Instructions
Edit WIDTH, HEIGHT and WP_PATH macros in "main.h".  
Compile and run.
