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
#include "ItemsArrivingState.h"
#include <sstream>
#include <algorithm>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/Craft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Savegame/Vehicle.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "GeoscapeState.h"
#include "../Engine/Options.h"
#include "../Basescape/BasescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Items Arriving window.
 * @param game Pointer to the core game.
 * @param state Pointer to the Geoscape state.
 */
ItemsArrivingState::ItemsArrivingState(GeoscapeState *state) : _state(state), _base(0)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 184, 0, 8, POPUP_BOTH);
	_btnOk = new TextButton(142, 16, 16, tr(166,"ItemArrivingBtnY"));
	_btnGotoBase = new TextButton(142, 16, 162, tr(166,"ItemArrivingBtnY"));
	_txtTitle = new Text(310, 17, 5, 18);
	_txtItem = new Text(114, tr(9,"TextSmallH"), 16, 34);
	_txtQuantity = new Text(54, tr(9,"TextSmallH"), 152, 34);
	_txtDestination = new Text(112, tr(9,"TextSmallH"), 212, 34);
	_lstTransfers = new TextList(271, tr(112,"ItemArrivingListH"), 14, tr(50,"ItemArrivingListY"));

	// Set palette
	setPalette("PAL_GEOSCAPE", 6);

	add(_window);
	add(_btnOk);
	add(_btnGotoBase);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtDestination);
	add(_lstTransfers);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ItemsArrivingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ItemsArrivingState::btnOkClick, Options::keyCancel);

	_btnGotoBase->setColor(Palette::blockOffset(8)+5);
	_btnGotoBase->setText(tr("STR_GO_TO_BASE"));
	_btnGotoBase->onMouseClick((ActionHandler)&ItemsArrivingState::btnGotoBaseClick);
	_btnGotoBase->onKeyboardPress((ActionHandler)&ItemsArrivingState::btnGotoBaseClick, Options::keyOk);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_ITEMS_ARRIVING"));

	_txtItem->setColor(Palette::blockOffset(8)+5);
	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(8)+5);
	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtDestination->setColor(Palette::blockOffset(8)+5);
	_txtDestination->setText(tr("STR_DESTINATION_UC"));

	_lstTransfers->setColor(Palette::blockOffset(8)+10);
	_lstTransfers->setArrowColor(Palette::blockOffset(8)+5);
	_lstTransfers->setColumns(3, 155, 41, 98);
	_lstTransfers->setSelectable(true);
	_lstTransfers->setBackground(_window);
	_lstTransfers->setMargin(2);

	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Transfer*>::iterator j = (*i)->getTransfers()->begin(); j != (*i)->getTransfers()->end();)
		{
			if ((*j)->getHours() == 0)
			{
				_base = (*i);

				// Check if we have an automated use for an item
				if ((*j)->getType() == TRANSFER_ITEM)
				{
					RuleItem *item = _game->getRuleset()->getItem((*j)->getItems());
					for (std::vector<Craft*>::iterator c = (*i)->getCrafts()->begin(); c != (*i)->getCrafts()->end(); ++c)
					{
						// Check if it's ammo to reload a craft
						if ((*c)->getStatus() == "STR_READY")
						{
							for (std::vector<CraftWeapon*>::iterator w = (*c)->getWeapons()->begin(); w != (*c)->getWeapons()->end(); ++w)
							{
								if ((*w) != 0 && (*w)->getRules()->getClipItem() == item->getType() && (*w)->getAmmo() < (*w)->getRules()->getAmmoMax())
								{
									(*w)->setRearming(true);
									(*c)->setStatus("STR_REARMING");
								}
							}
						}
						// Check if it's ammo to reload a vehicle
						for (std::vector<Vehicle*>::iterator v = (*c)->getVehicles()->begin(); v != (*c)->getVehicles()->end(); ++v)
						{
							std::vector<std::string>::iterator ammo = std::find((*v)->getRules()->getCompatibleAmmo()->begin(), (*v)->getRules()->getCompatibleAmmo()->end(), item->getType());
							if (ammo != (*v)->getRules()->getCompatibleAmmo()->end() && (*v)->getAmmo() < item->getClipSize())
							{
								int used = std::min((*j)->getQuantity(), item->getClipSize() - (*v)->getAmmo());
								(*v)->setAmmo((*v)->getAmmo() + used);
								// Note that the items have already been delivered, so we remove them from the base, not the transfer
								_base->getItems()->removeItem(item->getType(), used);
							}
						}
					}
				}

				// Remove transfer
				std::wostringstream ss;
				ss << (*j)->getQuantity();
				_lstTransfers->addRow(3, (*j)->getName(_game->getLanguage()).c_str(), ss.str().c_str(), (*i)->getName().c_str());
				delete *j;
				j = (*i)->getTransfers()->erase(j);
			}
			else
			{
				++j;
			}
		}
	}
}

/**
 *
 */
ItemsArrivingState::~ItemsArrivingState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ItemsArrivingState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the base for the respective transfer.
 * @param action Pointer to an action.
 */
void ItemsArrivingState::btnGotoBaseClick(Action *)
{
	_state->timerReset();
	_game->popState();
	_game->pushState(new BasescapeState(_base, _state->getGlobe()));
}

}
