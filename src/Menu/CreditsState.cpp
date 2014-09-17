/*
 * Copyright 2010-2014 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "CreditsState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Options.h"
#include "../Engine/CrossPlatform.h"
#include <fstream>

namespace OpenXcom
{

/**
 * Initializes all the elements
 */
CreditsState::CreditsState()
{
	// Create objects
	_window = new Window(this, 310, 190, (320-310)/2, (200-190)/2, POPUP_VERTICAL);
	_txtTitle = new Text(250, 16, (320-250)/2, 15);
	_lstCredits = new TextList(250-15, 170-32-4, _txtTitle->getX(), 32);
	_btnOk = new TextButton(80, 16, (320-80)/2, 170);

	// Set palette
	setPalette("PAL_GEOSCAPE", 0);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_lstCredits);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CreditsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CreditsState::btnOkClick, Options::keyOk);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_CREDITS"));
	_txtTitle->setBig();

	_lstCredits->setColor(Palette::blockOffset(15) - 1);
	_lstCredits->setSecondaryColor(Palette::blockOffset(8) + 10);
	_lstCredits->setColumns(1, _lstCredits->getWidth());
	_lstCredits->setBackground(_window);

	std::vector<std::wstring> lines;
	std::string s = CrossPlatform::getDataFile("Language/ko/credits.txt");
	std::ifstream ifs(s);
	do
	{
		std::getline(ifs, s);
		lines.push_back(Language::utf8ToWstr(s));
	} while (!ifs.eof());
	ifs.close();

	size_t rows = lines.size();
	for (std::vector<std::wstring>::const_reverse_iterator i = lines.rbegin(); i != lines.rend(); ++i, --rows)
	{
		if (i->find_last_not_of(L" \t\r") < i->length())
			break;
	}
	for (std::vector<std::wstring>::const_iterator i = lines.begin(); rows > _lstCredits->getRows(); ++i)
		_lstCredits->addRow(1, i->c_str());

	// Set music
	_game->getResourcePack()->playMusic("GMWIN");
}

CreditsState::~CreditsState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CreditsState::btnOkClick(Action *)
{
	_game->popState();
	_game->getResourcePack()->playMusic("GMSTORY");
}

}