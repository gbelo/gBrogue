/*
 *  Items.c
 *  gBrogue
 *  A Brogue variant created by G. Reed on 4/5/17.
 *  Copyright 2017. All rights reserved.
 *
 *  This file is modified from original source:
 *
 *  Brogue
 *
 *  Created by Brian Walker on 1/17/09.
 *  Copyright 2012. All rights reserved.
 *
 *  This file is part of Brogue.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Rogue.h"
#include "IncludeGlobals.h"
#include <math.h>

item *initializeItem() {
	short i;
	item *theItem;

	theItem = (item *) malloc(sizeof(item));
	memset(theItem, '\0', sizeof(item) );

	theItem->category = 0;
	theItem->kind = 0;
	theItem->flags = 0;
	theItem->displayChar = '&';
	theItem->foreColor = &itemColor;
	theItem->inventoryColor = &white;
	theItem->armor = 0;
	theItem->strengthRequired = 0;
	theItem->enchant1 = 0;
	theItem->enchant2 = 0;
    theItem->timesEnchanted = 0;
	theItem->vorpalEnemy = 0;
	theItem->charges = 0;
	theItem->quantity = 1;
	theItem->quiverNumber = 0;
	theItem->originDepth = 0;
	theItem->inscription[0] = '\0';
	theItem->nextItem = NULL;

	for (i=0; i < KEY_ID_MAXIMUM; i++) {
		theItem->keyLoc[i].x = 0;
		theItem->keyLoc[i].y = 0;
		theItem->keyLoc[i].machine = 0;
		theItem->keyLoc[i].disposableHere = false;
	}
    return theItem;
}

// Allocates space, generates a specified item (or random category/kind if -1)
// and returns a pointer to that item. The item is not given a location here
// and is not inserted into the item chain!
item *generateItem(unsigned short theCategory, short theKind) {
	item *theItem = initializeItem();
	makeItemInto(theItem, theCategory, theKind);
	return theItem;
}

unsigned long pickItemCategory(unsigned long theCategory) {
	short i, sum, randIndex;
// Probability of different item categories
//	short probabilities[13] =						{50,	42,		52,		3,		3,		10,		8,		2,		3,      2,        0,		0,		0};
//	short probabilities[13] =						{50,	42,		52,		3,		0,		7,		8,		2,		3,      3,        0,		0,		0};
