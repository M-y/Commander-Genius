/*
 * CControlsettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include <base/utils/StringUtils.h>


#include "SelectionMenu.h"
#include "ControlSettings.h"
#include "engine/core/CBehaviorEngine.h"

#include <base/CInput.h>



/**
 * \brief This sets the default settings for a classic gameplay
 */
class ReadInputEvent : public InvokeFunctorEvent
{
public:

	ReadInputEvent( const int selPlayer,
			const InputCommand command,
			const std::string &commandName ) :
		mSelPlayer(selPlayer),
		mCommand(command),
        mCommandName(commandName)
		{}

    void setButtonPtr(std::shared_ptr<GsButton> button)
	{
		mpButton = button;
	}

    void operator()() const
	{
        gInput.setupNewEvent(Uint8(mSelPlayer-1),
                             mCommand);

		const std::string buf = mCommandName;		
		mpButton->setText(buf + "=Reading=");
	}

	int mSelPlayer;
	InputCommand mCommand;
	const std::string mCommandName;
    std::shared_ptr<GsButton> mpButton;
};


/**
 * \brief This sets the default settings for a classic gameplay
 */
class ResetInputEvent : public InvokeFunctorEvent
{
public:

	ResetInputEvent( const int selPlayer ) :
		mSelPlayer(selPlayer)
		{}

    void operator()() const
	{
        assert(mSelPlayer>0);
        gInput.resetControls(mSelPlayer-1);
	}

	int mSelPlayer;
};



CControlsettings::CControlsettings(const int selectedPlayer ,
                                   const Style style) :
