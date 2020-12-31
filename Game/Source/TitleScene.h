#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "Module.h"

#include "GuiButton.h"

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

	// Declare on mouse click event
	bool OnGuiMouseClickEvent(GuiControl* control);

private:
	SDL_Texture* titleScreen;
	SString folderTexture;
	SString folderAudioMusic;

	GuiButton* btnPlay;
	GuiButton* btnContinue;

	GuiButton* btnSettings;

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
	float dtTmp;
};

#endif // __TITLE_SCENE_H__