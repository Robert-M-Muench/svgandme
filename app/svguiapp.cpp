#pragma comment(lib, "blend2d.lib")



#include <filesystem>

#include "svguiapp.h"




using namespace waavs;


waavs::FontHandler& getFontHandler() {
	static waavs::FontHandler fh;
	return fh;
}


APP_EXTERN waavs::Recorder gRecorder{ nullptr };

static VOIDROUTINE gSetupHandler = nullptr;



// Typography
static bool loadFont(const char* filename, BLFontFace& ff) noexcept
{
	bool success = getFontHandler().loadFontFace(filename, ff);
	return success;
}

static bool loadFontDirectory(const char* dir) noexcept
{
	const std::filesystem::path fontPath(dir);

	// If the path does not exist
	// return immediately
	if (!std::filesystem::exists(fontPath))
	{
		return false;
	}

	if (fontPath.empty())
	{
		return false;
	}

	for (const auto& dir_entry : std::filesystem::directory_iterator(fontPath))
	{
		if (dir_entry.is_regular_file())
		{
			if ((dir_entry.path().extension() == ".ttf") ||
				(dir_entry.path().extension() == ".otf") ||
				(dir_entry.path().extension() == ".TTF"))
			{
				BLFontFace ff{};
				bool success = getFontHandler().loadFontFace(dir_entry.path().generic_string().c_str(), ff);
			}
				
			//if (endsWith(dir_entry.path().generic_string(), ".ttf") ||
			//	endsWith(dir_entry.path().generic_string(), ".TTF") ||
			//	endsWith(dir_entry.path().generic_string(), ".otf"))
			//{
			//	BLFontFace ff{};
			//	bool success = gFontHandler.loadFontFace(dir_entry.path().generic_string().c_str(), ff);
			//}
		}
	}

	return true;
}

static bool loadDefaultFonts() noexcept
{
	// Load in some fonts to start
	std::vector<const char*> fontNames{
		"c:\\Windows\\Fonts\\arial.ttf",
		"c:\\Windows\\Fonts\\calibri.ttf",
		"c:\\Windows\\Fonts\\cascadiacode.ttf",
		"c:\\Windows\\Fonts\\consola.ttf",
		"c:\\Windows\\Fonts\\cour.ttf",
		"c:\\Windows\\Fonts\\gothic.ttf",
		"c:\\Windows\\Fonts\\segoui.ttf",
		"c:\\Windows\\Fonts\\tahoma.ttf",
		"c:\\Windows\\Fonts\\terminal.ttf",
		"c:\\Windows\\Fonts\\times.ttf",
		"c:\\Windows\\Fonts\\verdana.ttf",
		"c:\\Windows\\Fonts\\wingding.ttf"
	};
	return getFontHandler().loadFonts(fontNames);
}

static bool loadFontFiles(std::vector<const char*> filenames)
{
	getFontHandler().loadFonts(filenames);

	return true;
}

static void registerAppHandlers()
{
	// we're going to look within our own module
	// to find handler functions.  This is because the user's application should
	// be compiled with the application, so the exported functions should
	// be attainable using 'GetProcAddress()'

	HMODULE hInst = ::GetModuleHandleA(NULL);


	// Get the general app routines
	// onLoad()
	gSetupHandler = (VOIDROUTINE)::GetProcAddress(hInst, "setup");

}

void onLoad()
{
	//printf("onLoad\n");

	registerAppHandlers();

	// if setup() exists, call that
	if (gSetupHandler != nullptr)
		gSetupHandler();

	// Refresh the screen at least once
	screenRefresh();
}