/*
 * GsAppState.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include <base/GsAppState.h>
#include <base/GsLogging.h>
#include <base/CInput.h>

#include "common/Menu/CMenuController.h"

#include "core/CGameLauncherMenu.h"
#include "core/mode/CGamePassiveMode.h"

#include "core/CGameMain.h"

std::string getArgument( int argc, char *argv[], const std::string& text )
{
    std::string argument;
    for( int i=1 ; i<argc ; i++ )
    {
        argument = argv[i];
        if( argument.find(text) == 0 ) // argument was found!
            return argument;
    }
    return "";
}

bool getBooleanArgument( int argc, char *argv[], const std::string& text )
{
    std::string argument;
    for( int i=1 ; i<argc ; i++ )
    {
        argument = argv[i];
        if( argument.find(text) == 0 ) // argument was found!
            return true;
    }
    return false;
}



void GsAppStateEventSink::pumpEvent(const CEvent *ev)
{
    mpAppState->pumpEvent(ev);
}


GsAppState::GsAppState(bool &firsttime) :
m_firsttime(firsttime),
m_startGame_no(-1),
m_startLevel(-1),
mSink(this)
{
    g_pBehaviorEngine->EventList().regSink(&mSink);
}

GsAppState::~GsAppState()
{
    g_pBehaviorEngine->EventList().unregSink(&mSink);
}

////
// Initialization Routine
////
bool GsAppState::init(int argc, char *argv[])
{
	bool ok = true;
    CEventContainer& EventContainer = gEventManager;
	std::string argument;
	argument = getArgument( argc, argv, "-game" );

	// Check if some arguments were given.
	if(argument != "")
	{
		// Get the game number according to the created menu list.
		std::string buf = argument.substr(strlen("-game"));
		int chosengame = atoi(buf)-1;

		if(chosengame >= 0)
		{
			// Tell CG to pass the chosen number of game
			m_startGame_no = chosengame;

			// Now check, if a level was also passed as parameter
			argument = getArgument( argc, argv, "-level" );
			if(argument != "")
			{
				buf = argument.substr(strlen("-level"));
				m_startLevel = atoi(buf);
			}

			// Now check, if a difficulty was chosen, otherwise choose easy
			argument = getArgument( argc, argv, "-diff" );
			if(argument != "")
			{
				buf = argument.substr(strlen("-diff"));
				Difficulty startDifficulty = static_cast<Difficulty>(atoi(buf));
				
				// catch invalid entries
				if(startDifficulty < 0 || startDifficulty > 3)
				{
				  // TODO: Tell here that difficulty is invalid and that CG continues on easy
				  startDifficulty = UNKNOWN;
				}
				
				g_pBehaviorEngine->mDifficulty = startDifficulty;
			}
		}
	}

    // TODO: Check if finale cutscenes must be shown
    /*if(getBooleanArgument( argc, argv, "-finale" ))
	{
		argument = getArgument( argc, argv, "-finale" );
		m_startGame_no = atoi(argument.c_str()+strlen("-finale"))-1;
		m_startLevel = WM_MAP_NUM;
    }*/

	EventContainer.add( new GMSwitchToGameLauncher(m_startGame_no, m_startLevel) );

	return ok;
}



void GsAppState::pumpEvent(const CEvent *evPtr)
{
    // Pump all the events related to the root of AppState    
    if( const GMSwitchToGameLauncher* p_Launcher = dynamic_cast<const GMSwitchToGameLauncher*>(evPtr) )
    {
        g_pSound->unloadSoundData();
        gpMenuController->emptyMenuStack();
        mpEngine.reset(new CGameLauncherMenu( m_firsttime,
                          p_Launcher->m_ChosenGame,
                          p_Launcher->m_StartLevel) );
        mpEngine->start();
    }
    else if( dynamic_cast<const StartMainGameEvent*>(evPtr) )
    {
        mpEngine.reset(new CGameMain(gpMenuController->mOpenedGamePlay));
        mpEngine->start();
    }
    else if( dynamic_cast<const GMQuit*>(evPtr) )
    {
        mpEngine.release();
    }
    else if( const InvokeFunctorEvent *iEv = dynamic_cast<const InvokeFunctorEvent*>(evPtr) )
    {
        (*iEv)();
    }
    else // none of the above, let's see if the children have events to be processed
    {
        mpEngine->pumpEvent(evPtr);
        gpMenuController->pumpEvent(evPtr);
    }
}


void GsAppState::pollEvents()
{
    if( gInput.getExitEvent() )
    {
      mpEngine.release();
      return;
    }
}


////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void GsAppState::ponder(const float deltaT)
{    
    pollEvents();

	// Process the game control object if no effects are being processed
	if(mpEngine)
        mpEngine->ponder(deltaT);

	if(g_pGfxEngine->runningEffect())
	{
		if( gInput.getPressedAnyCommand() || gInput.mouseClicked() )
		{
            g_pGfxEngine->setFXfinished(true);
		}
	}
	
    gpMenuController->ponder(deltaT);
}

void GsAppState::render()
{
    if(mpEngine)
        mpEngine->render();

    gpMenuController->render();
}