GameMenu( GsRect<float>(0.1f, 0.25f, 0.8f, 0.5f), style ),
mSelectedPlayer(selectedPlayer)
{
    mpMenuDialog->add(
                new GameButton( "Movement",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsMovement(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style )  );


    mpMenuDialog->add(
                new GameButton( "Buttons",
                                [this]()
                                {
                                  gEventManager.add( new OpenMenuEvent(
                                    new CControlSettingsButtons(mSelectedPlayer,
                                                             this->getStyle()) ) );
                                },
                                style ) );

    mpTwoButtonSwitch =
            mpMenuDialog->add( new Switch( "Two Button Fire", style ) );
	mpTwoButtonSwitch->enable(gInput.getTwoButtonFiring(mSelectedPlayer-1));

    mpAnalogSwitch =
            mpMenuDialog->add( new Switch( "Analog Movement", style ) );
	mpAnalogSwitch->enable(gInput.isAnalog(mSelectedPlayer-1));

    mpSuperPogoSwitch =
            mpMenuDialog->add( new Switch( "Super Pogo", style ) );
	mpSuperPogoSwitch->enable(gInput.SuperPogo(mSelectedPlayer-1));

    mpImpPogoSwitch =
            mpMenuDialog->add( new Switch( "Impossible Pogo", style ) );
	mpImpPogoSwitch->enable(gInput.ImpossiblePogo(mSelectedPlayer-1));

    mpAutoGunSwitch =
            mpMenuDialog->add( new Switch( "Auto Gun", style ) );
	mpAutoGunSwitch->enable(gInput.AutoGun(mSelectedPlayer-1));

    mpMenuDialog->add( new GameButton( "Reset Controls",
                                             new ResetInputEvent(mSelectedPlayer-1),
                                             style ) );
    
    setMenuLabel("KEYBMENULABEL");

    mpMenuDialog->fit();
    select(1);
}

void CControlsettings::refresh()
{}


void CControlsettings::release()
{
	gInput.setTwoButtonFiring(mSelectedPlayer-1, mpTwoButtonSwitch->isEnabled() );
	gInput.enableAnalog(mSelectedPlayer-1, mpAnalogSwitch->isEnabled() );
	gInput.setSuperPogo(mSelectedPlayer-1, mpSuperPogoSwitch->isEnabled() );
	gInput.setImpossiblePogo(mSelectedPlayer-1, mpImpPogoSwitch->isEnabled() );
	gInput.setAutoGun(mSelectedPlayer-1, mpAutoGunSwitch->isEnabled() );
	gInput.saveControlconfig();
}



// Movements Parts of the Control Settings
CControlSettingsBase::
CControlSettingsBase(const int selectedPlayer,
                     const Style style) :
GameMenu(
        GsRect<float>(0.01f,
                      (1.0f-((MAX_COMMANDS/2.0f)+2)*0.06f)*0.5f,
                      0.98f,
                      (MAX_COMMANDS/2.0f+2)*0.06f), style ),
mSelectedPlayer(selectedPlayer)
{

}


void CControlSettingsBase::ponder(const float deltaT)
{
    if( !mapping && gInput.MappingInput() ) // mapping changed!
    {
        mapping = true;
    }
    else if( !gInput.MappingInput() )
    {
        if(mapping) // mapping changed!
        {
            mapping = false;

            int pos; unsigned char input;
            std::string evName = gInput.getNewMappedEvent(pos, input);
            InputCommand com = static_cast<InputCommand>(pos);

            if(pos >= MID_COMMANDS_OFFSETS)
                pos -= MID_COMMANDS_OFFSETS;

            mpButtonList[static_cast<unsigned int>(pos)]->setText(mCommandName[com] + evName);
        }
    }

    GameMenu::ponder(deltaT);
}

void CControlSettingsBase::release()
{
    if(!mCommandName.empty())
        mCommandName.clear();

    gInput.saveControlconfig();
}



void CControlSettingsMovement::refresh()
{
    mapping = false;
	mCommandName[IC_LEFT]		= "Left:   ";
	mCommandName[IC_RIGHT]		= "Right:  ";
	mCommandName[IC_UP]		= "Up:     ";
	mCommandName[IC_DOWN]		= "Down:   ";
	mCommandName[IC_UPPERLEFT]	= "U-left: ";
	mCommandName[IC_UPPERRIGHT] 	= "U-right:";
	mCommandName[IC_LOWERLEFT] 	= "D-left: ";
	mCommandName[IC_LOWERRIGHT]	= "D-right:";

	if(!mpButtonList.empty())
		mpButtonList.clear();

	std::map<InputCommand, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
		const std::string buf = it->second;
        const std::string buf2 = gInput.getEventShortName( it->first,
                                                           mSelectedPlayer-1 );

        ReadInputEvent *rie =
                new ReadInputEvent(mSelectedPlayer,
                                   it->first,
                                   it->second);

        auto guiButton =
            mpMenuDialog->add(
                    new GameButton( buf+buf2,
                                    rie,
                                    getStyle() ) );

		mpButtonList.push_back( guiButton );

        rie->setButtonPtr(
                    std::static_pointer_cast<GsButton>(guiButton)
                         );
	}

	setMenuLabel("MOVEMENULABEL");

    mpMenuDialog->fit();
}



// Movements Parts of the Control Settings
void CControlSettingsButtons::refresh()
{
    mapping = false;
	mCommandName[IC_JUMP] 		= "Jump:   ";
	mCommandName[IC_POGO] 		= "Pogo:   ";
    mCommandName[IC_FIRE]		= "Fire:   ";
    mCommandName[IC_RUN]		= "Run:    ";
    mCommandName[IC_STATUS] 	= "Status: ";
	mCommandName[IC_CAMLEAD] 	= "Camlead:";
	mCommandName[IC_HELP] 		= "Help:   ";
	mCommandName[IC_BACK] 		= "Back:   ";

	if(!mpButtonList.empty())
		mpButtonList.clear();

	std::map<InputCommand, std::string>::iterator it = mCommandName.begin();
	for ( ; it != mCommandName.end(); it++ )
	{
        const std::string buf = it->second;
        const std::string buf2 = gInput.getEventShortName( it->first,
                                                           mSelectedPlayer-1 );

        ReadInputEvent *rie =
                new ReadInputEvent(mSelectedPlayer,
                                   it->first, it->second);

        auto guiButton =
            mpMenuDialog->add(
                    new GameButton( buf+buf2,
                                    rie,
                                    getStyle() ) );

        mpButtonList.push_back( guiButton );

        rie->setButtonPtr(
                    std::static_pointer_cast<GsButton>(guiButton)
                         );
	}

	setMenuLabel("BUTTONMENULABEL");
    mpMenuDialog->fit();
}

