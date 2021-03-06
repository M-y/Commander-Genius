/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "engine/core/menu/GameMenu.h"

#include "widgets/Switch.h"
#include "widgets/Button.h"
#include "widgets/ComboSelection.h"
#include "widgets/NumberControl.h"

#include "fileio/CExeFile.h"
#include <string>
#include <memory>
#include <SDL.h>



class CAudioSettings : public GameMenu
{
public:
    CAudioSettings(const enum Style &style);

    void init();

    void ponder(const float deltaT) override;

    void release() override;

    void refresh() override;

private:


    std::shared_ptr <ComboSelection>	mpRate;
    std::shared_ptr <Switch>            mpStereo;
    std::shared_ptr <ComboSelection>	mpDepth;
    std::shared_ptr <ComboSelection>	mpSBToggle;
    std::shared_ptr <NumberControl>     mpSoundVolume;
    std::shared_ptr <NumberControl>     mpMusicVolume;

	SDL_AudioSpec	mAudioSpec;
	bool mSoundblaster;

	// This one does not really set the volume at all, it only verifies, if it changed...
    int mSoundVolume = 0;
    int mMusicVolume = 0;
};


#endif /* CAUDIOSETTINGS_H_ */
