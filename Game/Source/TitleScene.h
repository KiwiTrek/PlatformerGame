#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "Module.h"
#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSlider.h"

#define TITLE_FONT_SIZE 36
#define TITLE_FONT_SMALL_SIZE TITLE_FONT_SIZE/2

struct SDL_Texture;

class TitleScene : public Module
{
public:
	// Constructor
	TitleScene();

	// Destructor
	virtual ~TitleScene();

	// Called when program is executed
	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// On mouse click response
	bool OnGuiMouseClickEvent(GuiControl* control);

	bool loadRequest;

private:
	SString folderTexture;
	SString folderAudioMusic;

	SDL_Texture* titleScreen;

	// Gui
	GuiButton* btnPlay;
	GuiButton* btnContinue;
	float dtTmp;

	GuiButton* btnSettings;
	SString settingsTitle;
	bool settings;
	GuiSlider* sldrMusic;
	GuiSlider* sldrFx;
	GuiCheckBox* chckFullscreen;
	GuiCheckBox* chckVsync;

	GuiButton* btnCredits;
	bool credits;
	int titleFont;
	int titleFontSmall;
	int offsetText;
	SString title;
	SString underLine;
	SString authors;
	SString kiwi;
	SString lladruc;
	SString licence;
	SString licenceText1;
	SString licenceText2;
	SString licenceText3;
	SString licenceText4;
	SString licenceText5;
	SString licenceText6;
	GuiButton* btnBack;

	GuiButton* btnExit;
	bool exitRequest;
};

#endif // __TITLE_SCENE_H__