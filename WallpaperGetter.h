#pragma once
#include <windows.h>
#include <iostream>

/// <summary>
/// Black magic library to create wallpaper process
/// </summary>
class WallpaperGetter
{
private:
	HWND WorkerW;

public:
	
	/// <summary>
	/// Gets handle of wallpaper process, do return the handler on app exit (<see cref="WallpaperGetter::ReturnHandle()"/>)
	/// </summary>
	/// <returns>handle for new wallpaper process</returns>
	HWND GetHandle();
	
	/// <summary>
	/// Frees the process, and returns default wallpaper back
	/// </summary>
	void ReturnHandle();

	/// <summary>
	/// Checks if window was created okay
	/// </summary>
	/// <returns>true if window is created</returns>
	bool IsValid() const { return WorkerW != NULL; }
};

