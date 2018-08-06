/*
 *  Globals.c
 *  gBrogue
 *  A Brogue variant created by G. Reed on 4/5/17.
 *  Copyright 2017. All rights reserved.
 *
 *  This file is modified from original source:
 *
 *  Brogue
 *
 *  Created by Brian Walker on 1/10/09.
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

tcell tmap[DCOLS][DROWS];						// grids with info about the map
pcell pmap[DCOLS][DROWS];
short **scentMap;
cellDisplayBuffer displayBuffer[COLS][ROWS];	// used to optimize plotCharWithColor
short terrainRandomValues[DCOLS][DROWS][8];
short **safetyMap;								// used to help monsters flee
short **allySafetyMap;							// used to help allies flee
short **chokeMap;								// used to assess the importance of the map's various chokepoints
const short nbDirs[8][2] = {{0,-1}, {0,1}, {-1,0}, {1,0}, {-1,-1}, {-1,1}, {1,-1}, {1,1}};
const short cDirs[8][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
short numberOfWaypoints;
levelData *levels;
creature player;
playerCharacter rogue;
creature *monsters;
creature *dormantMonsters;
creature *graveyard;
creature *purgatory;
item *floorItems;
item *packItems;
item *monsterItemsHopper;

char displayedMessage[MESSAGE_LINES][COLS*2];
boolean messageConfirmed[MESSAGE_LINES];
char combatText[COLS * 2];
short messageArchivePosition;
char messageArchive[MESSAGE_ARCHIVE_LINES][COLS*2];

char currentFilePath[BROGUE_FILENAME_MAX];

char displayDetail[DCOLS][DROWS];		// used to make certain per-cell data accessible to external code (e.g. terminal adaptations)

#ifdef AUDIT_RNG
FILE *RNGLogFile;
#endif

unsigned char inputRecordBuffer[INPUT_RECORD_BUFFER + 100];
unsigned short locationInRecordingBuffer;
unsigned long randomNumbersGenerated;
unsigned long positionInPlaybackFile;
unsigned long lengthOfPlaybackFile;
unsigned long recordingLocation;
unsigned long maxLevelChanges;
char annotationPathname[BROGUE_FILENAME_MAX];	// pathname of annotation file
unsigned long previousGameSeed;

// Colors
//									Red		Green	Blue	RedRand	GreenRand	BlueRand	Rand	Dances?
// basic colors
const color white =					{100,	100,	100,	0,		0,			0,			0,		false};
const color gray =					{50,	50,		50,		0,		0,			0,			0,		false};
const color darkGray =				{30,	30,		30,		0,		0,			0,			0,		false};
const color veryDarkGray =			{15,	15,		15,		0,		0,			0,			0,		false};
const color black =					{0,		0,		0,		0,		0,			0,			0,		false};
const color yellow =				{100,	100,	0,		0,		0,			0,			0,		false};
const color darkYellow =			{50,	50,		0,		0,		0,			0,			0,		false};
const color teal =					{30,	100,	100,	0,		0,			0,			0,		false};
const color purple =				{100,	0,		100,	0,		0,			0,			0,		false};
const color darkPurple =			{50,	0,		50,		0,		0,			0,			0,		false};
const color brown =					{60,	40,		0,		0,		0,			0,			0,		false};
const color green =					{0,		100,	0,		0,		0,			0,			0,		false};
const color darkGreen =				{0,		50,		0,		0,		0,			0,			0,		false};
const color orange =				{100,	50,		0,		0,		0,			0,			0,		false};
const color darkOrange =			{50,	25,		0,		0,		0,			0,			0,		false};
const color blue =					{0,		0,		100,	0,		0,			0,			0,		false};
const color darkBlue =				{0,		0,		50,		0,		0,			0,			0,		false};
const color darkTurquoise =         {0,		40,		65,		0,		0,			0,			0,		false};
const color turquoise =             {0,		80,	    100,	0,		0,			0,			0,		false};
const color lightBlue =				{40,	40,		100,	0,		0,			0,			0,		false};
const color pink =					{100,	60,		66,		0,		0,			0,			0,		false};
const color red  =					{100,	0,		0,		0,		0,			0,			0,		false};
const color darkRed =				{50,	0,		0,		0,		0,			0,			0,		false};
const color tanColor =				{80,	67,		15,		0,		0,			0,			0,		false};

// bolt colors
const color rainbow =				{-70,	-70,	-70,	170,	170,		170,		0,		true};
const color descentBoltColor =      {-40,   -40,    -40,    0,      0,          80,         80,     true};
const color discordColor =			{25,	0,		25,		66,		0,			0,			0,		true};
const color poisonColor =			{0,		0,		0,		10,		50,			10,			0,		true};
const color beckonColor =			{10,	10,		10,		5,		5,			5,			50,		true};
const color invulnerabilityColor =	{25,	0,		25,		0,		0,			66,			0,		true};
const color dominationColor =		{0,		0,		100,	80,		25,			0,			0,		true};
const color empowermentColor =		{30,    100,	40,     25,		80,			25,			0,		true};
const color fireBoltColor =			{500,	150,	0,		45,		30,			0,			0,		true};
const color yendorLightColor =      {50,    -100,    30,     0,      0,          0,          0,      true};
const color dragonFireColor =		{500,	150,	0,      45,		30,			45,			0,		true};
const color flamedancerCoronaColor ={500,	150,	100,	45,		30,			0,			0,		true};
//const color shieldingColor =		{100,	50,		0,		0,		50,			100,		0,		true};
const color shieldingColor =		{150,	75,		0,		0,		50,			175,		0,		true};

// tile colors
const color undiscoveredColor =		{0,		0,		0,		0,		0,			0,			0,		false};

const color wallForeColor =			{7,		7,		7,		3,		3,			3,			0,		false};

color wallBackColor;
const color wallBackColorStart =	{45,	40,		40,		15,		0,			5,			20,		false};
const color wallBackColorEnd =		{40,	30,		35,		0,		20,			30,			20,		false};

const color netherWallForeColor =		{30,		30,		30,		20,		3,			3,			0,		false};
color netherWallBackColor;
const color netherWallBackColorStart =	{60,	60,		60,		2,		2,			2,			1,		false};
const color netherWallBackColorEnd =		{45,	45,		45,		1,		1,			1,			2,		false};

const color mudWallForeColor =      {55,	45,		0,		5,		5,			5,			1,		false};
//const color mudWallForeColor =      {40,	34,		7,		0,		3,			0,			3,		false};
const color mudWallBackColor =      {20,	12,		3,		8,		4,			3,			0,		false};

const color graniteBackColor =		{10,	10,		10,		0,		0,			0,			0,		false};

const color floorForeColor =		{30,	30,		30,		0,		0,			0,			35,		false};

color floorBackColor;
const color floorBackColorStart =	{2,		2,		10,		2,		2,			0,			0,		false};
const color floorBackColorEnd =		{5,		5,		5,		2,		2,			0,			0,		false};

const color stairsBackColor =		{15,	15,		5,		0,		0,			0,			0,		false};
const color firstStairsBackColor =	{10,	10,		25,		0,		0,			0,			0,		false};

const color refuseBackColor =		{6,		5,		3,		2,		2,			0,			0,		false};
const color rubbleBackColor =		{7,		7,		8,		2,		2,			1,			0,		false};
const color bloodflowerForeColor =  {30,    5,      40,     5,      1,          3,          0,      false};
const color bloodflowerPodForeColor = {50,  5,      25,     5,      1,          3,          0,      false};
const color bloodflowerBackColor =  {15,    3,      10,     3,      1,          3,          0,      false};
const color bedrollBackColor =      {10,	8,		5,		1,		1,			0,			0,		false};

const color obsidianBackColor =		{6,		0,		8,		2,		0,			3,			0,		false};
const color carpetForeColor =		{23,	30,		38,		0,		0,			0,			0,		false};
const color carpetBackColor =		{15,	8,		5,		0,		0,			0,			0,		false};
const color marbleForeColor =		{30,	23,		38,		0,		0,			0,			0,		false};
const color marbleBackColor =		{6,     5,		13,		1,		0,			1,			0,		false};
const color doorForeColor =			{70,	35,		15,		0,		0,			0,			0,		false};
const color doorBackColor =			{30,	10,		5,		0,		0,			0,			0,		false};
//const color ironDoorForeColor =		{40,	40,		40,		0,		0,			0,			0,		false};
const color ironDoorForeColor =		{500,	500,	500,	0,		0,			0,			0,		false};
const color ironDoorBackColor =		{15,	15,		30,		0,		0,			0,			0,		false};
const color bridgeFrontColor =		{33,	12,		12,		12,		7,			2,			0,		false};
const color bridgeBackColor =		{12,	3,		2,		3,		2,			1,			0,		false};
const color statueBackColor =		{20,	20,		20,		0,		0,			0,			0,		false};
const color glyphColor =            {20,    5,      5,      50,     0,          0,          0,      true};
const color glyphLightColor =       {150,   0,      0,      150,    0,          0,          0,      true};
const color sacredGlyphColor =      {5,     20,     5,      0,      50,         0,          0,      true};
const color sacredGlyphLightColor = {45,    150,	60,     25,		80,			25,			0,		true};

//const color deepWaterForeColor =	{5,		5,		40,		0,		0,			10,			10,		true};
//color deepWaterBackColor;
//const color deepWaterBackColorStart = {5,	5,		55,		5,		5,			10,			10,		true};
//const color deepWaterBackColorEnd =	{5,		5,		45,		2,		2,			5,			5,		true};
//const color shallowWaterForeColor =	{40,	40,		90,		0,		0,			10,			10,		true};
//color shallowWaterBackColor;
//const color shallowWaterBackColorStart ={30,30,		80,		0,		0,			10,			10,		true};
//const color shallowWaterBackColorEnd ={20,	20,		60,		0,		0,			5,			5,		true};

const color deepWaterForeColor =	{5,		8,		20,		0,		4,			15,			10,		true};
color deepWaterBackColor;
const color deepWaterBackColorStart = {5,	10,		31,		5,		5,			5,			6,		true};
const color deepWaterBackColorEnd =	{5,		8,		20,		2,		3,			5,			5,		true};
const color shallowWaterForeColor =	{28,	28,		60,		0,		0,			10,			10,		true};
color shallowWaterBackColor;
const color shallowWaterBackColorStart ={20,20,		60,		0,		0,			10,			10,		true};
const color shallowWaterBackColorEnd ={12,	15,		40,		0,		0,			5,			5,		true};

const color mudForeColor =			{18,	14,		5,		5,		5,			0,			0,		false};
const color mudBackColor =			{23,	17,		7,		5,		5,			0,			0,		false};
const color chasmForeColor =		{7,		7,		15,		4,		4,			8,			0,		false};
color chasmEdgeBackColor;
const color chasmEdgeBackColorStart ={5,	5,		25,		2,		2,			2,			0,		false};
const color chasmEdgeBackColorEnd =	{8,		8,		20,		2,		2,			2,			0,		false};
const color fireForeColor =			{70,	20,		0,		15,		10,			0,			0,		true};
const color lavaForeColor =			{20,	20,		20,		100,	10,			0,			0,		true};
const color brimstoneForeColor =	{100,	50,		10,		0,		50,			40,			0,		true};
const color brimstoneBackColor =	{18,	12,		9,		0,		0,			5,			0,		false};

const color lavaBackColor =			{70,	20,		0,		15,		10,			0,			0,		true};
const color acidBackColor =			{15,	80,		25,		5,		15,			10,			0,		true};

const color lightningColor =		{100,	150,	500,	50,		50,			0,			50,		true};
const color fungusLightColor =		{2,		11,		11,		4,		3,			3,			0,		true};
const color lavaLightColor =		{47,	13,		0,		10,		7,			0,			0,		true};
const color deepWaterLightColor =	{10,	30,		100,	0,		30,			100,		0,		true};

const color grassColor =			{15,	40,		15,		15,		50,			15,			10,		false};
const color deadGrassColor =		{20,	13,		0,		20,		10,			5,			10,		false};
const color fungusColor =			{15,	50,		50,		0,		25,			0,			30,		true};
const color grayFungusColor =		{30,	30,		30,		5,		5,			5,			10,		false};
const color foliageColor =			{25,	100,	25,		15,		0,			15,			0,		false};
const color deadFoliageColor =		{20,	13,		0,		30,		15,			0,			20,		false};
const color lichenColor =			{50,	5,		25,		10,		0,			5,			0,		true};
const color hayColor =				{70,	55,		5,		0,		20,			20,			0,		false};
const color ashForeColor =			{20,	20,		20,		0,		0,			0,			20,		false};
const color bonesForeColor =		{80,	80,		30,		5,		5,			35,			5,		false};
const color ectoplasmColor =		{45,	20,		55,		25,		0,			25,			5,		false};
const color forceFieldColor =		{0,		25,		25,		0,		25,			25,			0,		true};
const color wallCrystalColor =		{40,	40,		60,		20,		20,			40,			0,		true};
const color altarForeColor =		{5,		7,		9,		0,		0,			0,			0,		false};
const color altarBackColor =		{35,	18,		18,		0,		0,			0,			0,		false};
const color greenAltarBackColor =   {18,	25,		18,		0,		0,			0,			0,		false};
const color goldAltarBackColor =    {25,	24,		12,		0,		0,			0,			0,		false};
const color pedestalBackColor =		{10,	5,		20,		0,		0,			0,			0,		false};

// monster colors
const color goblinColor =			{40,	30,		20,		0,		0,			0,			0,		false};
const color jackalColor =			{60,	42,		27,		0,		0,			0,			0,		false};
const color ogreColor =				{60,	25,		25,		0,		0,			0,			0,		false};
const color eelColor =				{30,	12,		12,		0,		0,			0,			0,		false};
const color goblinConjurerColor =	{67,	10,		100,	0,		0,			0,			0,		false};
const color spectralBladeColor =	{15,	15,		60,		0,		0,			70,			50,		true};
const color spectralImageColor =	{13,	0,		0,		25,		0,			0,			0,		true};
const color toadColor =				{40,	65,		30,		0,		0,			0,			0,		false};
const color trollColor =			{40,	60,		15,		0,		0,			0,			0,		false};
const color centipedeColor =		{75,	25,		85,		0,		0,			0,			0,		false};
const color dragonColor =			{20,	80,		15,		0,		0,			0,			0,		false};
const color krakenColor =			{100,	55,		55,		0,		0,			0,			0,		false};
const color salamanderColor =		{40,	10,		0,		8,		5,			0,			0,		true};
const color pixieColor =			{60,	60,		60,		40,		40,			40,			0,		true};
const color darPriestessColor =		{0,		50,		50,		0,		0,			0,			0,		false};
const color darMageColor =			{50,	50,		0,		0,		0,			0,			0,		false};
const color wraithColor =			{66,	66,		25,		0,		0,			0,			0,		false};
const color pinkJellyColor =		{100,	40,		40,		5,		5,			5,			20,		true};
const color wormColor =				{80,	60,		40,		0,		0,			0,			0,		false};
const color sentinelColor =			{3,		3,		30,		0,		0,			10,			0,		true};
const color goblinMysticColor =		{10,	67,		100,	0,		0,			0,			0,		false};
const color ifritColor =			{50,	10,		100,	75,		0,			20,			0,		true};
const color phoenixColor =			{100,	0,		0,		0,		100,		0,			0,		true};
const color apparitionColor =		{40,	10,		55,		5,		5,			5,			10,		true};

const color arrowTurretColor =      {50,	40,		0,		40,		40,			5,			20,		true};
const color sparkTurretColor =      {50,	50,		50,		50,		50,			50,			20,		true};
const color flameTurretColor =      {50,	25,		0,		100,	50,			0,			20,		true};
const color dartTurretColor =       {50,	0,		25,		100,	0,			50,			20,		true};
const color acidTurretColor =       {5,	    75,		5,		100,	5,			50,			20,		true};

// light colors
color minersLightColor;
const color minersLightStartColor =	{180,	180,	180,	0,		0,			0,			0,		false};
const color minersLightEndColor =	{90,	90,		120,	0,		0,			0,			0,		false};
const color torchColor =			{150,	75,		30,		0,		30,			20,			0,		true};
const color torchLightColor =		{75,	38,		15,		0,		15,			7,			0,		true};
//const color hauntedTorchColor =     {75,	30,		150,	30,		20,			0,			0,		true};
const color hauntedTorchColor =     {75,	20,		40,     30,		10,			0,			0,		true};
//const color hauntedTorchLightColor ={19,     7,		37,		8,		4,			0,			0,		true};
const color hauntedTorchLightColor ={67,    10,		10,		20,		4,			0,			0,		true};
const color ifritLightColor =		{0,		10,		150,	100,	0,			100,		0,		true};
//const color unicornLightColor =		{-50,	-50,	-50,	200,	200,		200,		0,		true};
const color unicornLightColor =		{-50,	-50,	-50,	250,	250,		250,		0,		true};
const color wispLightColor =		{75,	100,	250,	33,		10,			0,			0,		true};
const color summonedImageLightColor ={200,	0,		75,		0,		0,			0,			0,		true};
const color spectralBladeLightColor ={40,	0,		230,	0,		0,			0,			0,		true};
const color ectoplasmLightColor =	{23,	10,		28,		13,		0,			13,			3,		false};
const color explosionColor =		{10,	8,		2,		0,		2,			2,			0,		true};
const color explosiveAuraColor =	{2000,	0,      -1000,  200,    200,		0,          0,		true};
const color swirlingWindColor =	{50,	50,      50,  10,    10,		10,          50,		true};

const color dartFlashColor =		{500,	500,	500,	0,		2,			2,			0,		true};
const color lichLightColor =		{-50,	80,		30,		0,		0,			20,			0,		true};
const color forceFieldLightColor =	{10,	10,		10,		0,		50,			50,			0,		true};
const color crystalWallLightColor =	{10,	10,		10,		0,		0,			50,			0,		true};
//const color sunLightColor =			{100,	100,	75,		0,		0,			0,			0,		false};
const color sunLightColor =			{100,	100,	75,		0,		0,			0,			40,		true}; // gsr
const color fungusForestLightColor ={30,	40,		60,		0,		0,			0,			40,		true};
const color fungusTrampledLightColor ={10,	10,		10,		0,		50,			50,			0,		true};
const color redFlashColor =			{100,	10,		10,		0,		0,			0,			0,		false};
const color darknessPatchColor =	{-10,	-10,	-10,	0,		0,			0,			0,		false};
const color darknessCloudColor =	{-20,	-20,	-20,	0,		0,			0,			0,		false};
const color magicMapFlashColor =	{60,	20,		60,		0,		0,			0,			0,		false};
const color sentinelLightColor =	{20,	20,		120,	10,		10,			60,			0,		true};
const color telepathyColor =		{30,	30,		130,	0,		0,			0,			0,		false};
const color teleportationColor =	{30,	130,	130,	0,		0,			0,			0,		false};

const color confusionLightColor =	{10,	10,		10,		10,		10,			10,			0,		true};
const color portalActivateLightColor ={300,	400,	500,	0,		0,			0,			0,		true};
const color descentLightColor =     {20,    20,     70,     0,      0,          0,          0,      false};
const color algaeBlueLightColor =   {20,    15,     50,     0,      0,          0,          0,      false};
const color algaeGreenLightColor =  {15,    50,     20,     0,      0,          0,          0,      false};

// flare colors
const color scrollProtectionColor =	{375,	750,	0,		0,		0,			0,          0,		true};
const color scrollEnchantmentColor ={250,	225,	300,	0,		0,			450,        0,		true};
const color potionStrengthColor =   {1000,  0,      400,	600,	0,			0,          0,		true};
const color empowermentFlashColor = {500,   1000,   600,	0,      500,		0,          0,		true};
const color genericFlashColor =     {800,   800,    800,    0,      0,          0,          0,      false};
const color summoningFlashColor =   {0,     0,      0,      600,    0,          1200,       0,      true};
const color fireFlashColor =		{750,	225,	0,		100,	50,			0,			0,		true};
const color explosionFlareColor =   {10000, 6000,   1000,   0,      0,          0,          0,      false};
const color quietusFlashColor =     {0,     -1000,  -200,   0,      0,          0,          0,      true};
const color slayingFlashColor =     {-1000, -200,   0,      0,      0,          0,          0,      true};

// color multipliers
const color colorDim25 =			{25,	25,		25,		25,		25,			25,			25,		false};
const color colorMultiplier100 =	{100,	100,	100,	100,	100,		100,		100,	false};
const color memoryColor =			{25,	25,		50,		20,		20,			20,			0,		false};
const color memoryOverlay =			{25,	25,		50,		0,		0,			0,			0,		false};
const color magicMapColor =			{60,	20,		60,		60,		20,			60,			0,		false};
const color clairvoyanceColor =		{50,	90,		50,		50,		90,			50,			66,		false};
const color telepathyMultiplier =	{30,	30,		130,	30,		30,			130,		66,		false};
const color omniscienceColor =		{140,	100,	60,		140,	100,		60,			90,		false};
const color basicLightColor =		{180,	180,	180,	180,	180,		180,		180,	false};

// blood colors
const color humanBloodColor =		{60,	20,		10,		15,		0,			0,			15,		false};
const color insectBloodColor =		{10,	60,		20,		0,		15,			0,			15,		false};
const color vomitColor =			{60,	50,		5,		0,		15,			15,			0,		false};
const color urineColor =			{70,	70,		40,		0,		0,			0,			10,		false};
const color methaneColor =			{45,	60,		15,		0,		0,			0,			0,		false};

// gas colors
const color poisonGasColor =		{75,	25,		85,		0,		0,			0,			0,		false};
const color confusionGasColor =		{60,	60,		60,		40,		40,			40,			0,		true};

// interface colors
const color itemColor =				{100,	95,		-30,	0,		0,			0,			0,		false};
const color blueBar =				{15,	10,		50,		0,		0,			0,			0,		false};
const color redBar =				{45,	10,		15,		0,		0,			0,			0,		false};
const color hiliteColor =			{100,	100,	0,		0,		0,			0,			0,		false};
const color interfaceBoxColor =		{7,		6,		15,		0,		0,			0,			0,		false};
const color interfaceButtonColor =	{18,	15,		38,		0,		0,			0,			0,		false};
const color buttonHoverColor =		{100,	70,		40,		0,		0,			0,			0,		false};
const color titleButtonColor =		{23,	15,		30,		0,		0,			0,			0,		false};

const color playerInvisibleColor =  {20,    20,     30,     0,      0,          80,         0,      true};
const color playerInLightColor =	{100,	90,     30,		0,		0,			0,			0,		false};
const color playerInShadowColor =	{60,	60,		100,	0,		0,			0,			0,		false};
const color playerInDarknessColor =	{30,	30,		65,		0,		0,			0,			0,		false};

const color inLightMultiplierColor ={150,   150,    75,     150,    150,        75,         100,    true};
//const color inDarknessMultiplierColor={66,  66,     120,    66,     66,         120,        66,     true};
const color inDarknessMultiplierColor={0,  0,     0,    66,     66,         120,        66,     true}; // gsr

const color goodMessageColor =		{60,	50,		100,	0,		0,			0,			0,		false};
const color badMessageColor =		{100,	50,		60,		0,		0,			0,			0,		false};
const color advancementMessageColor ={50,	100,	60,		0,		0,			0,			0,		false};
const color itemMessageColor =		{100,	100,	50,		0,		0,			0,			0,		false};
const color flavorTextColor =		{50,	40,		90,		0,		0,			0,			0,		false};
const color backgroundMessageColor ={60,	20,		70,		0,		0,			0,			0,		false};

const color superVictoryColor =     {150,	100,	300,	0,		0,			0,			0,		false};

//const color flameSourceColor = {0, 0, 0, 65, 40, 100, 0, true}; // 1
//const color flameSourceColor = {0, 0, 0, 80, 50, 100, 0, true}; // 2
//const color flameSourceColor = {25, 13, 25, 50, 25, 50, 0, true}; // 3
//const color flameSourceColor = {20, 20, 20, 60, 20, 40, 0, true}; // 4
//const color flameSourceColor = {30, 18, 18, 70, 36, 36, 0, true}; // 7**

const color flameSourceColor = {7, 30, 7, 40, 60, 40, 0, true}; // 8 //const color flameSourceColor = {20, 7, 7, 60, 40, 40, 0, true}; // 8
const color flameSourceColorSecondary = {6, 60, 0, 0, 10, 0, 0, true}; //const color flameSourceColorSecondary = {7, 2, 0, 10, 0, 0, 0, true};

const color flameTitleColor = {10, 5, 0, 20, 7, 5, 0, true}; // pale orange
//const color flameTitleColor = {0, 0, 0, 7, 7, 10, 0, true}; // *pale blue*
//const color flameTitleColor = {0, 0, 0, 9, 9, 15, 0, true}; // *pale blue**
//const color flameTitleColor = {0, 0, 0, 11, 11, 18, 0, true}; // *pale blue*
//const color flameTitleColor = {0, 0, 0, 15, 15, 9, 0, true}; // pale yellow
//const color flameTitleColor = {0, 0, 0, 15, 9, 15, 0, true}; // pale purple

// Dynamic color references

const color *dynamicColors[NUMBER_DYNAMIC_COLORS][3] = {
	// used color			shallow color				deep color
	{&minersLightColor,		&minersLightStartColor,		&minersLightEndColor},
	{&wallBackColor,		&wallBackColorStart,		&wallBackColorEnd},
	{&deepWaterBackColor,	&deepWaterBackColorStart,	&deepWaterBackColorEnd},
	{&shallowWaterBackColor,&shallowWaterBackColorStart,&shallowWaterBackColorEnd},
	{&floorBackColor,		&floorBackColorStart,		&floorBackColorEnd},
	{&chasmEdgeBackColor,	&chasmEdgeBackColorStart,	&chasmEdgeBackColorEnd},
	{&netherWallBackColor,		&netherWallBackColorStart,		&netherWallBackColorEnd},

};

// Autogenerator definitions

const autoGenerator autoGeneratorCatalog[NUMBER_AUTOGENERATORS] = {
//	 terrain					layer	DF							Machine						reqDungeon  reqLiquid   >Depth	<Depth          freq	minIncp	minSlope	maxNumber
    // Ordinary features of the dungeon
	{0,							0,		DF_GRANITE_COLUMN,			0,							FLOOR,		NOTHING,    1,		DEEPEST_LEVEL,	60,		100,	0,			4},
	{0,							0,		DF_CRYSTAL_WALL,			0,							WALL,       NOTHING,    14,		DEEPEST_LEVEL,	15,		-325,	25,			5},
	{0,							0,		DF_LUMINESCENT_FUNGUS,		0,							FLOOR,		NOTHING,    7,		DEEPEST_LEVEL,	15,		-300,	70,			14},
	{0,							0,		DF_GRASS,					0,							FLOOR,		NOTHING,    0,		10,             0,		1000,	-80,        10},
	{0,							0,		DF_DEAD_GRASS,				0,							FLOOR,		NOTHING,    4,		9,              0,		-200,	80,         10},
	{0,							0,		DF_DEAD_GRASS,				0,							FLOOR,		NOTHING,    9,		14,             0,		1200,	-80,        10},
	{0,							0,		DF_BONES,					0,							FLOOR,		NOTHING,    12,		DEEPEST_LEVEL-1,30,		0,		0,			4},
	{0,							0,		DF_RUBBLE,					0,							FLOOR,		NOTHING,    0,		DEEPEST_LEVEL-1,30,		0,		0,			4},
	{0,							0,		DF_FOLIAGE,					0,							FLOOR,		NOTHING,    0,		8,              15,		1000,	-333,       10},
	{0,							0,		DF_FUNGUS_FOREST,			0,							FLOOR,		NOTHING,    13,		DEEPEST_LEVEL,	30,		-600,	50,			12},
    {0,							0,		DF_BUILD_ALGAE_WELL,		0,							FLOOR,      DEEP_WATER, 10,		DEEPEST_LEVEL,	50,		0,      0,			2},
	{STATUE_INERT,				DUNGEON,0,							0,							WALL,       NOTHING,    6,		DEEPEST_LEVEL-1,5,		-100,	35,			3},
	{STATUE_INERT,				DUNGEON,0,							0,							FLOOR,		NOTHING,    10,		DEEPEST_LEVEL-1,50,		0,		0,			3},
	{TORCH_WALL,				DUNGEON,0,							0,							WALL,       NOTHING,    6,		DEEPEST_LEVEL-1,5,		-200,	70,			12},

    // Pre-revealed traps
	{GAS_TRAP_POISON,           DUNGEON,0,							0,							FLOOR,		NOTHING,    2,		4,              20,		0,      0,			1},
    {NET_TRAP,                  DUNGEON,0,							0,							FLOOR,		NOTHING,    2,		5,              20,		0,      0,			1},
	{0,                         0,      0,							MT_PARALYSIS_TRAP_AREA,		FLOOR,		NOTHING,    2,		6,              20,		0,      0,			1},
    {ALARM_TRAP,                DUNGEON,0,							0,							FLOOR,		NOTHING,    4,		7,              20,		0,      0,			1},
	{GAS_TRAP_CONFUSION,        DUNGEON,0,							0,							FLOOR,		NOTHING,    2,		10,             20,		0,      0,			1},
	{FLAMETHROWER,              DUNGEON,0,							0,							FLOOR,		NOTHING,    4,		12,             20,		0,      0,			1},
    {FLOOD_TRAP,                DUNGEON,0,							0,							FLOOR,		NOTHING,    10,		14,             20,		0,      0,			1},

    // Hidden traps
	{GAS_TRAP_POISON_HIDDEN,	DUNGEON,0,							0,							FLOOR,		NOTHING,    5,		DEEPEST_LEVEL-1,20,		100,	0,			3},
    {NET_TRAP_HIDDEN,			DUNGEON,0,							0,							FLOOR,		NOTHING,    6,		DEEPEST_LEVEL-1,20,		100,	0,			3},
	{0,                         0,      0,							MT_PARALYSIS_TRAP_HIDDEN_AREA, FLOOR,	NOTHING,    7,		DEEPEST_LEVEL-1,20,		100,	0,			3},
    {ALARM_TRAP_HIDDEN,			DUNGEON,0,							0,							FLOOR,		NOTHING,    8,		DEEPEST_LEVEL-1,20,		100,	0,			2},
	{TRAP_DOOR_HIDDEN,			DUNGEON,0,							0,							FLOOR,		NOTHING,    9,		DEEPEST_LEVEL-1,20,		100,	0,			2},
	{GAS_TRAP_CONFUSION_HIDDEN,	DUNGEON,0,							0,							FLOOR,		NOTHING,    11,		DEEPEST_LEVEL-1,20,		100,	0,			3},
	{FLAMETHROWER_HIDDEN,		DUNGEON,0,							0,							FLOOR,		NOTHING,    13,		DEEPEST_LEVEL-1,20,		100,	0,			3},
    {FLOOD_TRAP_HIDDEN,			DUNGEON,0,							0,							FLOOR,		NOTHING,    15,		DEEPEST_LEVEL-1,20,		100,	0,			3},
	{0,							0,		0,							MT_SWAMP_AREA,				FLOOR,		NOTHING,    1,		DEEPEST_LEVEL-1,30,		0,		0,			2},
	{0,							0,		DF_SUNLIGHT,				0,							FLOOR,		NOTHING,    0,		5,              15,		500,	-150,       10},
	{0,							0,		DF_DARKNESS,				0,							FLOOR,		NOTHING,    1,		15,             15,		500,	-50,        10},
	{STEAM_VENT,				DUNGEON,0,							0,							FLOOR,		NOTHING,    16,		DEEPEST_LEVEL-1,30,		100,	0,			3},
    {CRYSTAL_WALL,              DUNGEON,0,                          0,                          WALL,       NOTHING,    DEEPEST_LEVEL,DEEPEST_LEVEL,100,0,      0,          600},

    // Dewars
    {DEWAR_CAUSTIC_GAS,         DUNGEON,DF_CARPET_AREA,             0,                          FLOOR,      NOTHING,    8,      DEEPEST_LEVEL-1,2,      0,      0,          2},
    {DEWAR_CONFUSION_GAS,       DUNGEON,DF_CARPET_AREA,             0,                          FLOOR,      NOTHING,    8,      DEEPEST_LEVEL-1,2,      0,      0,          2},
    {DEWAR_PARALYSIS_GAS,       DUNGEON,DF_CARPET_AREA,             0,                          FLOOR,      NOTHING,    8,      DEEPEST_LEVEL-1,2,      0,      0,          2},
    {DEWAR_METHANE_GAS,         DUNGEON,DF_CARPET_AREA,             0,                          FLOOR,      NOTHING,    8,      DEEPEST_LEVEL-1,2,      0,      0,          2},

    // Flavor machines
    {0,							0,		DF_LUMINESCENT_FUNGUS,		0,							FLOOR,		NOTHING,    DEEPEST_LEVEL,DEEPEST_LEVEL,100,0,      0,			200},
    {0,                         0,      0,                          MT_BLOODFLOWER_AREA,        FLOOR,      NOTHING,    1,      30,             25,     140,    -10,        3},
    {0,							0,		0,							MT_SHRINE_AREA,				FLOOR,		NOTHING,    5,		AMULET_LEVEL,   7,		0,		0,          1},
	{0,							0,		0,							MT_IDYLL_AREA,				FLOOR,		NOTHING,    1,		5,              15,		0,		0,          1},
	{0,							0,		0,							MT_REMNANT_AREA,			FLOOR,		NOTHING,    10,		DEEPEST_LEVEL,	15,		0,		0,			2},
	{0,							0,		0,							MT_DISMAL_AREA,				FLOOR,		NOTHING,    7,		DEEPEST_LEVEL,	12,		0,		0,			5},
	{0,							0,		0,							MT_BRIDGE_TURRET_AREA,		FLOOR,		NOTHING,    5,		DEEPEST_LEVEL-1,6,		0,		0,			2},
	{0,							0,		0,							MT_LAKE_PATH_TURRET_AREA,	FLOOR,		NOTHING,    5,		DEEPEST_LEVEL-1,6,		0,		0,			2},
	{0,							0,		0,							MT_TRICK_STATUE_AREA,		FLOOR,		NOTHING,    6,		DEEPEST_LEVEL-1,15,		0,		0,			3},
	{0,							0,		0,							MT_SENTINEL_AREA,			FLOOR,		NOTHING,    12,		DEEPEST_LEVEL-1,10,		0,		0,			2},
	{0,							0,		0,							MT_WORM_AREA,				FLOOR,		NOTHING,    12,		DEEPEST_LEVEL-1,12,		0,		0,			3},
};

// Terrain definitions

const floorTileType tileCatalog[NUMBER_TILETYPES] = {

	// promoteChance is in hundredths of a percent per turn

	//	char		fore color				back color		priority	ignit	fireType	discovType	promoteType		promoteChance	glowLight		flags																								description			flavorText

	// dungeon layer (this layer must have all of fore color, back color and char)
	{	' ',		&black,					&black,					100,0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,																								"a chilly void",		""},
	{WALL_CHAR,		&wallBackColor,			&graniteBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a rough granite wall",	"The granite is split open with splinters of rock jutting out at odd angles."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "the ground",			""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "the ground",			""},
	{FLOOR_CHAR,	&carpetForeColor,		&carpetBackColor,		85,	0,	DF_EMBERS,		0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "the carpet",			"Ornate carpeting fills this room, a relic of ages past."},
	{FLOOR_CHAR,	&marbleForeColor,       &marbleBackColor,       85,	0,	DF_EMBERS,		0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "the marble ground",    "Light from the nearby crystals catches the grain of the lavish marble floor."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a stone wall",			"The rough stone wall is firm and unyielding."},
	{DOOR_CHAR,		&doorForeColor,			&doorBackColor,			25,	50,	DF_EMBERS,		0,			DF_OPEN_DOOR,	0,				NO_LIGHT,		(T_OBSTRUCTS_VISION | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP | TM_VISUALLY_DISTINCT), "a wooden door",	"you pass through the doorway."},
	{OPEN_DOOR_CHAR,&doorForeColor,			&doorBackColor,			25,	50,	DF_EMBERS,		0,			DF_CLOSED_DOOR,	10000,			NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),           "an open door",			"you pass through the doorway."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	50,	DF_EMBERS,		DF_SHOW_DOOR,0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET),	"a stone wall",		"The rough stone wall is firm and unyielding."},
	{DOOR_CHAR,		&ironDoorForeColor,		&ironDoorBackColor,		15,	50,	DF_EMBERS,		0,			DF_OPEN_IRON_DOOR_INERT,0,		NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_WITH_KEY | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_BRIGHT_MEMORY | TM_INTERRUPT_EXPLORATION_WHEN_SEEN | TM_INVERT_WHEN_HIGHLIGHTED),	"a locked iron door",	"you search your pack but do not have a matching key."},
	{OPEN_DOOR_CHAR,&white,                 &ironDoorBackColor,		90,	50,	DF_EMBERS,		0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VISUALLY_DISTINCT),                           "an open iron door",	"you pass through the doorway."},
	{DESCEND_CHAR,	&itemColor,				&stairsBackColor,		30,	0,	DF_PLAIN_FIRE,	0,			DF_REPEL_CREATURES, 0,			NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_PROMOTES_ON_STEP | TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_BRIGHT_MEMORY | TM_INTERRUPT_EXPLORATION_WHEN_SEEN | TM_INVERT_WHEN_HIGHLIGHTED), "a downward staircase",	"stairs spiral downward into the depths."},
	{ASCEND_CHAR,	&itemColor,				&stairsBackColor,		30,	0,	DF_PLAIN_FIRE,	0,			DF_REPEL_CREATURES, 0,			NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_PROMOTES_ON_STEP | TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_BRIGHT_MEMORY | TM_INTERRUPT_EXPLORATION_WHEN_SEEN | TM_INVERT_WHEN_HIGHLIGHTED), "an upward staircase",	"stairs spiral upward."},
	{OMEGA_CHAR,	&lightBlue,				&firstStairsBackColor,	30,	0,	DF_PLAIN_FIRE,	0,			DF_REPEL_CREATURES, 0,			NO_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_PROMOTES_ON_STEP | TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_BRIGHT_MEMORY | TM_INTERRUPT_EXPLORATION_WHEN_SEEN | TM_INVERT_WHEN_HIGHLIGHTED), "the dungeon exit",		"the gilded doors leading out of the dungeon are sealed by an invisible force."},
    {OMEGA_CHAR,	&wallCrystalColor,		&firstStairsBackColor,	30,	0,	DF_PLAIN_FIRE,	0,			DF_REPEL_CREATURES, 0,			INCENDIARY_DART_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_PROMOTES_ON_STEP | TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_BRIGHT_MEMORY | TM_INTERRUPT_EXPLORATION_WHEN_SEEN | TM_INVERT_WHEN_HIGHLIGHTED), "a crystal portal",		"dancing lights play across the plane of this sparkling crystal portal."},
	{WALL_CHAR,		&torchColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a wall-mounted torch",	"The torch is anchored firmly to the wall and sputters quietly in the gloom."},
	{WALL_CHAR,		&wallCrystalColor,		&wallCrystalColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				CRYSTAL_WALL_LIGHT,(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS | T_OBSTRUCTS_DIAGONAL_MOVEMENT), (TM_STAND_IN_TILE | TM_REFLECTS_BOLTS),"a crystal formation", "You feel the crystal's glossy surface and admire the dancing lights beneath."},
	{WALL_CHAR,		&gray,					&floorBackColor,		10,	0,	DF_PLAIN_FIRE,	0,			DF_OPEN_PORTCULLIS,	0,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_CONNECTS_LEVEL), "a heavy portcullis",	"The iron bars rattle but will not budge; they are firmly locked in place."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_ACTIVATE_PORTCULLIS,0,		NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),                                                    "the ground",			""},
	{WALL_CHAR,		&doorForeColor,			&floorBackColor,		10,	100,DF_WOODEN_BARRICADE_BURN,0,	0,				0,				NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT | TM_CONNECTS_LEVEL),"a dry wooden barricade","The wooden barricade is firmly set but has dried over the years. Might it burn?"},
	{WALL_CHAR,		&torchLightColor,		&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_PILOT_LIGHT,	0,				TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),			"a wall-mounted torch",	"The torch is anchored firmly to the wall, and sputters quietly in the gloom."},
	{FIRE_CHAR,		&fireForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING | T_IS_FIRE), (TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR),						"a fallen torch",		"The torch lies at the foot of the wall, spouting gouts of flame haphazardly."},
    {WALL_CHAR,		&torchColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_HAUNTED_TORCH_TRANSITION,0,	TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),			"a wall-mounted torch",	"The torch is anchored firmly to the wall and sputters quietly in the gloom."},
    {WALL_CHAR,		&torchColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_HAUNTED_TORCH,2000,			TORCH_LIGHT,	(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                          "a wall-mounted torch",	"The torch is anchored firmly to the wall and sputters quietly in the gloom."},
    {WALL_CHAR,		&hauntedTorchColor,		&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				HAUNTED_TORCH_LIGHT,(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),                                                   "a sputtering torch",	"A dim purple flame sputters and spits atop this wall-mounted torch."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	DF_REVEAL_LEVER,0,			0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET),			"a stone wall",			"The rough stone wall is firm and unyielding."},
    {LEVER_CHAR,	&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_PULL_LEVER,  0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_ON_PLAYER_ENTRY | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_INVERT_WHEN_HIGHLIGHTED),"a lever", "The lever moves."},
    {LEVER_PULLED_CHAR,&wallForeColor,		&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),                                                       "an inactive lever",    "The lever won't budge."},
    {WALL_CHAR,		&wallForeColor,         &wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,          DF_CREATE_LEVER,0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_IS_WIRED),											"a stone wall",			"The rough stone wall is firm and unyielding."},
    {STATUE_CHAR,	&wallBackColor,			&statueBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE),	"a marble statue",	"The cold marble statue has weathered the years with grace."},
	{STATUE_CHAR,	&wallBackColor,			&statueBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			DF_CRACKING_STATUE,0,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),"a marble statue",	"The cold marble statue has weathered the years with grace."},
	{STATUE_CHAR,	&wallBackColor,			&statueBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			DF_STATUE_SHATTER,3500,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_LIST_IN_SIDEBAR),"a cracking statue",	"Deep cracks ramble down the side of the statue even as you watch."},
    {STATUE_CHAR,	&wallBackColor,			&statueBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			DF_STATUE_SHATTER,0,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),"a marble statue",	"The cold marble statue has weathered the years with grace."},
	{OMEGA_CHAR,	&wallBackColor,			&floorBackColor,		17,	0,	DF_PLAIN_FIRE,	0,			DF_PORTAL_ACTIVATE,0,			NO_LIGHT,		(T_OBSTRUCTS_ITEMS), (TM_STAND_IN_TILE | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),  "a stone archway",		"This ancient moss-covered stone archway radiates a strange, alien energy."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_TURRET_EMERGE,0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),			"a stone wall",			"The rough stone wall is firm and unyielding."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_WALL_SHATTER,0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),			"a stone wall",			"The rough stone wall is firm and unyielding."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_DARKENING_FLOOR,	0,			NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),                                                    "the ground",			""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_DARK_FLOOR,	1500,			NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION),                                                                  "the ground",			""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,				0,				DARKNESS_CLOUD_LIGHT, 0, 0,                                                                                         "the ground",			""},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_ON_PLAYER_ENTRY),                      "the ground",			""},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			0,				0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),							"a candle-lit altar",	"a gilded altar is adorned with candles that flicker in the breeze."},
	{GEM_CHAR,		&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			0,				0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_PROMOTES_WITH_KEY | TM_IS_WIRED | TM_LIST_IN_SIDEBAR),           "a candle-lit altar",	"ornate gilding spirals around a spherical depression in the top of the altar."},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			DF_ITEM_CAGE_CLOSE,	0,			CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_WITHOUT_KEY | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),"a candle-lit altar",	"a cage, open on the bottom, hangs over this altar on a retractable chain."},
	{WALL_CHAR,		&altarBackColor,		&veryDarkGray,			17, 0,	0,				0,			DF_ITEM_CAGE_OPEN,	0,			CANDLE_LIGHT,	(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_WITH_KEY | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),"an iron cage","the missing item must be replaced before you can access the remaining items."},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			DF_ALTAR_INERT,	0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_ON_ITEM_PICKUP | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),	"a candle-lit altar",	"a weathered stone altar is adorned with candles that flicker in the breeze."},
	{ALTAR_CHAR,	&altarForeColor,		&altarBackColor,		17, 0,	0,				0,			DF_ALTAR_RETRACT,0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_ON_ITEM_PICKUP | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),	"a candle-lit altar",	"a weathered stone altar is adorned with candles that flicker in the breeze."},
	{WALL_CHAR,		&altarBackColor,		&veryDarkGray,			17, 0,	0,				0,			DF_CAGE_DISAPPEARS,	0,			CANDLE_LIGHT,	(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),"an iron cage","the cage won't budge. Perhaps there is a way to raise it nearby..."},
	{ALTAR_CHAR,	&altarForeColor,		&pedestalBackColor,		17, 0,	0,				0,			0,				0,				CANDLE_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), 0,																	"a stone pedestal",		"elaborate carvings wind around this ancient pedestal."},
	{ALTAR_CHAR,	&floorBackColor,		&veryDarkGray,			17, 0,	0,				0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),																			"an open cage",			"the interior of the cage is filthy and reeks of decay."},
	{WALL_CHAR,		&gray,					&darkGray,				17, 0,	0,				0,			DF_MONSTER_CAGE_OPENS,	0,		NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS | T_OBSTRUCTS_GAS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_WITH_KEY | TM_LIST_IN_SIDEBAR | TM_INTERRUPT_EXPLORATION_WHEN_SEEN),"a locked iron cage","the bars of the cage are firmly set and will not budge."},
	{ALTAR_CHAR,	&bridgeFrontColor,		&bridgeBackColor,		17,	20,	DF_COFFIN_BURNS,0,			DF_COFFIN_BURSTS,0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_IS_WIRED | TM_VANISHES_UPON_PROMOTION | TM_LIST_IN_SIDEBAR),                  "a sealed coffin",		"a coffin made from thick wooden planks rests in a bed of moss."},
	{ALTAR_CHAR,	&black,					&bridgeBackColor,		17,	20,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_LIST_IN_SIDEBAR),				"an empty coffin",		"an open wooden coffin rests in a bed of moss."},

	// traps (part of dungeon layer):
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_POISON_GAS_CLOUD, DF_SHOW_POISON_GAS_TRAP, 0, 0,			NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET),                                                                     "the ground",			""},
	{TRAP_CHAR,		&poisonGasColor,		0,                      30,	0,	DF_POISON_GAS_CLOUD, 0,		0,				0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),                                        "a caustic gas trap",	"there is a hidden pressure plate in the floor above a reserve of caustic gas."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_POISON_GAS_CLOUD, DF_SHOW_TRAPDOOR,0,	0,				NO_LIGHT,		(T_AUTO_DESCENT), (TM_IS_SECRET),                                                                   "the ground",			"you plunge through a hidden trap door!"},
	{CHASM_CHAR,	&chasmForeColor,		&black,					30,	0,	DF_POISON_GAS_CLOUD,0,      0,				0,				NO_LIGHT,		(T_AUTO_DESCENT), 0,                                                                                "a hole",				"you plunge through a hole in the ground!"},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	0,              DF_SHOW_PARALYSIS_GAS_TRAP, 0, 0,           NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET | TM_IS_WIRED),                                                       "the ground",			""},
	{TRAP_CHAR,		&pink,					0,              		30,	0,	0,              0,          0,				0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),                          "a paralysis trigger",	"there is a hidden pressure plate in the floor."},
    {FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	DF_DISCOVER_PARALYSIS_VENT, DF_PARALYSIS_VENT_SPEW,0,NO_LIGHT,	(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET | TM_IS_WIRED),                                 "the ground",			""},
	{VENT_CHAR,		&pink,                  0,              		30,	0,	DF_PLAIN_FIRE,	0,			DF_PARALYSIS_VENT_SPEW,0,		NO_LIGHT,		(0), (TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),                                     "an inactive gas vent",	"A dormant gas vent is connected to a reserve of paralytic gas."},

	// Confusion traps are a pain -- gsr
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_STEAM_PUFF,DF_SHOW_CONFUSION_GAS_TRAP, 0,0,NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET),                                                                     "the ground",			""},
	{TRAP_CHAR,		&white,         		0,              		30,	0,	DF_STEAM_PUFF,0,	0,			0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),                                        "a steam trap",		"A hidden pressure plate accompanies a reserve of scalding steam."},

	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_FLAMETHROWER,	DF_SHOW_FLAMETHROWER_TRAP, 0,	0,		NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET),                                                                     "the ground",			""},
	{TRAP_CHAR,		&red,                   0,              		30,	0,	DF_FLAMETHROWER,	0,		0,				0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),										"a fire trap",			"A hidden pressure plate is connected to a crude flamethrower mechanism."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_FLOOD,		DF_SHOW_FLOOD_TRAP, 0,		0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET),                                                                     "the ground",			""},
	{TRAP_CHAR,		&blue,                  0,              		58,	0,	DF_FLOOD,		0,			0,				0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),										"a flood trap",			"A hidden pressure plate is connected to floodgates in the walls and ceiling."},
    {FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_NET,         DF_SHOW_NET_TRAP, 0,        0,              NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET),                                                                     "the ground",			""},
	{TRAP_CHAR,		&tanColor,              0,                      30,	0,	DF_NET,         0,          0,				0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),                                        "a net trap",           "you see netting subtly concealed in the ceiling over a hidden pressure plate."},
    {FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_AGGRAVATE_TRAP, DF_SHOW_ALARM_TRAP, 0,   0,              NO_LIGHT,		(T_IS_DF_TRAP), (TM_IS_SECRET),                                                                     "the ground",			""},
	{TRAP_CHAR,		&gray,                  0,                      30,	0,	DF_AGGRAVATE_TRAP, 0,       0,				0,				NO_LIGHT,		(T_IS_DF_TRAP), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),                                        "an alarm trap",        "a hidden pressure plate is connected to a loud alarm mechanism."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	DF_SHOW_POISON_GAS_VENT, DF_POISON_GAS_VENT_OPEN, 0, NO_LIGHT, (0), (TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET | TM_IS_WIRED),                                  "the ground",			""},
	{VENT_CHAR,		&floorForeColor,		0,              		30,	0,	DF_PLAIN_FIRE,	0,			DF_POISON_GAS_VENT_OPEN,0,		NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),		"an inactive gas vent",	"An inactive gas vent is hidden in a crevice in the ground."},
	{VENT_CHAR,		&floorForeColor,		0,              		30,	0,	DF_PLAIN_FIRE,	0,			DF_VENT_SPEW_POISON_GAS,10000,	NO_LIGHT,		0, (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),														"a gas vent",			"Clouds of caustic gas are wafting out of a hidden vent in the floor."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	DF_SHOW_METHANE_VENT, DF_METHANE_VENT_OPEN,0,NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET | TM_IS_WIRED),                                     "the ground",			""},
	{VENT_CHAR,		&floorForeColor,		0,              		30,	0,	DF_PLAIN_FIRE,	0,			DF_METHANE_VENT_OPEN,0,			NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),		"an inactive gas vent",	"An inactive gas vent is hidden in a crevice in the ground."},
	{VENT_CHAR,		&floorForeColor,		0,              		30,	15,	DF_EMBERS,		0,			DF_VENT_SPEW_METHANE,5000,		NO_LIGHT,		(T_IS_FLAMMABLE), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),										"a gas vent",			"Clouds of explosive gas are wafting out of a hidden vent in the floor."},
	{VENT_CHAR,		&gray,					0,              		15,	15,	DF_EMBERS,		0,			DF_STEAM_PUFF,	250,			NO_LIGHT,		T_OBSTRUCTS_ITEMS, (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),										"a steam vent",			"A natural crevice in the floor periodically vents scalding gouts of steam."},
	{TRAP_CHAR,		&white,					&chasmEdgeBackColor,	15,	0,	0,				0,			DF_MACHINE_PRESSURE_PLATE_USED,0,NO_LIGHT,      (T_IS_DF_TRAP), (TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP | TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),"a pressure plate",		"There is an exposed pressure plate here. A thrown item might trigger it."},
    {TRAP_CHAR,		&darkGray,				&chasmEdgeBackColor,	15,	0,	0,				0,			0,				0,				NO_LIGHT,		0, (TM_LIST_IN_SIDEBAR),                                                                            "an inactive pressure plate", "This pressure plate has already been depressed."},
    {CHASM_CHAR,	&glyphColor,            0,                      42,	0,	0,              0,          DF_INACTIVE_GLYPH,0,			GLYPH_LIGHT_DIM,(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_ON_PLAYER_ENTRY | TM_VISUALLY_DISTINCT),"a magical glyph",      "A strange glyph, engraved into the floor, flickers with magical light."},
    {CHASM_CHAR,	&glyphColor,            0,                      42,	0,	0,              0,          DF_ACTIVE_GLYPH,10000,			GLYPH_LIGHT_BRIGHT,(0), (TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),                                        "a glowing glyph",      "A strange glyph, engraved into the floor, radiates magical light."},
    {DEWAR_CHAR,    &poisonGasColor,        &darkGray,              10, 20, DF_DEWAR_CAUSTIC,0,         DF_DEWAR_CAUSTIC,0,             NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT | TM_LIST_IN_SIDEBAR | TM_PROMOTES_ON_PLAYER_ENTRY | TM_INVERT_WHEN_HIGHLIGHTED),"a glass dewar of caustic gas", ""},
    {DEWAR_CHAR,    &confusionGasColor,     &darkGray,              10, 20, DF_DEWAR_CONFUSION,0,       DF_DEWAR_CONFUSION,0,           NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT | TM_LIST_IN_SIDEBAR | TM_PROMOTES_ON_PLAYER_ENTRY | TM_INVERT_WHEN_HIGHLIGHTED),"a glass dewar of confusion gas", ""},
    {DEWAR_CHAR,    &pink,                  &darkGray,              10, 20, DF_DEWAR_PARALYSIS,0,       DF_DEWAR_PARALYSIS,0,           NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT | TM_LIST_IN_SIDEBAR | TM_PROMOTES_ON_PLAYER_ENTRY | TM_INVERT_WHEN_HIGHLIGHTED),"a glass dewar of paralytic gas", ""},
    {DEWAR_CHAR,    &methaneColor,          &darkGray,              10, 20, DF_DEWAR_METHANE,0,         DF_DEWAR_METHANE,0,             NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_SURFACE_EFFECTS | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT | TM_LIST_IN_SIDEBAR | TM_PROMOTES_ON_PLAYER_ENTRY | TM_INVERT_WHEN_HIGHLIGHTED),"a glass dewar of methane gas", ""},

	// liquid layer
	{LIQUID_CHAR,	&deepWaterForeColor,	&deepWaterBackColor,	40,	100,DF_STEAM_ACCUMULATION,	0,	0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_IS_DEEP_WATER), (TM_ALLOWS_SUBMERGING | TM_STAND_IN_TILE | TM_EXTINGUISHES_FIRE),"the murky waters",    "water currents seemingly flow in several directions at once."}, // changed to account for water walking -- gsr
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	55,	0,	DF_STEAM_ACCUMULATION,	0,	0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),                              "shallow water",		"this water is cold and shallow."}, // changed to account for water walking -- gsr
	{MUD_CHAR,		&mudForeColor,			&mudBackColor,			55,	0,	DF_PLAIN_FIRE,	0,			DF_METHANE_GAS_PUFF, 100,		NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_ALLOWS_SUBMERGING),                                                     "a bog",				"you are knee-deep in thick, foul-smelling mud."},
	{CHASM_CHAR,	&chasmForeColor,		&black,					40,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_AUTO_DESCENT), (TM_STAND_IN_TILE),																"a chasm",				"you plunge downward into the chasm!"},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "the brink of a chasm",	"chilly winds blow upward from the stygian depths."},
	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_SPREADABLE_COLLAPSE,0,		NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),                                                    "the ground",			""},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	45,	0,	DF_PLAIN_FIRE,	0,			DF_COLLAPSE_SPREADS,2500,		NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION),                                                                  "the crumbling ground",	"cracks are appearing in the ground beneath your feet!"},
	{LIQUID_CHAR,	&fireForeColor,			&lavaBackColor,			40,	0,	DF_OBSIDIAN,	0,			0,				0,				LAVA_LIGHT,		(T_LAVA_INSTA_DEATH), (TM_STAND_IN_TILE | TM_ALLOWS_SUBMERGING),									"lava",					"searing heat rises from the lava."},
	{LIQUID_CHAR,	&fireForeColor,			&lavaBackColor,			40,	0,	DF_OBSIDIAN,	0,			DF_RETRACTING_LAVA,	0,			LAVA_LIGHT,		(T_LAVA_INSTA_DEATH), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_ALLOWS_SUBMERGING),"lava","searing heat rises from the lava."},
	{LIQUID_CHAR,	&fireForeColor,			&lavaBackColor,			40,	0,	DF_OBSIDIAN,	0,			DF_OBSIDIAN_WITH_STEAM,	-1500,	LAVA_LIGHT,		(T_LAVA_INSTA_DEATH), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_ALLOWS_SUBMERGING),		"cooling lava",         "searing heat rises from the lava."},


//	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_PLAIN_FIRE,	0,			0,				0,				SUN_LIGHT,		(0), (TM_STAND_IN_TILE),																			"a patch of sunlight",	"sunlight streams through cracks in the ceiling."},
//	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		90,	0,	DF_PLAIN_FIRE,	0,			0,				0,				DARKNESS_PATCH_LIGHT,	(0), 0,																						"a patch of shadows",	"this area happens to be cloaked in shadows -- perhaps a safe place to hide."},

// more defined sun and shade --gsr
	{FLOOR_CHAR,	&torchLightColor,         		&floorBackColor,		90,	0,	DF_PLAIN_FIRE,	0,			0,				0,				SUN_LIGHT,		(0), (TM_STAND_IN_TILE),																			"a patch of sunlight",	"sunlight streams through cracks in the ceiling."},
	{FLOOR_CHAR,	&floorForeColor,		        &black,        90,	0,	DF_PLAIN_FIRE,	0,			0,				0,				DARKNESS_PATCH_LIGHT,	(0), 0,																						"a patch of shadows",	"this area happens to be cloaked in shadows -- perhaps a safe place to hide."},

	{ASH_CHAR,		&brimstoneForeColor,	&brimstoneBackColor,	40, 100,DF_INERT_BRIMSTONE,	0,		DF_INERT_BRIMSTONE,	10,			NO_LIGHT,		(T_IS_FLAMMABLE | T_SPONTANEOUSLY_IGNITES), 0,                                                      "hissing brimstone",	"the jagged brimstone hisses and spits ominously as it crunches under your feet."},
	{ASH_CHAR,		&brimstoneForeColor,	&brimstoneBackColor,	40, 0,	DF_INERT_BRIMSTONE,	0,		DF_ACTIVE_BRIMSTONE, 800,		NO_LIGHT,		(T_SPONTANEOUSLY_IGNITES), 0,                                                                       "hissing brimstone",	"the jagged brimstone hisses and spits ominously as it crunches under your feet."},
	{FLOOR_CHAR,	&darkGray,				&obsidianBackColor,		50,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "the obsidian ground",	"the ground has fused into obsidian."},
	{BRIDGE_CHAR,	&bridgeFrontColor,		&bridgeBackColor,		45,	50,	DF_BRIDGE_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "a rickety rope bridge","the rickety rope bridge creaks underfoot."},
	{BRIDGE_CHAR,	&bridgeFrontColor,		&bridgeBackColor,		45,	50,	DF_BRIDGE_FALL,	0,			DF_BRIDGE_FALL, 10000,			NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "a plummeting bridge",  "the bridge is plunging into the chasm before your eyes!"},
	{BRIDGE_CHAR,	&bridgeFrontColor,		&bridgeBackColor,		45,	50,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "a rickety rope bridge","the rickety rope bridge is staked to the edge of the chasm."},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	20,	50,	DF_BRIDGE_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "a stone bridge",		"the narrow stone bridge winds precariously across the chasm."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	60,	0,	DF_STEAM_ACCUMULATION,	0,	DF_SPREADABLE_WATER,0,			NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),	"shallow water",	"the water is cold and reaches your knees."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	60,	0,	DF_STEAM_ACCUMULATION,	0,	DF_WATER_SPREADS,2500,			NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),	"shallow water",		"the water is cold and reaches your knees."},
	{MUD_CHAR,		&mudForeColor,			&mudBackColor,			55,	0,	DF_PLAIN_FIRE,	0,			DF_MUD_ACTIVATE,0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_ALLOWS_SUBMERGING),			"a bog",				"you are knee-deep in thick, foul-smelling mud."},

	// surface layer
	{CHASM_CHAR,	&chasmForeColor,		&black,					9,	0,	DF_PLAIN_FIRE,	0,			DF_HOLE_DRAIN,	-1000,			NO_LIGHT,		(T_AUTO_DESCENT), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "a hole",				"you plunge downward into the hole!"},
    {CHASM_CHAR,	&chasmForeColor,		&black,					9,	0,	DF_PLAIN_FIRE,	0,			DF_HOLE_DRAIN,	-1000,			DESCENT_LIGHT,	(T_AUTO_DESCENT), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "a hole",				"you plunge downward into the hole!"},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	50,	0,	DF_PLAIN_FIRE,	0,			0,				-500,			NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION),																	"translucent ground",	"chilly gusts of air blow upward through the translucent floor."},
	{LIQUID_CHAR,	&deepWaterForeColor,	&deepWaterBackColor,	41,	100,DF_STEAM_ACCUMULATION,	0,	DF_FLOOD_DRAIN,	-200,			NO_LIGHT,		(T_IS_FLAMMABLE | T_IS_DEEP_WATER), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING), "sloshing water", "roiling water floods the room."},
	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	50,	0,	DF_STEAM_ACCUMULATION,	0,	DF_PUDDLE,		-100,			NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),	"shallow water",		"knee-deep water drains slowly into holes in the floor."},
	{GRASS_CHAR,	&grassColor,			0,						60,	15,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "grass-like fungus",	"grass-like fungus crunches underfoot."},
	{GRASS_CHAR,	&deadGrassColor,		0,						60,	40,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "withered fungus",		"dead fungus covers the ground."},
	{GRASS_CHAR,	&grayFungusColor,		0,						51,	10,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "withered fungus",		"groping tendrils of pale fungus rise from the muck."},
	{GRASS_CHAR,	&fungusColor,			0,						60,	10,	DF_PLAIN_FIRE,	0,			0,				0,				FUNGUS_LIGHT,	(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "luminescent fungus",	"luminescent fungus casts a pale, eerie glow."},
	{GRASS_CHAR,	&lichenColor,			0,						60,	50,	DF_PLAIN_FIRE,	0,			DF_LICHEN_GROW,	10000,			NO_LIGHT,		(T_CAUSES_POISON | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                "deadly lichen",		"venomous barbs cover the quivering tendrils of this fast-growing lichen."},
	{GRASS_CHAR,	&hayColor,				&refuseBackColor,		57,	50,	DF_STENCH_BURN,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "filthy hay",			"a pile of hay, matted with filth, has been arranged here as a makeshift bed."},
	{FLOOR_CHAR,	&humanBloodColor,		0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pool of blood",		"the floor is splattered with blood."},
	{FLOOR_CHAR,	&insectBloodColor,		0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pool of green blood", "the floor is splattered with green blood."},
	{FLOOR_CHAR,	&poisonGasColor,		0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pool of purple blood", "the floor is splattered with purple blood."},
	{FLOOR_CHAR,	&acidBackColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		0, 0,                                                                                               "the acid-flecked ground", "the floor is splattered with acid."},
	{FLOOR_CHAR,	&vomitColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a puddle of vomit",	"the floor is caked with vomit."},
	{FLOOR_CHAR,	&urineColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				100,			NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                               "a puddle of urine",	"a puddle of urine covers the ground."},
	{FLOOR_CHAR,	&white,					0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				UNICORN_POOP_LIGHT,(0), (TM_STAND_IN_TILE),                                                                         "unicorn poop",			"a pile of lavender-scented unicorn poop sparkles with rainbow light."},
	{FLOOR_CHAR,	&wormColor,				0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pool of worm entrails", "worm viscera cover the ground."},
	{ASH_CHAR,		&ashForeColor,			0,						80,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pile of ashes",		"charcoal and ash crunch underfoot."},
	{ASH_CHAR,		&ashForeColor,			0,						87,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "burned carpet",		"the carpet has been scorched by an ancient fire."},
	{FLOOR_CHAR,	&shallowWaterBackColor,	0,						80,	20,	0,				0,			0,				100,			NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                  "a puddle of water",	"a puddle of water covers the ground."},
	{BONES_CHAR,	&bonesForeColor,		0,						70,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pile of bones",		"unidentifiable bones, yellowed with age, litter the ground."},
	{BONES_CHAR,	&gray,					0,						70,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pile of rubble",		"rocky rubble covers the ground."},
	{BONES_CHAR,	&mudBackColor,			&refuseBackColor,		50,	20,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(0), (TM_STAND_IN_TILE),                                                                            "a pile of filthy effects","primitive tools, carvings and trinkets are strewn about the area."},
    {BONES_CHAR,    &white,                 0,                      70, 0,  DF_PLAIN_FIRE,  0,          0,              0,              NO_LIGHT,       (0), (TM_STAND_IN_TILE),                                                                            "shattered glass",      "jagged chunks of glass litter the ground."},
	{FLOOR_CHAR,	&ectoplasmColor,		0,						70,	0,	DF_PLAIN_FIRE,	0,			0,				0,				ECTOPLASM_LIGHT,(0), (TM_STAND_IN_TILE),                                                                            "ectoplasmic residue",	"a thick, glowing substance has congealed on the ground."},
	{ASH_CHAR,		&fireForeColor,			0,						70,	0,	DF_PLAIN_FIRE,	0,			DF_ASH,			300,			EMBER_LIGHT,	(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                               "sputtering embers",	"sputtering embers cover the ground."},
	{WEB_CHAR,		&white,					0,						19,	100,DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_ENTANGLES | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),"a spiderweb",       "thick, sticky spiderwebs fill the area."},
    {WEB_CHAR,		&brown,                 0,						19,	40, DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_ENTANGLES | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),"a net",             "a dense tangle of netting fills the area."},
	{FOLIAGE_CHAR,	&foliageColor,			0,						45,	15,	DF_PLAIN_FIRE,	0,			DF_TRAMPLED_FOLIAGE, 0,			NO_LIGHT,		(T_OBSTRUCTS_VISION | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP), "dense foliage",   "dense foliage fills the area, thriving on what sunlight trickles in."},
	{FOLIAGE_CHAR,	&deadFoliageColor,		0,						45,	80,	DF_PLAIN_FIRE,	0,			DF_SMALL_DEAD_GRASS, 0,			NO_LIGHT,		(T_OBSTRUCTS_VISION | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP), "dead foliage",    "the decaying husk of a fungal growth fills the area."},
	{TRAMPLED_FOLIAGE_CHAR,&foliageColor,	0,						60,	15,	DF_PLAIN_FIRE,	0,			DF_FOLIAGE_REGROW, 100,			NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "trampled foliage",		"dense foliage fills the area, thriving on what sunlight trickles in."},
	{FOLIAGE_CHAR,	&fungusForestLightColor,0,						45,	15,	DF_PLAIN_FIRE,	0,			DF_TRAMPLED_FUNGUS_FOREST, 0,	FUNGUS_FOREST_LIGHT,(T_OBSTRUCTS_VISION | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP),"a luminescent fungal forest", "luminescent fungal growth fills the area, groping upward from the rich soil."},
	{TRAMPLED_FOLIAGE_CHAR,&fungusForestLightColor,0,				60,	15,	DF_PLAIN_FIRE,	0,			DF_FUNGUS_FOREST_REGROW, 100,	FUNGUS_LIGHT,	(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "trampled fungal foliage", "luminescent fungal growth fills the area, groping upward from the rich soil."},
	{WALL_CHAR,		&forceFieldColor,		&forceFieldColor,		0,	0,	0,				0,			DF_FORCEFIELD_MELT, -200,		FORCEFIELD_LIGHT, (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_GAS | T_OBSTRUCTS_DIAGONAL_MOVEMENT), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP),		"a green crystal",		"The translucent green crystal is melting away in front of your eyes."},
	{WALL_CHAR,		&black,                 &forceFieldColor,		0,	0,	0,				0,			0,				-10000,			FORCEFIELD_LIGHT, (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_GAS | T_OBSTRUCTS_DIAGONAL_MOVEMENT), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),		"a dissolving crystal",		"The translucent green crystal is melting away in front of your eyes."},
    {CHASM_CHAR,    &sacredGlyphColor,      0,                      57, 0,  0,              0,          0,              0,              SACRED_GLYPH_LIGHT, (T_SACRED), 0,                                                                                  "a sacred glyph",       "a sacred glyph adorns the floor, glowing with a powerful warding enchantment."},
	{CHAIN_TOP_LEFT,&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the ceiling."},
	{CHAIN_BOTTOM_RIGHT, &gray,				0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the floor."},
	{CHAIN_TOP_RIGHT, &gray,				0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the ceiling."},
	{CHAIN_BOTTOM_LEFT, &gray,				0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the floor."},
	{CHAIN_TOP,		&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{CHAIN_BOTTOM,	&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{CHAIN_LEFT,	&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{CHAIN_RIGHT,	&gray,					0,						20,	0,	0,				0,			0,				0,				NO_LIGHT,		0, 0,																								"an iron manacle",		"a thick iron manacle is anchored to the wall."},
	{0,				0,						0,						1,	0,	0,				0,			0,				10000,			PORTAL_ACTIVATE_LIGHT,(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),											"blinding light",		"blinding light streams out of the archway."},
    {0,				0,						0,						100,0,	0,				0,			0,				10000,			GLYPH_LIGHT_BRIGHT,(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                            "a red glow",           "a red glow fills the area."},

	// fire tiles
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,				0,			DF_EMBERS,		500,			FIRE_LIGHT,		(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),				"billowing flames",		"flames billow upward."},
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,				0,			0,				2500,			BRIMSTONE_FIRE_LIGHT,(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),           "sulfurous flames",		"sulfurous flames leap from the unstable bed of brimstone."},
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,				0,			DF_OBSIDIAN,	5000,			FIRE_LIGHT,		(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),				"clouds of infernal flame", "billowing infernal flames eat at the floor."},
	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	0,              0,			0,              8000,			FIRE_LIGHT,		(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),				"a cloud of burning gas", "burning gas fills the air with flame."},
	{FIRE_CHAR,		&yellow,				0,						10,	0,	0,				0,			0,              10000,			EXPLOSION_LIGHT,(T_IS_FIRE | T_CAUSES_EXPLOSIVE_DAMAGE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT), "a violent explosion",	"the force of the explosion slams into you."},
	{FIRE_CHAR,		&white,					0,						10,	0,	0,				0,			0,				10000,			INCENDIARY_DART_LIGHT ,(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),			"a flash of fire",		"flames burst out of the incendiary dart."},
    {FIRE_CHAR,		&white,                 0,						10,	0,	0,				0,			DF_EMBERS,		3000,			FIRE_LIGHT,		(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),				"crackling flames",		"crackling flames rise from the blackened item."},
    {FIRE_CHAR,		&white,                 0,						10,	0,	0,				0,			DF_EMBERS,		3000,			FIRE_LIGHT,		(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),				"greasy flames",		"greasy flames rise from the corpse."},

	// gas layer
	{	' ',		0,						&poisonGasColor,		35,	100,DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_CAUSES_DAMAGE), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES),							"a cloud of caustic gas", "you can feel the purple gas eating at your flesh."},
	{	' ',		0,						&confusionGasColor,		35,	100,DF_GAS_FIRE,	0,			0,				0,				CONFUSION_GAS_LIGHT,(T_IS_FLAMMABLE | T_CAUSES_CONFUSION), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES_QUICKLY),			"a cloud of confusion gas", "the rainbow-colored gas tickles your brain."},
	{	' ',		0,						&vomitColor,			35,	100,DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_CAUSES_NAUSEA), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES_QUICKLY),					"a cloud of putrescence", "the stench of rotting flesh is overpowering."},
    {	' ',		0,						&vomitColor,			35,	0,  DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_CAUSES_NAUSEA), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES_QUICKLY),                                  "a cloud of putrid smoke", "you retch violently at the smell of the greasy smoke."},
	{	' ',		0,						&pink,					35,	100,DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE | T_CAUSES_PARALYSIS), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES_QUICKLY),				"a cloud of paralytic gas", "the pale gas causes your muscles to stiffen."},
	{	' ',		0,						&methaneColor,			35,	100,DF_GAS_FIRE,    0,          DF_EXPLOSION_FIRE, 0,			NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_EXPLOSIVE_PROMOTE),                                        "a cloud of explosive gas",	"the smell of explosive swamp gas fills the air."},
	{	' ',		0,						&white,					35,	0,	DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_CAUSES_DAMAGE), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES_QUICKLY),									"a cloud of scalding steam", "scalding steam fills the air!"},
	{	' ',		0,						0,						35,	0,	DF_GAS_FIRE,	0,			0,				0,				DARKNESS_CLOUD_LIGHT,	(0), (TM_STAND_IN_TILE),                                                                    "a cloud of supernatural darkness", "everything is obscured by an aura of supernatural darkness."},
	{	' ',		0,						&darkRed,				35,	0,	DF_GAS_FIRE,	0,			0,				0,				NO_LIGHT,		(T_CAUSES_HEALING), (TM_STAND_IN_TILE | TM_GAS_DISSIPATES_QUICKLY),                                 "a cloud of healing spores", "bloodwort spores, renowned for their healing properties, fill the air."},

    // bloodwort pods
    {FOLIAGE_CHAR,  &bloodflowerForeColor,  &bloodflowerBackColor,  10, 20, DF_PLAIN_FIRE,  0,          DF_BLOODFLOWER_PODS_GROW, 100,  NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_IS_FLAMMABLE), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT), "a bloodwort stalk", "this spindly plant grows seed pods famous for their healing properties."},
    {GOLD_CHAR,     &bloodflowerPodForeColor, 0,                    11, 20, DF_BLOODFLOWER_POD_BURST,0, DF_BLOODFLOWER_POD_BURST, 0,    NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_PLAYER_ENTRY | TM_VISUALLY_DISTINCT | TM_INVERT_WHEN_HIGHLIGHTED), "a bloodwort pod", "the bloodwort seed pod bursts, releasing a cloud of healing spores."},

    // shrine accoutrements
    {BRIDGE_CHAR,	&black,                 &bedrollBackColor,      57,	50,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "an abandoned bedroll", "a bedroll lies in the corner, disintegrating with age."},

    // algae
    {FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			DF_ALGAE_1,		100,			NO_LIGHT,		0, 0,                                                                                               "the ground",			""},
    {LIQUID_CHAR,	&deepWaterForeColor,    &deepWaterBackColor,	40,	100,DF_STEAM_ACCUMULATION,	0,	DF_ALGAE_1,     500,			LUMINESCENT_ALGAE_BLUE_LIGHT,(T_IS_FLAMMABLE | T_IS_DEEP_WATER), (TM_STAND_IN_TILE | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),	"luminescent waters",	"blooming algae fills the waters with a swirling luminescence."},
    {LIQUID_CHAR,	&deepWaterForeColor,    &deepWaterBackColor,	39,	100,DF_STEAM_ACCUMULATION,	0,	DF_ALGAE_REVERT,300,			LUMINESCENT_ALGAE_GREEN_LIGHT,(T_IS_FLAMMABLE | T_IS_DEEP_WATER), (TM_STAND_IN_TILE | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),	"luminescent waters",	"blooming algae fills the waters with a swirling luminescence."},

    // ancient spirit terrain
	{VINE_CHAR,		&lichenColor,           0,						19,	100,DF_PLAIN_FIRE,	0,			DF_ANCIENT_SPIRIT_GRASS,1000,	NO_LIGHT,		(T_ENTANGLES | T_CAUSES_DAMAGE | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT | TM_PROMOTES_ON_PLAYER_ENTRY),"thorned vines",       "thorned vines make a rustling noise as they quiver restlessly."},
    {GRASS_CHAR,	&grassColor,			0,						60,	15,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_STAND_IN_TILE),                                                               "a tuft of grass",      "tufts of lush grass have improbably pushed upward through the stone ground."},

    // Yendor amulet floor tile
  	{FLOOR_CHAR,	&floorForeColor,		&floorBackColor,		95,	0,	DF_PLAIN_FIRE,	0,			0,              0,				NO_LIGHT,       0, (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_PROMOTES_ON_ITEM_PICKUP),                         "the ground",           ""},

    // commutation device
	{ALTAR_CHAR,	&altarForeColor,		&greenAltarBackColor,   17, 0,	0,				0,			DF_ALTAR_COMMUTE,0,             NO_LIGHT,       (T_OBSTRUCTS_SURFACE_EFFECTS), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_SWAP_ENCHANTS_ACTIVATION | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),	"a commutation altar",	"crude diagrams on this altar and its twin invite you to place items upon them."},
    {ALTAR_CHAR,	&black,                 &greenAltarBackColor,   17, 0,	0,				0,			0,				0,				NO_LIGHT,       (T_OBSTRUCTS_SURFACE_EFFECTS), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),							"a scorched altar",     "scorch marks cover the surface of the altar, but it is cold to the touch."},
    {'+',           &veryDarkGray,          0,                      45, 0,  DF_PLAIN_FIRE,  0,          DF_INERT_PIPE,  0,              CONFUSION_GAS_LIGHT, (0), (TM_IS_WIRED | TM_VANISHES_UPON_PROMOTION),                                               "glowing glass pipes",  "glass pipes are set into the floor and emit a soft glow of shifting color."},
    {'+',           &black,                 0,                      45, 0,  DF_PLAIN_FIRE,  0,          0,              0,              NO_LIGHT,       (0), (0),                                                                                           "charred glass pipes",  "the inside of the glass pipes are charred."},

    // resurrection altar
	{ALTAR_CHAR,	&altarForeColor,		&goldAltarBackColor,    17, 0,	0,				0,			DF_ALTAR_RESURRECT,0,           CANDLE_LIGHT,   (T_OBSTRUCTS_SURFACE_EFFECTS), (TM_IS_WIRED | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),           "a resurrection altar",	"the souls of the dead surround you. A deceased ally might be called back."},
    {ALTAR_CHAR,	&black,                 &goldAltarBackColor,    16, 0,	0,				0,			0,				0,				NO_LIGHT,       (T_OBSTRUCTS_SURFACE_EFFECTS), (TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT),							"a scorched altar",     "scorch marks cover the surface of the altar, but it is cold to the touch."},
    {0,             0,                      0,                      95,	0,	0,              0,			0,				0,				NO_LIGHT,		(0), (TM_IS_WIRED | TM_PROMOTES_ON_PLAYER_ENTRY),                                                   "the ground",			""},

    // doorway statues
    {STATUE_CHAR,	&wallBackColor,			&statueBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_CONNECTS_LEVEL),	"a marble statue",	"The cold marble statue has weathered the years with grace."},
	{STATUE_CHAR,	&wallBackColor,			&statueBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			DF_CRACKING_STATUE,0,			NO_LIGHT,		(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_CONNECTS_LEVEL),"a marble statue",	"The cold marble statue has weathered the years with grace."},

    // extensible stone bridge
	{CHASM_CHAR,	&chasmForeColor,		&black,					40,	0,	DF_PLAIN_FIRE,	0,			0,              0,              NO_LIGHT,		(T_AUTO_DESCENT), (TM_STAND_IN_TILE),                                                               "a chasm",				"you plunge downward into the chasm!"},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	40,	0,	DF_PLAIN_FIRE,	0,			DF_BRIDGE_ACTIVATE,6000,        NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION),                                                                  "a stone bridge",		"the narrow stone bridge is extending across the chasm."},
	{FLOOR_CHAR,	&white,					&chasmEdgeBackColor,	80,	0,	DF_PLAIN_FIRE,	0,			DF_BRIDGE_ACTIVATE_ANNOUNCE,0,	NO_LIGHT,		(0), (TM_IS_WIRED),                                                                                 "the brink of a chasm",	"chilly winds blow upward from the stygian depths."},

    // rat trap
    {WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_WALL_CRACK,  0,              NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),			"a stone wall",			"The rough stone wall is firm and unyielding."},
    {WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_WALL_SHATTER,500,			NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_LIST_IN_SIDEBAR),     "a cracking wall",		"Cracks are running ominously across the base of this rough stone wall."},

    // electric crystals
    {ELECTRIC_CRYSTAL_CHAR, &wallCrystalColor, &graniteBackColor,   0,  0,  DF_PLAIN_FIRE,  0,          DF_ELECTRIC_CRYSTAL_ON, 0,      NO_LIGHT,       (T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_PROMOTES_ON_ELECTRICITY | TM_IS_CIRCUIT_BREAKER | TM_IS_WIRED | TM_LIST_IN_SIDEBAR), "a darkened crystal globe", "A slight electric shock startles you when you touch the inert crystal globe."},
    {ELECTRIC_CRYSTAL_CHAR, &white,         &wallCrystalColor,      0,  0,  DF_PLAIN_FIRE,  0,          0,              0,              CRYSTAL_WALL_LIGHT,(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_GAS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR), "a shining crystal globe", "Crackling light streams out of the crystal globe."},
    {LEVER_CHAR,	&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_TURRET_LEVER,0,              NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_PLAYER_ENTRY | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_INVERT_WHEN_HIGHLIGHTED),"a lever", "The lever moves."},

    // worm tunnels
	{0,				0,						0,						100,0,	0,				0,			DF_WORM_TUNNEL_MARKER_ACTIVE,0, NO_LIGHT,       (0), (TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED),                                                    "",                     ""},
	{0,				0,						0,						100,0,	0,				0,			DF_GRANITE_CRUMBLES,-2000,		NO_LIGHT,		(0), (TM_VANISHES_UPON_PROMOTION),                                                                  "a rough granite wall",	"The granite is split open with splinters of rock jutting out at odd angles."},
	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			DF_WALL_SHATTER,0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_WIRED | TM_CONNECTS_LEVEL),"a stone wall", "The rough stone wall is firm and unyielding."},

    // zombie crypt
	{FIRE_CHAR,		&fireForeColor,			&statueBackColor,       0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				BURNING_CREATURE_LIGHT,	(T_OBSTRUCTS_PASSABILITY | T_OBSTRUCTS_ITEMS | T_IS_FIRE), (TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR),"a ceremonial brazier",		"The ancient brazier smolders with a deep crimson flame."},

    // goblin warren
    {FLOOR_CHAR,    &mudBackColor,          &refuseBackColor,       85,	0,	DF_STENCH_SMOLDER,0,		0,				0,				NO_LIGHT,		(T_IS_FLAMMABLE), (TM_VANISHES_UPON_PROMOTION),                                                     "the mud floor",		"Rotting animal matter has been ground into the mud floor; the stench is awful."},
    {WALL_CHAR,		&mudWallForeColor,		&mudWallBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a mud-covered wall",	"A malodorous layer of clay and fecal matter coats the wall."},
    {OMEGA_CHAR,	&mudWallForeColor,      &refuseBackColor,		25,	50,	DF_EMBERS,		0,			0,              0,				NO_LIGHT,		(T_OBSTRUCTS_VISION | T_OBSTRUCTS_GAS | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VISUALLY_DISTINCT), "hanging animal skins",	"you push through the animal skins that hang across the threshold."},

    // new stuff -- gsr
    {FLOOR_CHAR,    &wallBackColor,          &graniteBackColor,       85,	0,	0,0,		0,				0,				NO_LIGHT,		0, 0,                                                     "the hideout floor",		""},
	{WALL_CHAR,		&darkGray,			&wallBackColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a gritty stone wall",	"Chunks of rock have been haphazardly chipped away."},
    {OMEGA_CHAR,	&wallBackColor,      &graniteBackColor,		25,	50,	DF_EMBERS,		0,			0,              0,				NO_LIGHT,		(T_OBSTRUCTS_VISION | T_OBSTRUCTS_GAS), (TM_STAND_IN_TILE | TM_VISUALLY_DISTINCT), "a bead-adorned archway",	"you push through strings of stone beads hanging from above."},
	{ALTAR_CHAR,	&doorForeColor,		&doorBackColor,		0, 0,	0,				0,			0,				0,				NO_LIGHT,	(T_OBSTRUCTS_SURFACE_EFFECTS), (TM_VISUALLY_DISTINCT),							"a wooden table",	"an uninviting, ill-crafted table rests here."},

	// corpse! -- gsr
	{CORPSE_CHAR,	&darkRed,			0,						    20,	0,	0,	            0,			0,			    500,			0,	            0,    (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION),                                               "a mangled corpse",	"an unrecognizable, mangled corpse rests here."},

//    {CORPSE_CHAR,   &white,          0,       85,	0,	0,0,		0,				0,				NO_LIGHT,		0, 0,                                                     "a mangled corpse",		""},

	// hell's decor
    {OMEGA_CHAR,	&lavaLightColor,		&firstStairsBackColor,	30,	0,	DF_PLAIN_FIRE,	0,			DF_REPEL_CREATURES, 0,			PHOENIX_LIGHT,		(T_OBSTRUCTS_ITEMS | T_OBSTRUCTS_SURFACE_EFFECTS), (TM_PROMOTES_ON_STEP | TM_STAND_IN_TILE | TM_LIST_IN_SIDEBAR | TM_VISUALLY_DISTINCT | TM_BRIGHT_MEMORY | TM_INTERRUPT_EXPLORATION_WHEN_SEEN | TM_INVERT_WHEN_HIGHLIGHTED), "a foreboding archway",		"Bright lights of all sorts of shades of red swirl around inside this archway."},
    {WALL_CHAR,		&netherWallForeColor,   &netherWallBackColor,			0,	0,	DF_PLAIN_FIRE,	0,			0,				0,		NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a magmatite wall",			"The rough magmatite wall is firm and warm to the touch."},
    {WALL_CHAR,		&wallBackColor,			&lavaLightColor,		0,	0,	DF_PLAIN_FIRE,	0,			0,				0,				PHOENIX_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a rough magma wall",	"Magmatite has split open, revealing semi-solid magma."},

    // unused for now
    {OMEGA_CHAR,	&doorForeColor,			&netherWallBackColor,	25,	50,	0,		DF_SHOW_DOOR,			DF_RUBBLE,	0,				NO_LIGHT,		(T_OBSTRUCTS_VISION | T_OBSTRUCTS_GAS), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP | TM_VISUALLY_DISTINCT), "a pile of rubble",	"you push your way through the loose rock."},
//	{WALL_CHAR,		&netherWallForeColor,   &netherWallBackColor,	0,	0,	0,	0,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET),	"a magnetite wall",		"The rough magmatite wall is firm and warm to the touch."},
    {WALL_CHAR,		&netherWallForeColor,   &netherWallBackColor,	0,	0,	0,	DF_SHOW_DOOR,			0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING), (TM_STAND_IN_TILE),														"a magmatite wall",			"The rough magmatite wall is firm and warm to the touch."},

    // For water elementals!
	//	char		fore color				back color		priority	ignit	fireType	discovType	promoteType		promoteChance	glowLight		flags																								description			flavorText

	{0,				&shallowWaterForeColor,	&shallowWaterBackColor,	50,	200,	DF_STEAM_ACCUMULATION,	0,	0,		2000,			NO_LIGHT,		(0), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_EXTINGUISHES_FIRE | TM_ALLOWS_SUBMERGING),	"shallow water",		"knee-deep water sloshes around very subtly."},

//	{FIRE_CHAR,		&fireForeColor,			0,						10,	0,	    0,				        0,	DF_EMBERS,		500,			FIRE_LIGHT,		(T_IS_FIRE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_VISUALLY_DISTINCT),				"billowing flames",		"flames billow upward."},


//	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	50,	DF_EMBERS,		DF_SHOW_DOOR,0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET),	"a stone wall",		"The rough stone wall is firm and unyielding."},

//	{WALL_CHAR,		&wallForeColor,			&wallBackColor,			0,	50,	DF_EMBERS,		DF_SHOW_DOOR,0,				0,				NO_LIGHT,		(T_OBSTRUCTS_EVERYTHING | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_IS_SECRET),	"a stone wall",		"The rough stone wall is firm and unyielding."},


//	{FOLIAGE_CHAR,	&deadFoliageColor,		0,						45,	80,	DF_PLAIN_FIRE,	0,			DF_SMALL_DEAD_GRASS, 0,			NO_LIGHT,		(T_OBSTRUCTS_VISION | T_IS_FLAMMABLE), (TM_STAND_IN_TILE | TM_VANISHES_UPON_PROMOTION | TM_PROMOTES_ON_STEP), "dead foliage",    "the decaying husk of a fungal growth fills the area."},


};

// Dungeon Feature definitions

// Features in the gas layer use the startprob as volume, ignore probdecr, and spawn in only a single point.
// Intercepts and slopes are in units of 0.01.
dungeonFeature dungeonFeatureCatalog[NUMBER_DUNGEON_FEATURES] = {
	// tileType					layer		start	decr	fl	txt  flare   fCol fRad	propTerrain	subseqDF
	{0}, // nothing
	{GRANITE,					DUNGEON,	80,		70,		DFF_CLEAR_OTHER_TERRAIN},
	{CRYSTAL_WALL,				DUNGEON,	200,	50,		DFF_CLEAR_OTHER_TERRAIN},
	{LUMINESCENT_FUNGUS,		SURFACE,	60,		8,		DFF_BLOCKED_BY_OTHER_LAYERS},
	{GRASS,						SURFACE,	75,		5,		DFF_BLOCKED_BY_OTHER_LAYERS},
	{DEAD_GRASS,				SURFACE,	75,		5,		DFF_BLOCKED_BY_OTHER_LAYERS,	"", 0,	0,	0,		0,			DF_DEAD_FOLIAGE},
	{BONES,						SURFACE,	75,		23,		0},
	{RUBBLE,					SURFACE,	45,		23,		0},
	{FOLIAGE,					SURFACE,	100,	33,		(DFF_BLOCKED_BY_OTHER_LAYERS)},
	{FUNGUS_FOREST,				SURFACE,	100,	45,		(DFF_BLOCKED_BY_OTHER_LAYERS)},
	{DEAD_FOLIAGE,				SURFACE,	50,		30,		(DFF_BLOCKED_BY_OTHER_LAYERS)},

	// misc. liquids
	{SUNLIGHT_POOL,				LIQUID,		65,		6,		0},
	{DARKNESS_PATCH,			LIQUID,		65,		11,		0},

	// Dungeon features spawned during gameplay:

	// revealed secrets
	{DOOR,						DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{GAS_TRAP_POISON,			DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{GAS_TRAP_PARALYSIS,		DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{CHASM_EDGE,				LIQUID,		100,	100,	0, "", GENERIC_FLASH_LIGHT},
	{TRAP_DOOR,					LIQUID,		0,		0,		DFF_CLEAR_OTHER_TERRAIN, "", GENERIC_FLASH_LIGHT, 0, 0, 0, DF_SHOW_TRAPDOOR_HALO},
	{GAS_TRAP_CONFUSION,		DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{FLAMETHROWER,				DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{FLOOD_TRAP,				DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{NET_TRAP,                  DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},
	{ALARM_TRAP,                DUNGEON,	0,		0,		0, "", GENERIC_FLASH_LIGHT},

	// bloods
	// Start probability is actually a percentage for bloods.
	// Base probability is 15 + (damage * 2/3), and then take the given percentage of that.
	// If it's a gas, we multiply the base by an additional 100.
	// Thus to get a starting gas volume of a poison potion (1000), with a hit for 10 damage, use a starting probability of 48.
	{RED_BLOOD,					SURFACE,	100,	25,		0},
	{GREEN_BLOOD,				SURFACE,	100,	25,		0},
	{PURPLE_BLOOD,				SURFACE,	100,	25,		0},
	{WORM_BLOOD,				SURFACE,	100,	25,		0},
	{ACID_SPLATTER,				SURFACE,	200,	25,		0},
	{ASH,						SURFACE,	50,		25,		0},
	{EMBERS,					SURFACE,	125,	25,		0},
	{ECTOPLASM,					SURFACE,	110,	25,		0},
	{RUBBLE,					SURFACE,	33,		25,		0},
	{ROT_GAS,					GAS,		12,		0,		0},


	// monster effects
	{VOMIT,						SURFACE,	30,		10,		0},
	{POISON_GAS,				GAS,		2000,	0,		0},
    {GAS_EXPLOSION,				SURFACE,	350,	100,	0,	"", EXPLOSION_FLARE_LIGHT},
	{RED_BLOOD,					SURFACE,	150,	30,		0},
	{FLAMEDANCER_FIRE,			SURFACE,	200,	75,		0},

    // corpse! -- gsr
	{MONSTER_CORPSE,			DUNGEON,	0,		0,		0,	"", 0,	0,	0,		0,			0},

     // mutation effects
    {GAS_EXPLOSION,				SURFACE,	350,	100,	0,	"The corpse detonates with terrifying force!", EXPLOSION_FLARE_LIGHT},
	{LICHEN,					SURFACE,	70,		60,		0,  "Poisonous spores burst from the corpse!"},

	// misc
	{NOTHING,					GAS,		0,		0,		DFF_EVACUATE_CREATURES_FIRST},
	{ROT_GAS,					GAS,		15,		0,		0},
	{STEAM,						GAS,		325,	0,		0},
	{STEAM,						GAS,		15,		0,		0},
	{METHANE_GAS,				GAS,		2,		0,		0},
	{EMBERS,					SURFACE,	0,		0,		0},
	{URINE,						SURFACE,	65,		25,		0},
	{UNICORN_POOP,				SURFACE,	65,		40,		0},
	{PUDDLE,					SURFACE,	13,		25,		0},
	{ASH,						SURFACE,	0,		0,		0},
	{ECTOPLASM,					SURFACE,	0,		0,		0},
	{FORCEFIELD,				SURFACE,	100,	50,		0},
    {FORCEFIELD_MELT,           SURFACE,    0,      0,      0},
    {SACRED_GLYPH,              LIQUID,     100,    100,    0,  "", EMPOWERMENT_LIGHT},
	{LICHEN,					SURFACE,	2,		100,	(DFF_BLOCKED_BY_OTHER_LAYERS)}, // Lichen won't spread through lava.
	{RUBBLE,					SURFACE,	45,		23,		(DFF_ACTIVATE_DORMANT_MONSTER)},
	{RUBBLE,					SURFACE,	0,		0,		(DFF_ACTIVATE_DORMANT_MONSTER)},

    {SPIDERWEB,                 SURFACE,    15,     12,     0},
    {SPIDERWEB,                 SURFACE,    100,    39,     0},

    {ANCIENT_SPIRIT_VINES,      SURFACE,    75,     70,     0},
    {ANCIENT_SPIRIT_GRASS,      SURFACE,    50,     47,     0},

	// foliage
	{TRAMPLED_FOLIAGE,			SURFACE,	0,		0,		0},
	{DEAD_GRASS,				SURFACE,	75,		75,		0},
	{FOLIAGE,					SURFACE,	0,		0,		(DFF_BLOCKED_BY_OTHER_LAYERS)},
	{TRAMPLED_FUNGUS_FOREST,	SURFACE,	0,		0,		0},
	{FUNGUS_FOREST,				SURFACE,	0,		0,		(DFF_BLOCKED_BY_OTHER_LAYERS)},

	// brimstone
	{ACTIVE_BRIMSTONE,			LIQUID,		0,		0,		0},
	{INERT_BRIMSTONE,			LIQUID,		0,		0,		0,	"", 0,	0,	0,		0,			DF_BRIMSTONE_FIRE},

    // bloodwort
    {BLOODFLOWER_POD,           SURFACE,    60,     60,     DFF_EVACUATE_CREATURES_FIRST},
    {BLOODFLOWER_POD,           SURFACE,    10,     10,     DFF_EVACUATE_CREATURES_FIRST},
	{HEALING_CLOUD,				GAS,		350,	0,		0},

    // dewars
    {POISON_GAS,                GAS,        20000,  0,      0, "the dewar shatters and pressurized caustic gas explodes outward!", 0, &poisonGasColor, 4, 0, DF_DEWAR_GLASS},
    {CONFUSION_GAS,             GAS,        20000,  0,      0, "the dewar shatters and pressurized confusion gas explodes outward!", 0, &confusionGasColor, 4, 0, DF_DEWAR_GLASS},
    {PARALYSIS_GAS,             GAS,        20000,  0,      0, "the dewar shatters and pressurized paralytic gas explodes outward!", 0, &pink, 4, 0, DF_DEWAR_GLASS},
    {METHANE_GAS,               GAS,        20000,  0,      0, "the dewar shatters and pressurized methane gas explodes outward!", 0, &methaneColor, 4, 0, DF_DEWAR_GLASS},
    {BROKEN_GLASS,              SURFACE,    100,    70,     0},
    {CARPET,                    DUNGEON,    120,    20,     0},

    // algae
    {DEEP_WATER_ALGAE_WELL,     DUNGEON,    0,      0,      DFF_SUPERPRIORITY},
    {DEEP_WATER_ALGAE_1,		LIQUID,		50,		100,	0,  "", 0,  0,   0,     DEEP_WATER, DF_ALGAE_2},
    {DEEP_WATER_ALGAE_2,        LIQUID,     0,      0,      0},
    {DEEP_WATER,                LIQUID,     0,      0,      DFF_SUPERPRIORITY},

	// doors, item cages, altars, glyphs, guardians -- reusable machine components
	{OPEN_DOOR,					DUNGEON,	0,		0,		0},
	{DOOR,						DUNGEON,	0,		0,		0},
	{OPEN_IRON_DOOR_INERT,		DUNGEON,	0,		0,		0,  "", GENERIC_FLASH_LIGHT},
	{ALTAR_CAGE_OPEN,			DUNGEON,	0,		0,		0,	"the cages lift off of the altars as you approach.", GENERIC_FLASH_LIGHT},
	{ALTAR_CAGE_CLOSED,			DUNGEON,	0,		0,		(DFF_EVACUATE_CREATURES_FIRST), "the cages lower to cover the altars.", GENERIC_FLASH_LIGHT},
	{ALTAR_INERT,				DUNGEON,	0,		0,		0},
	{FLOOR_FLOODABLE,			DUNGEON,	0,		0,		0,	"the altar retracts into the ground with a grinding sound.", GENERIC_FLASH_LIGHT},
	{PORTAL_LIGHT,				SURFACE,	0,		0,		(DFF_EVACUATE_CREATURES_FIRST | DFF_ACTIVATE_DORMANT_MONSTER), "the archway flashes, and you catch a glimpse of another world!"},
    {MACHINE_GLYPH_INACTIVE,    DUNGEON,    0,      0,      0},
    {MACHINE_GLYPH,             DUNGEON,    0,      0,      0},
    {GUARDIAN_GLOW,             SURFACE,    0,      0,      0,  ""},
    {GUARDIAN_GLOW,             SURFACE,    0,      0,      0,  "the glyph beneath you glows, and the guardians take a step!"},
    {GUARDIAN_GLOW,             SURFACE,    0,      0,      0,  "the mirrored totem flashes, reflecting the red glow of the glyph beneath you."},
    {MACHINE_GLYPH,             DUNGEON,    200,    95,     DFF_BLOCKED_BY_OTHER_LAYERS},
    {WALL_LEVER,                DUNGEON,    0,      0,      0,  "you notice a lever hidden behind a loose stone in the wall.", GENERIC_FLASH_LIGHT},
    {WALL_LEVER_PULLED,         DUNGEON,    0,      0,      0},
    {WALL_LEVER_HIDDEN,         DUNGEON,    0,      0,      0},

    {BRIDGE_FALLING,            LIQUID,     200,    100,    0, "", 0, 0, 0, BRIDGE},
    {CHASM,                     LIQUID,     0,      0,      0, "", GENERIC_FLASH_LIGHT, 0, 0, 0, DF_BRIDGE_FALL_PREP},

	// fire
	{PLAIN_FIRE,				SURFACE,	0,		0,		0},
	{GAS_FIRE,					SURFACE,	0,		0,		0},
	{GAS_EXPLOSION,				SURFACE,	60,		17,		0},
	{DART_EXPLOSION,			SURFACE,	0,		0,		0},
	{BRIMSTONE_FIRE,			SURFACE,	0,		0,		0},
	{0,                         0,          0,		0,		0,	"the rope bridge snaps from the heat and plunges into the chasm!", FALLEN_TORCH_FLASH_LIGHT,	0,	0,		0,		DF_BRIDGE_FALL},
	{PLAIN_FIRE,				SURFACE,	100,	37,		0},
	{EMBERS,					SURFACE,	0,		0,		0},
	{EMBERS,					SURFACE,	100,	94,		0},
	{OBSIDIAN,					SURFACE,	0,		0,		0},
    {ITEM_FIRE,                 SURFACE,    0,      0,      0,  "", FALLEN_TORCH_FLASH_LIGHT},
    {CREATURE_FIRE,             SURFACE,    0,      0,      0,  "", FALLEN_TORCH_FLASH_LIGHT},

	{FLOOD_WATER_SHALLOW,		SURFACE,	225,	37,		0,	"", 0,	0,	0,		0,			DF_FLOOD_2},
	{FLOOD_WATER_DEEP,			SURFACE,	175,	37,		0,	"the area becomes flooded!"},
	{FLOOD_WATER_SHALLOW,		SURFACE,	10,		25,		0},
	{HOLE,						SURFACE,	200,	100,	0},
	{HOLE_EDGE,					SURFACE,	0,		0,		0},

	// gas trap effects
	{POISON_GAS,				GAS,		1000,	0,		0,	"a cloud of caustic gas sprays upward from the floor!"},
//	{CONFUSION_GAS,				GAS,		300,	0,		0,	"a sparkling cloud of confusion gas sprays upward from the floor!"},
	{STENCH_SMOKE_GAS,			GAS,		300,	0,		0,	"a sparkling cloud of nauseating gas sprays upward from the floor!"},
    {NETTING,                   SURFACE,    300,    90,     0,  "a net falls from the ceiling!"},
    {0,                         0,          0,      0,      DFF_AGGRAVATES_MONSTERS, "a piercing shriek echoes through the nearby rooms!", 0, 0, DCOLS/2},
	{METHANE_GAS,				GAS,		10000,	0,		0}, // debugging toy
	{NETTING,                   SURFACE,    300,    90,     0,  ""},


	// potions
	{POISON_GAS,				GAS,		1000,	0,		0,	"", 0,	&poisonGasColor,4},
	{PARALYSIS_GAS,				GAS,		1000,	0,		0,	"", 0,	&pink,4},
	{CONFUSION_GAS,				GAS,		1000,	0,		0,	"", 0,	&confusionGasColor, 4},
	{PLAIN_FIRE,				SURFACE,	100,	37,		0,	"", EXPLOSION_FLARE_LIGHT},
	{DARKNESS_CLOUD,			GAS,		200,	0,		0},
	{HOLE_EDGE,					SURFACE,	300,	100,	0,	"", 0,	&darkBlue,3,0,			DF_HOLE_2},
	{LICHEN,					SURFACE,	70,		60,		0},

    // other items
    {PLAIN_FIRE,				SURFACE,	100,	45,		0,	"", 0,	&yellow,3},
	{HOLE_GLOW,					SURFACE,	200,	100,	DFF_SUBSEQ_EVERYWHERE,	"", 0,	&darkBlue,3,0,			DF_STAFF_HOLE_EDGE},
    {HOLE_EDGE,					SURFACE,	100,	100,	0},

	// machine components

    // commutation altars
    {COMMUTATION_ALTAR_INERT,   DUNGEON,    0,      0,      0,  "the items on the two altars flash with a brilliant light!", SCROLL_ENCHANTMENT_LIGHT},
    {PIPE_GLOWING,              SURFACE,    90,     60,     0},
    {PIPE_INERT,                SURFACE,    0,      0,      0,  "", SCROLL_ENCHANTMENT_LIGHT},

    // resurrection altars
    {RESURRECTION_ALTAR_INERT,  DUNGEON,    0,      0,      DFF_RESURRECT_ALLY, "An old friend emerges from a bloom of sacred light!", EMPOWERMENT_LIGHT},
    {MACHINE_TRIGGER_FLOOR_REPEATING, LIQUID, 300,  100,    DFF_SUPERPRIORITY},

	// coffin bursts open to reveal vampire:
	{COFFIN_OPEN,				DUNGEON,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"the coffin opens and a dark figure rises!", 0, &darkGray, 3},
	{PLAIN_FIRE,				SURFACE,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"as flames begin to lick the coffin, its tenant bursts forth!", 0, 0, 0, 0, DF_EMBERS_PATCH},
	{MACHINE_TRIGGER_FLOOR,		DUNGEON,	200,	100,	0},

	// throwing tutorial:
	{ALTAR_INERT,				DUNGEON,	0,		0,		0,	"the cage lifts off of the altar.", GENERIC_FLASH_LIGHT},
	{TRAP_DOOR,					LIQUID,		225,	100,	(DFF_CLEAR_OTHER_TERRAIN | DFF_SUBSEQ_EVERYWHERE), "", 0, 0, 0, 0, DF_SHOW_TRAPDOOR_HALO},
	{LAVA,						LIQUID,		225,	100,	(DFF_CLEAR_OTHER_TERRAIN)},
    {MACHINE_PRESSURE_PLATE_USED,DUNGEON,   0,      0,      0},

    // rat trap:
    {RAT_TRAP_WALL_CRACKING,    DUNGEON,    0,      0,      0,  "a scratching sound emanates from the nearby walls!", 0, 0, 0, 0, DF_RUBBLE},

	// wooden barricade at entrance:
	{PLAIN_FIRE,				SURFACE,	0,		0,		0,	"flames quickly consume the wooden barricade."},

	// wooden barricade around altar:
	{WOODEN_BARRICADE,          DUNGEON,	220,	100,	(DFF_TREAT_AS_BLOCKING | DFF_SUBSEQ_EVERYWHERE), "", 0, 0, 0, 0, DF_SMALL_DEAD_GRASS},

	// shallow water flood machine:
	{MACHINE_FLOOD_WATER_SPREADING,	LIQUID,	0,		0,		0,	"you hear a heavy click, and the nearby water begins flooding the area!"},
	{SHALLOW_WATER,				LIQUID,		0,		0,		0},
	{MACHINE_FLOOD_WATER_SPREADING,LIQUID,	100,	100,	0,	"", 0,	0,	0,		FLOOR_FLOODABLE,			DF_SHALLOW_WATER},
	{MACHINE_FLOOD_WATER_DORMANT,LIQUID,	250,	100,	(DFF_TREAT_AS_BLOCKING), "", 0, 0, 0, 0,            DF_SPREADABLE_DEEP_WATER_POOL},
	{DEEP_WATER,				LIQUID,		90,		100,	(DFF_CLEAR_OTHER_TERRAIN | DFF_PERMIT_BLOCKING)},

	// unstable floor machine:
	{MACHINE_COLLAPSE_EDGE_SPREADING,LIQUID,0,		0,		0,	"you hear a deep rumbling noise as the floor begins to collapse!"},
	{CHASM,						LIQUID,		0,		0,		DFF_CLEAR_OTHER_TERRAIN, "", 0, 0, 0, 0, DF_SHOW_TRAPDOOR_HALO},
	{MACHINE_COLLAPSE_EDGE_SPREADING,LIQUID,100,	100,	0,	"", 0,	0,	0,	FLOOR_FLOODABLE,	DF_COLLAPSE},
	{MACHINE_COLLAPSE_EDGE_DORMANT,LIQUID,	0,		0,		0},

	// levitation bridge machine:
    {CHASM_WITH_HIDDEN_BRIDGE_ACTIVE,LIQUID,100,    100,    0,  "", 0, 0,  0,  CHASM_WITH_HIDDEN_BRIDGE,  DF_BRIDGE_APPEARS},
    {CHASM_WITH_HIDDEN_BRIDGE_ACTIVE,LIQUID,100,    100,    0,  "a stone bridge extends from the floor with a grinding sound.", 0, 0,  0,  CHASM_WITH_HIDDEN_BRIDGE,  DF_BRIDGE_APPEARS},
	{STONE_BRIDGE,				LIQUID,		0,		0,		0},
	{MACHINE_CHASM_EDGE,        LIQUID,     100,	100,	0},

	// retracting lava pool:
    {LAVA_RETRACTABLE,          LIQUID,     100,    100,    0,  "", 0, 0,  0,  LAVA},
	{LAVA_RETRACTING,			LIQUID,		0,		0,		0,	"hissing fills the air as the lava begins to cool."},
	{OBSIDIAN,					SURFACE,	0,		0,		0,	"", 0,	0,	0,		0,			DF_STEAM_ACCUMULATION},

	// hidden poison vent machine:
	{MACHINE_POISON_GAS_VENT_DORMANT,DUNGEON,0,		0,		0,	"you notice an inactive gas vent hidden in a crevice of the floor.", GENERIC_FLASH_LIGHT},
	{MACHINE_POISON_GAS_VENT,	DUNGEON,	0,		0,		0,	"deadly purple gas starts wafting out of hidden vents in the floor!"},
	{PORTCULLIS_CLOSED,			DUNGEON,	0,		0,		DFF_EVACUATE_CREATURES_FIRST,	"with a heavy mechanical sound, an iron portcullis falls from the ceiling!", GENERIC_FLASH_LIGHT},
	{PORTCULLIS_DORMANT,		DUNGEON,	0,		0,		0,  "the portcullis slowly rises from the ground into a slot in the ceiling.", GENERIC_FLASH_LIGHT},
	{POISON_GAS,				GAS,		25,		0,		0},

	// hidden methane vent machine:
	{MACHINE_METHANE_VENT_DORMANT,DUNGEON,0,		0,		0,	"you notice an inactive gas vent hidden in a crevice of the floor.", GENERIC_FLASH_LIGHT},
	{MACHINE_METHANE_VENT,		DUNGEON,	0,		0,		0,	"explosive methane gas starts wafting out of hidden vents in the floor!", 0, 0, 0, 0, DF_VENT_SPEW_METHANE},
	{METHANE_GAS,				GAS,		60,		0,		0},
	{PILOT_LIGHT,				DUNGEON,	0,		0,		0,	"a torch falls from its mount and lies sputtering on the floor.", FALLEN_TORCH_FLASH_LIGHT},

    // paralysis trap:
	{MACHINE_PARALYSIS_VENT,    DUNGEON,    0,		0,		0,	"you notice an inactive gas vent hidden in a crevice of the floor.", GENERIC_FLASH_LIGHT},
	{PARALYSIS_GAS,				GAS,		350,	0,		0,	"paralytic gas sprays upward from hidden vents in the floor!", 0, 0, 0, 0, DF_REVEAL_PARALYSIS_VENT_SILENTLY},
	{MACHINE_PARALYSIS_VENT,    DUNGEON,    0,		0,		0},

    // thematic dungeon:
    {RED_BLOOD,					SURFACE,	75,     25,		0},

	// statuary:
	{STATUE_CRACKING,			DUNGEON,	0,		0,		0,	"cracks begin snaking across the marble surface of the statue!", 0, 0, 0, 0, DF_RUBBLE},
	{RUBBLE,					SURFACE,	120,	100,	DFF_ACTIVATE_DORMANT_MONSTER,	"the statue shatters!", 0, &darkGray, 3, 0, DF_RUBBLE},

	// hidden turrets:
	{WALL,                      DUNGEON,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"you hear a click, and the stones in the wall shift to reveal turrets!", 0, 0, 0, 0, DF_RUBBLE},

    // worm tunnels:
    {WORM_TUNNEL_MARKER_DORMANT,LIQUID,     5,      5,      0,  "", 0,  0,  GRANITE},
    {WORM_TUNNEL_MARKER_ACTIVE, LIQUID,     0,      0,      0},
    {FLOOR,                     DUNGEON,    0,      0,      (DFF_SUPERPRIORITY | DFF_ACTIVATE_DORMANT_MONSTER),  "", 0, 0,  0,  0,  DF_TUNNELIZE},
    {FLOOR,                     DUNGEON,    0,      0,      0,  "the nearby wall cracks and collapses in a cloud of dust!", 0, &darkGray,  5,  0,  DF_TUNNELIZE},

	// haunted room:
	{DARK_FLOOR_DARKENING,		DUNGEON,	0,		0,		0,	"the light in the room flickers and you feel a chill in the air."},
	{DARK_FLOOR,				DUNGEON,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"", 0, 0, 0, 0, DF_ECTOPLASM_DROPLET},
    {HAUNTED_TORCH_TRANSITIONING,DUNGEON,   0,      0,      0},
    {HAUNTED_TORCH,             DUNGEON,    0,      0,      0},

	// mud pit:
	{MACHINE_MUD_DORMANT,		LIQUID,		100,	100,	0},
	{MUD,						LIQUID,		0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"across the bog, bubbles rise ominously from the mud."},

    // electric crystals:
    {ELECTRIC_CRYSTAL_ON,       DUNGEON,    0,      0,      0, "the crystal absorbs the electricity and begins to glow.", CHARGE_FLASH_LIGHT},
	{WALL,                      DUNGEON,	0,		0,		DFF_ACTIVATE_DORMANT_MONSTER,	"the wall above the lever shifts to reveal a spark turret!"},

	// idyll:
	{SHALLOW_WATER,				LIQUID,		150,	100,	(DFF_PERMIT_BLOCKING)},
	{DEEP_WATER,				LIQUID,		90,		100,	(DFF_TREAT_AS_BLOCKING | DFF_CLEAR_OTHER_TERRAIN | DFF_SUBSEQ_EVERYWHERE), "", 0, 0, 0, 0, DF_SHALLOW_WATER_POOL},

	// swamp:
	{SHALLOW_WATER,				LIQUID,		30,		100,	0},
	{GRAY_FUNGUS,				SURFACE,	80,		50,		0,	"", 0, 0, 0, 0, DF_SWAMP_MUD},
	{MUD,						LIQUID,		75,		5,		0,	"", 0, 0, 0, 0, DF_SWAMP_WATER},

	// camp:
	{HAY,						SURFACE,	90,		87,		0},
	{JUNK,						SURFACE,	20,		20,		0},

	// remnants:
	{CARPET,					DUNGEON,	110,	20,		DFF_SUBSEQ_EVERYWHERE,	"", 0, 0, 0, 0, DF_REMNANT_ASH},
	{BURNED_CARPET,				SURFACE,	120,	100,	0},

	// chasm catwalk:
	{CHASM,						LIQUID,		0,		0,		DFF_CLEAR_OTHER_TERRAIN, "", 0, 0, 0, 0, DF_SHOW_TRAPDOOR_HALO},
	{STONE_BRIDGE,				LIQUID,		0,		0,		DFF_CLEAR_OTHER_TERRAIN},

	// lake catwalk:
	{DEEP_WATER,				LIQUID,		0,		0,		DFF_CLEAR_OTHER_TERRAIN, "", 0, 0, 0, 0, DF_LAKE_HALO},
	{SHALLOW_WATER,				LIQUID,		160,	100,	0},

	// worms pop out of walls:
	{RUBBLE,					SURFACE,	120,	100,	DFF_ACTIVATE_DORMANT_MONSTER,	"the nearby wall explodes in a shower of stone fragments!", 0, &darkGray, 3, 0, DF_RUBBLE},

	// monster cages open:
	{MONSTER_CAGE_OPEN,			DUNGEON,	0,		0,		0},

    // goblin warren:
    {STENCH_SMOKE_GAS,          GAS,		50,		0,		0, "", 0, 0, 0, 0, DF_PLAIN_FIRE},
    {STENCH_SMOKE_GAS,          GAS,		50,		0,		0, "", 0, 0, 0, 0, DF_EMBERS},

    // water elemental
	{ROLLING_WATER,			SURFACE,		300,		90,		0},

//	{ROLLING_WATER,			LIQUID,	90,	100,		0},


};

// Dungeon Profiles

dungeonProfile dungeonProfileCatalog[NUMBER_DUNGEON_PROFILES] = {
    // Room frequencies:
    //      0. Cross room
    //      1. Small symmetrical cross room
    //      2. Small room
    //      3. Circular room
    //      4. Chunky room
    //      5. Cave
    //      6. Cavern (the kind that fills a level)
    //      7. Entrance room (the big upside-down T room at the start of depth 1)

    // Room frequencies
    // 0    1   2   3   4   5   6   7   Corridor chance
    {{2,    1,  1,  1,  7,  1,  0,  0}, 10},    // Basic dungeon generation (further adjusted by depth)
    {{10,   0,  0,  3,  7,  10, 10, 0}, 0},     // First room for basic dungeon generation (further adjusted by depth)

    {{0,    0,  1,  0,  0,  0,  0,  0}, 0},     // Goblin warrens
    {{0,    5,  0,  1,  0,  0,  0,  0}, 0},     // Sentinel sanctuaries
    {{0,    0,  0,  0,  1,  1,  0,  0}, 0},     // Ogre armories
};

// Lights

// radius is in units of 0.01
const lightSource lightCatalog[NUMBER_LIGHT_KINDS] = {
	//color					radius range			fade%	passThroughCreatures
	{0},																// NO_LIGHT
	{&minersLightColor,		{0, 0, 1},				35,		true},		// miners light
	{&fireBoltColor,		{300, 400, 1},			0,		false},		// burning creature light
	{&wispLightColor,		{400, 800, 1},			0,		false},		// will-o'-the-wisp light
	{&fireBoltColor,		{300, 400, 1},			0,		false},		// salamander glow
	{&pink,					{600, 600, 1},			0,		true},		// imp light
	{&pixieColor,			{400, 600, 1},			50,		false},		// pixie light
	{&lichLightColor,		{1500, 1500, 1},		0,		false},		// lich light
	{&flamedancerCoronaColor,{1000, 2000, 1},		0,		false},		// flamedancer light
	{&sentinelLightColor,	{300, 500, 1},			0,		false},		// sentinel light
	{&unicornLightColor,	{300, 400, 1},			0,		false},		// unicorn light
	{&ifritLightColor,		{300, 600, 1},			0,		false},		// ifrit light
	{&fireBoltColor,		{400, 600, 1},			0,		false},		// phoenix light
	{&fireBoltColor,		{150, 300, 1},			0,		false},		// phoenix egg light
	{&yendorLightColor,		{1500, 1500, 1},        0,		false},		// Yendorian light
	{&spectralBladeLightColor,{350, 350, 1},		0,		false},		// spectral blades
	{&summonedImageLightColor,{350, 350, 1},		0,		false},		// weapon images
	{&lightningColor,		{250, 250, 1},			35,		false},		// lightning turret light
    {&explosiveAuraColor,   {150, 200, 1},          0,      true},      // explosive bloat light
	{&lightningColor,		{300, 300, 1},			0,		false},		// bolt glow
	{&telepathyColor,		{200, 200, 1},			0,		true},		// telepathy light
	{&teleportationColor,	{600, 600, 1},			0,		true},		// teleportation/portal light

    // flares:
	{&scrollProtectionColor,{600, 600, 1},			0,		true},		// scroll of protection flare
    {&scrollEnchantmentColor,{600, 600, 1},			0,		true},		// scroll of enchantment flare
    {&potionStrengthColor,  {600, 600, 1},			0,		true},		// potion of strength flare
    {&empowermentFlashColor,{600, 600, 1},			0,		true},		// empowerment flare
    {&genericFlashColor,    {300, 300, 1},			0,		true},		// generic flash flare
	{&fireFlashColor,		{800, 800, 1},			0,		false},		// fallen torch flare
    {&summoningFlashColor,  {600, 600, 1},			0,		true},		// summoning flare
    {&explosionFlareColor,  {5000, 5000, 1},        0,      true},      // explosion (explosive bloat or incineration potion)
	{&quietusFlashColor,	{300, 300, 1},			0,		true},		// quietus activation flare
	{&slayingFlashColor,	{300, 300, 1},			0,		true},		// slaying activation flare
    {&lightningColor,       {800, 800, 1},          0,      false},     // electric crystal activates

	// glowing terrain:
	{&torchLightColor,		{1000, 1000, 1},		50,		false},		// torch
	{&lavaLightColor,		{300, 300, 1},			50,		false},		// lava
	{&sunLightColor,		{200, 200, 1},			25,		true},		// sunlight
	{&darknessPatchColor,	{400, 400, 1},			0,		true},		// darkness patch
	{&fungusLightColor,		{300, 300, 1},			50,		false},		// luminescent fungus
	{&fungusForestLightColor,{500, 500, 1},			0,		false},		// luminescent forest
	{&algaeBlueLightColor,	{300, 300, 1},			0,		false},		// luminescent algae blue
	{&algaeGreenLightColor,	{300, 300, 1},			0,		false},		// luminescent algae green
	{&ectoplasmColor,		{200, 200, 1},			50,		false},		// ectoplasm
	{&unicornLightColor,	{200, 200, 1},			0,		false},		// unicorn poop light
	{&lavaLightColor,		{200, 200, 1},			50,		false},		// embers
	{&lavaLightColor,		{500, 1000, 1},			0,		false},		// fire
	{&lavaLightColor,		{200, 300, 1},			0,		false},		// brimstone fire
	{&explosionColor,		{DCOLS*100,DCOLS*100,1},100,	false},		// explosions
	{&dartFlashColor,		{15*100,15*100,1},		0,		false},		// incendiary darts
	{&portalActivateLightColor,	{DCOLS*100,DCOLS*100,1},0,	false},		// portal activation
	{&confusionLightColor,	{300, 300, 1},			100,	false},		// confusion gas
	{&darknessCloudColor,	{500, 500, 1},			0,		true},		// darkness cloud
	{&forceFieldLightColor,	{200, 200, 1},			50,		false},		// forcefield
	{&crystalWallLightColor,{300, 500, 1},			50,		false},		// crystal wall
	{&torchLightColor,		{200, 400, 1},			0,		false},		// candle light
    {&hauntedTorchColor,	{400, 600, 1},          0,		false},		// haunted torch
    {&glyphLightColor,      {100, 100, 1},          0,      false},     // glyph dim light
    {&glyphLightColor,      {300, 300, 1},          0,      false},     // glyph bright light
    {&sacredGlyphColor,     {300, 300, 1},          0,      false},     // sacred glyph light
    {&descentLightColor,    {600, 600, 1},          0,      false},     // magical pit light

    // other/new
    {&swirlingWindColor,    {300, 300, 1},          0,      true},      // explosive bloat light
};

// Blueprints

const blueprint blueprintCatalog[NUMBER_BLUEPRINTS] = {
	{{0}}, // nothing
	//BLUEPRINTS:
	//depths			roomSize	freq	featureCt   dungeonProfileType  flags	(features on subsequent lines)

		//FEATURES:
		//DF		terrain		layer		instanceCtRange	minInsts	itemCat		itemKind	monsterKind		reqSpace		hordeFl		itemFlags	featureFlags

	// -- REWARD ROOMS --

	// First floor guaranteed library; general store
	{{0, 0},           {10, 50},	10,		6,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE),	{
        {0,         OGRE_DOORWAY,DUNGEON,	{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{0,			CARPET,	DUNGEON,	{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,0,	                        {1,2},		1,			KEY,		KEY_CAGE,	0,				2,				0,			(ITEM_IS_KEY | ITEM_PLAYER_AVOIDS),(MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_KEY_DISPOSABLE | MF_SKELETON_KEY)},
		{0,			MONSTER_CAGE_CLOSED,DUNGEON,	{2,3},		2,			(STAFF|RING|CHARM),-1,	0,				2,				0,			(ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING)},
		{0,			MONSTER_CAGE_CLOSED,DUNGEON,	{0,1},		0,			(WEAPON|ARMOR),-1,	0,				2,				0,			(ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING)},
		{0,			MONSTER_CAGE_CLOSED,DUNGEON,    {1,2},		1,			0,			-1,			0,				1,				(HORDE_MACHINE_KENNEL | HORDE_LEADER_CAPTIVE), 0, (MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING)}
    }},
	// Guaranteed good permanent item on a glowing pedestal
	{{5, AMULET_LEVEL},           {10, 30},	2,		8,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			STATUE_INERT,DUNGEON,		{2,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(WEAPON),	-1,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(ARMOR),	-1,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(STAFF),	-1,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(RING),	    -1,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(CHARM),	-1,			0,				2,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING)},
        {0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)}}},

    // Outsourced item -- same item possibilities as in the good permanent item reward room (plus charms), but directly adopted by 1-2 key machines.
    {{3, AMULET_LEVEL},           {0, 0},     1,		5,			0,                  (BP_REWARD | BP_NO_INTERIOR_FLAG),	{
		{0,			0,			0,				{1,1},		1,			(WEAPON),	-1,			0,				0,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_NO_THROWING_WEAPONS | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_BUILD_ANYWHERE_ON_LEVEL)},
		{0,			0,			0,				{1,1},		1,			(ARMOR),	-1,			0,				0,				0,			ITEM_IDENTIFIED,(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_REQUIRE_GOOD_RUNIC | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_BUILD_ANYWHERE_ON_LEVEL)},
		{0,			0,			0,				{2,2},		2,			(STAFF),	-1,			0,				0,				0,			ITEM_MAX_CHARGES_KNOWN | ITEM_KIND_AUTO_ID, (MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_BUILD_ANYWHERE_ON_LEVEL)},
        {0,			0,			0,				{1,2},		1,			(CHARM),	-1,			0,				0,				0,			ITEM_KIND_AUTO_ID, (MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_BUILD_ANYWHERE_ON_LEVEL)}}},

	// Mixed item library -- can check one item out at a time
	{{2, AMULET_LEVEL},           {30, 50},	10,		5,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)},
        {0,			ALTAR_CAGE_OPEN,DUNGEON,	{3,3},		3,			(WEAPON|ARMOR|STAFF),-1,	0,				2,				0,			(ITEM_MAX_CHARGES_KNOWN | ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{2,3},		2,			(STAFF|RING|CHARM),-1,	0,				2,				0,			(ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
        {0,			STATUE_INERT,DUNGEON,		{2,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)}}},

	// I'm rich!!
	{{8, AMULET_LEVEL},	{10, 40},	3,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			FUNGUS_FOREST,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,			0,				{3,6},		2,			(GOLD),	-1,			0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)},
        {0,			STATUE_INERT,DUNGEON,		{2,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)}}},
    // Commutation altars
	{{13, AMULET_LEVEL},{10, 30},	20,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			STATUE_INERT,DUNGEON,		{1,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)},
		{DF_MAGIC_PIPING,COMMUTATION_ALTAR,DUNGEON,{2,2},	2,			0,          -1,         0,              2,				0,			0,          (MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)}}},
    // Goblin warren
	{{5, 15},           {75, 200},	5,     9,			DP_GOBLIN_WARREN,   (BP_ROOM | BP_REWARD | BP_MAXIMIZE_INTERIOR | BP_REDESIGN_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			MUD_FLOOR,	DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,         MUD_DOORWAY,DUNGEON,        {1,1},      1,          0,          -1,         0,              1,              0,          0,          (MF_BUILD_AT_ORIGIN)},
        {0,         MUD_WALL,   DUNGEON,        {1,1},      100,        0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS | MF_EVERYWHERE)},
        {0,			0,	0,		{2,6},		1,			0,	0, MK_GOBLIN, 2,   0,			0,	(MF_MONSTER_SLEEPING)},
        {0,			0,	0,		{0,2},		1,			0,	0, MK_GOBLIN_CONJURER, 2,   0,			0,	(MF_MONSTER_SLEEPING)},
        {0,			0,	0,		{0,3},		1,			0,	0, MK_GOBLIN_DRIVER, 2,   0,			0,	(MF_MONSTER_SLEEPING)},
        {0,			0,	0,		{0,2},		1,			0,	0, MK_GOBLIN_MYSTIC, 2,   0,			0,	(MF_MONSTER_SLEEPING)},
        {0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(SCROLL),	SCROLL_ENCHANTING, MK_GOBLIN_CHIEFTAN, 2,   0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_MONSTER_SLEEPING | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
//        {0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(POTION),	POTION_STRENGTH, MK_GOBLIN_CHIEFTAN, 2,         0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_MONSTER_SLEEPING | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
        {0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(POTION),	POTION_EMPOWERMENT, MK_GOBLIN_CHIEFTAN, 2,         0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_MONSTER_SLEEPING | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
        {0,         0,          0,              {5, 8},     5,          0,          -1,         0,              2,              HORDE_MACHINE_GOBLIN_WARREN,    0,  (MF_GENERATE_HORDE | MF_NOT_IN_HALLWAY | MF_MONSTER_SLEEPING)},
        {0,			0,			0,				{2,3},		2,			(WEAPON|ARMOR),	-1,		0,				1,				0,			0,			(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {DF_HAY,	0,			0,				{10, 15},	1,			0,			-1,			0,				1,				0,			0,			(MF_NOT_IN_HALLWAY)},
		{DF_JUNK,	0,			0,				{7, 12},	1,			0,			-1,			0,				1,				0,			0,			(MF_NOT_IN_HALLWAY)}}},
    // Sentinel sanctuary
	{{10, 23},           {75, 200}, 3,  10,			DP_SENTINEL_SANCTUARY, (BP_ROOM | BP_REWARD | BP_MAXIMIZE_INTERIOR | BP_REDESIGN_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			MARBLE_FLOOR,DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,         CRYSTAL_WALL,DUNGEON,       {0,0},      0,          0,			-1,			0,				0,				0,			0,			(MF_BUILD_IN_WALLS | MF_EVERYWHERE)},
        {0,         PEDESTAL, DUNGEON, {1,1},	1,			(SCROLL),	SCROLL_ENCHANTING,0,	2,              0,              (ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
//        {0,         PEDESTAL, DUNGEON, {1,1},	1,			(POTION),	POTION_STRENGTH,0,          2,              0,              (ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
        {0,         PEDESTAL, DUNGEON, {1,1},	1,			(POTION),	POTION_EMPOWERMENT,0,          2,              0,              (ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
        {0,         MACHINE_GLYPH,DUNGEON,      {30, 35},	20,			0,			-1,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING)},
        {0,         STATUE_INERT,DUNGEON,       {3, 5},     3,          0,          -1,         MK_SENTINEL,    2,              0,          0,          (MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,         STATUE_INERT,DUNGEON,       {10, 15},   8,          0,          -1,         MK_SENTINEL,    2,              0,          0,          MF_BUILD_IN_WALLS},
        {0,         0,          0,              {4, 6},     4,          0,          -1,         MK_GUARDIAN,    1,              0,          0,          MF_TREAT_AS_BLOCKING},
        {0,         0,          0,              {0, 2},     0,          0,          -1,         MK_WINGED_GUARDIAN, 1,          0,          0,          MF_TREAT_AS_BLOCKING},
        {0,			0,			0,				{2,3},		2,			(SCROLL | POTION), -1,	0,				1,				0,			0,			(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Ogre armory -- a few equippables, including one good one, in a crappy makeshift hideout -- gsr
	{{9, 20},           {70, 150},	5,     12,			DP_OGRE_ARMORY,   (BP_ROOM | BP_REWARD | BP_MAXIMIZE_INTERIOR | BP_REDESIGN_INTERIOR | BP_PURGE_LIQUIDS | BP_SURROUND_WITH_WALLS),	{
//		{0,			MARBLE_FLOOR,     DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,         OGRE_DOORWAY,       DUNGEON,        {1,1},      1,          0,          -1,         0,              1,              0,          0,          (MF_BUILD_AT_ORIGIN)},
        {0,			OGRE_FLOOR,DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,         TABLE, DUNGEON, {1,1},	1,			(WEAPON|ARMOR),	0,0,          2,              0,              0,	(MF_REQUIRE_GOOD_RUNIC | MF_GENERATE_ITEM | MF_NOT_IN_HALLWAY)},
        {0,         TABLE, DUNGEON, {1,3},	1,			(WEAPON|ARMOR),	0,0,          2,              0,              0,	(MF_GENERATE_ITEM | MF_NOT_IN_HALLWAY)},
        {0,         TABLE,       DUNGEON,       {0,3},      0,          0,			-1,			0,				0,				0,			0,			(MF_NOT_IN_HALLWAY)},
        {0,         OGRE_WALL,       DUNGEON,       {0,0},      0,          0,			-1,			0,				0,				0,			0,			(MF_BUILD_IN_WALLS | MF_EVERYWHERE)},
        {DF_BONES, 0,	0,				{3,5},		3,			0,			-1,			0,				1,				0,			0,			0},
        {DF_HAY,	0,			0,				{1,2},	1,			0,			-1,			0,				1,				0,			0,			(MF_NOT_IN_HALLWAY)},
        {0,			0,	          0,	    	{2,8},		1,			0,	        -1, MK_OGRE, 2,   0,			0,	(MF_GENERATE_HORDE)},
        {0,			0,	          0,	    	{0,1},		1,			0,	        -1, MK_OGRE_SHAMAN, 2,   0,			0,	(MF_GENERATE_HORDE)},
        {0,         STATUE_INERT,DUNGEON,       {6,12},     0,          0,          -1,         0,    2,              0,          0,          (MF_NOT_IN_HALLWAY)},
        {0,			TORCH_WALL,	DUNGEON,		{5,6},		0,			0,			0,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS)}}},



/*
	// Mixed item library -- can check one item out at a time
	{{2, 12},           {30, 50},	10,		6,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)},
//		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{1,1},		1,			WAND,       WAND_EMPOWERMENT, 0,		2,				0,			(ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{1,1},		1,			STAFF,       STAFF_BLINKING, 0,		2,				0,			(ITEM_MAX_CHARGES_KNOWN | ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
        {0,			ALTAR_CAGE_OPEN,DUNGEON,	{3,3},		3,			(WEAPON|ARMOR|STAFF),-1,	0,				2,				0,			(ITEM_MAX_CHARGES_KNOWN | ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{2,3},		2,			(STAFF|RING|CHARM),-1,	0,				2,				0,			(ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
        {0,			STATUE_INERT,DUNGEON,		{2,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)}}},

	// Single item category library -- can check one item out at a time
	{{2, 12},           {30, 50},	10,		5,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{3,4},		3,			(RING),		-1,			0,				2,				0,			(ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_ALTERNATIVE | MF_IMPREGNABLE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{4,5},		4,			(STAFF),	-1,			0,				2,				0,			(ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_ALTERNATIVE | MF_IMPREGNABLE)},
        {0,			STATUE_INERT,DUNGEON,		{2,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)}}},
	// Treasure room -- apothecary or archive (potions or scrolls)
	{{8, AMULET_LEVEL},	{20, 40},	5,		6,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			0,			0,				{2,5},		2,			(POTION),	-1,			0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			0,			0,				{2,4},		2,			(SCROLL),	-1,			0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			FUNGUS_FOREST,SURFACE,		{3,4},		0,			0,			-1,			0,				2,				0,			0,			0},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)},
        {0,			STATUE_INERT,DUNGEON,		{2,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)}}},
	// Guaranteed good consumable item on glowing pedestals (scrolls of enchanting, potion of life)
	{{10, AMULET_LEVEL},{10, 30},	13,		5,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			STATUE_INERT,DUNGEON,		{1,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)},
		{0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(SCROLL),	SCROLL_ENCHANTING, 0,       2,				0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
//        {0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(POTION),	POTION_STRENGTH,0,              2,				0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
//        {0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(POTION),	POTION_LIFE,0,              2,				0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
        {0,			PEDESTAL,	DUNGEON,		{1,1},		1,			(POTION),	POTION_EMPOWERMENT,0,              2,				0,			(ITEM_KIND_AUTO_ID),	(MF_GENERATE_ITEM | MF_ALTERNATIVE | MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)}}},
    // Commutation altars
	{{13, AMULET_LEVEL},{10, 30},	20,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			STATUE_INERT,DUNGEON,		{1,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)},
		{DF_MAGIC_PIPING,COMMUTATION_ALTAR,DUNGEON,{2,2},	2,			0,          -1,         0,              2,				0,			0,          (MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)}}},
    // Resurrection altar
	{{13, AMULET_LEVEL},{10, 30},	10,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_IMPREGNABLE | BP_REWARD),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			STATUE_INERT,DUNGEON,		{1,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)},
		{DF_MACHINE_FLOOR_TRIGGER_REPEATING, RESURRECTION_ALTAR,DUNGEON, {1,1}, 1, 0, -1,       0,              2,				0,			0,          (MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)}}},
	// Dungeon -- two allies chained up for the taking
	{{5, AMULET_LEVEL},	{30, 80},	2,		5,			0,                  (BP_ROOM | BP_REWARD),	{
		{0,			VOMIT,		SURFACE,		{2,2},		2,			0,			-1,			0,				2,				(HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE), 0, (MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING)},
		{0,			MONSTER_CAGE_CLOSED,DUNGEON,{3,5},		3,			0,			-1,			0,				2,				(HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE), 0, (MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{DF_BONES,	0,			0,				{2,3},		1,			0,			-1,			0,				1,				0,			0,			0},
		{DF_VOMIT,	0,			0,				{2,3},		1,			0,			-1,			0,				1,				0,			0,			0},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)}}},
	// Kennel -- allies locked in cages in an open room; choose one or two to unlock and take with you.
	{{5, AMULET_LEVEL},	{30, 80},	2,		5,			0,                  (BP_ROOM | BP_REWARD),	{
		{0,			MONSTER_CAGE_CLOSED,DUNGEON,{3,5},		3,			0,			-1,			0,				2,				(HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE), 0, (MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{0,			0,			0,				{1,2},		1,			KEY,		KEY_CAGE,	0,				1,				0,			(ITEM_IS_KEY | ITEM_PLAYER_AVOIDS),(MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_SKELETON_KEY | MF_KEY_DISPOSABLE)},
        {DF_AMBIENT_BLOOD, 0,	0,				{3,5},		3,			0,			-1,			0,				1,				0,			0,			0},
        {DF_BONES,	0,			0,				{3,5},		3,			0,			-1,			0,				1,				0,			0,			0},
        {0,			TORCH_WALL,	DUNGEON,		{2,3},		2,			0,			0,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS)}}},
	// Vampire lair -- allies locked in cages and chained in a hidden room with a vampire in a coffin; vampire has one cage key.
	{{10, AMULET_LEVEL},{50, 80},	2,		4,			0,                  (BP_ROOM | BP_REWARD | BP_SURROUND_WITH_WALLS | BP_PURGE_INTERIOR),	{
		{DF_AMBIENT_BLOOD,0,	0,				{1,2},		1,			0,			-1,			0,				2,				(HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE), 0, (MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{DF_AMBIENT_BLOOD,MONSTER_CAGE_CLOSED,DUNGEON,{2,4},2,			0,			-1,			0,				2,				(HORDE_MACHINE_KENNEL | HORDE_LEADER_CAPTIVE), 0, (MF_GENERATE_HORDE | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE | MF_NOT_IN_HALLWAY)},
		{DF_TRIGGER_AREA,COFFIN_CLOSED,0,		{1,1},		1,			KEY,		KEY_CAGE,	MK_VAMPIRE,		1,				0,			(ITEM_IS_KEY | ITEM_PLAYER_AVOIDS),(MF_GENERATE_ITEM | MF_SKELETON_KEY | MF_MONSTER_TAKE_ITEM | MF_MONSTERS_DORMANT | MF_FAR_FROM_ORIGIN | MF_KEY_DISPOSABLE)},
		{DF_AMBIENT_BLOOD,SECRET_DOOR,DUNGEON,	{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)}}},
	// Legendary ally -- approach the altar with the crystal key to activate a portal and summon a legendary ally.
	{{8, AMULET_LEVEL}, {30, 50},	0,		2,			0,                  (BP_ROOM | BP_REWARD),	{
		{DF_LUMINESCENT_FUNGUS,	ALTAR_KEYHOLE, DUNGEON,	{1,1}, 1,		KEY,		KEY_PORTAL,	0,				2,				0,			(ITEM_IS_KEY | ITEM_PLAYER_AVOIDS),(MF_GENERATE_ITEM | MF_NOT_IN_HALLWAY | MF_NEAR_ORIGIN | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_KEY_DISPOSABLE)},
		{DF_LUMINESCENT_FUNGUS,	PORTAL,	DUNGEON,{1,1},		1,			0,			-1,			0,				2,				HORDE_MACHINE_LEGENDARY_ALLY,0,	(MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_FAR_FROM_ORIGIN)}}},
*/

    // -- AMULET HOLDER --
	{{10, AMULET_LEVEL},{35, 40},	0,		4,			0,                  (BP_PURGE_INTERIOR | BP_OPEN_INTERIOR),	{
		{DF_LUMINESCENT_FUNGUS,	AMULET_SWITCH, DUNGEON, {1,1}, 1,		AMULET,		-1,			0,				2,				0,			0,			(MF_GENERATE_ITEM | MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,			FUNGUS_FOREST,SURFACE,		{2,3},		0,			0,			-1,			0,				2,				0,			0,			MF_NOT_IN_HALLWAY},
		{0,			STATUE_INSTACRACK,DUNGEON,	{1,1},		1,			0,			-1,			MK_WARDEN_OF_YENDOR,1,          0,          0,          (MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_MONSTERS_DORMANT | MF_FAR_FROM_ORIGIN | MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_IMPREGNABLE)},
        {0,			TORCH_WALL,	DUNGEON,		{3,4},		0,			0,			0,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS)}}},

    // -- VESTIBULES --

    // Plain locked door, key guarded by an adoptive room
    {{1, AMULET_LEVEL-1},	{1, 1},     80,		1,		0,                  (BP_VESTIBULE),	{
		{0,			LOCKED_DOOR, DUNGEON,		{1,1},		1,			KEY,		KEY_DOOR,	0,				1,				0,			(ITEM_IS_KEY | ITEM_PLAYER_AVOIDS), (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_GENERATE_ITEM | MF_OUTSOURCE_ITEM_TO_MACHINE | MF_KEY_DISPOSABLE | MF_IMPREGNABLE)}}},
    // Plain secret door
    {{2, AMULET_LEVEL-1},	{1, 1},     20,		1,			0,                  (BP_VESTIBULE),	{
		{0,			SECRET_DOOR, DUNGEON,		{1,1},		1,			0,          0,          0,				1,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING)}}},
    // Lever and either an exploding wall or a portcullis
    {{4, AMULET_LEVEL-1},	{1, 1},     8,		3,			0,                  (BP_VESTIBULE),	{
		{0,         WORM_TUNNEL_OUTER_WALL,DUNGEON,{1,1},	1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_IMPREGNABLE | MF_ALTERNATIVE)},
        {0,			PORTCULLIS_CLOSED,DUNGEON,  {1,1},      1,			0,			0,			0,				3,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_IMPREGNABLE | MF_ALTERNATIVE)},
        {0,			WALL_LEVER_HIDDEN,DUNGEON,  {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS | MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_BUILD_ANYWHERE_ON_LEVEL)}}},

     // Pit traps -- area outside entrance is full of pit traps
	{{1, AMULET_LEVEL-1},	{30, 60},	8,		3,			0,                  (BP_VESTIBULE | BP_OPEN_INTERIOR | BP_NO_INTERIOR_FLAG),	{
		{0,			DOOR,       DUNGEON,        {1,1},      1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN | MF_ALTERNATIVE)},
        {0,			SECRET_DOOR,DUNGEON,        {1,1},      1,			0,			0,			0,				1,				0,			0,			(MF_IMPREGNABLE | MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN | MF_ALTERNATIVE)},
		{0,			TRAP_DOOR_HIDDEN,DUNGEON,	{60, 60},	1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)}}},
    // Beckoning obstacle -- a mirrored totem guards the door, and glyph are around the doorway.
	{{5, AMULET_LEVEL-1}, {15, 30},	10,		3,			0,                  (BP_VESTIBULE | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR), {
        {0,         DOOR,       DUNGEON,        {1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN)},
		{0,         0,          0,              {1,1},		1,			0,			-1,			MK_MIRRORED_TOTEM,3,			0,			0,			(MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_IN_VIEW_OF_ORIGIN | MF_FAR_FROM_ORIGIN)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},		0,			0,			-1,			0,				1,				0,			0,			(MF_NEAR_ORIGIN | MF_EVERYWHERE)},
        {0,         MACHINE_GLYPH,DUNGEON,      {3,5},      2,          0,          -1,         0,              2,              0,          0,          (MF_TREAT_AS_BLOCKING)}}},
    // "Go away" -- force totem at doorway
	{{5, AMULET_LEVEL-1}, {15, 30},	10,		3,			0,                  (BP_VESTIBULE | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR), {
        {0,         0,       0,        {1,1},		1,			0,			-1,			MK_FORCE_TOTEM,				1,				0,			0,			(MF_BUILD_AT_ORIGIN)}}},
    // Guardian obstacle -- a guardian is in the door on a glyph, with other glyphs scattered around.
	{{6, AMULET_LEVEL-1}, {25, 25},	10,		4,          0,                  (BP_VESTIBULE | BP_OPEN_INTERIOR),	{
		{0,			DOOR,       DUNGEON,        {1,1},		1,			0,			0,			MK_GUARDIAN,	2,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_ALTERNATIVE)},
		{0,			DOOR,       DUNGEON,        {1,1},		1,			0,			0,			MK_WINGED_GUARDIAN,2,           0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_ALTERNATIVE)},
        {0,         MACHINE_GLYPH,DUNGEON,      {10,10},    3,          0,          -1,         0,              1,              0,          0,          (MF_PERMIT_BLOCKING| MF_NEAR_ORIGIN)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},      0,          0,          -1,         0,              2,              0,          0,          (MF_EVERYWHERE | MF_PERMIT_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Flammable barricade in the doorway -- burn the wooden barricade to enter
	{{2, AMULET_LEVEL-1},			{1, 1},     10,		3,			0,                  (BP_VESTIBULE), {
		{0,			WOODEN_BARRICADE,DUNGEON,   {1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{0,			0,			0,				{1,1},		1,			THROWING_WEAPON,		INCENDIARY_DART, 0,			1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_INCINERATION, 0,		1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)}}},


 /*   // Flammable barricade in the doorway -- burn the wooden barricade to enter
	{{2, 15},			{1, 1},     10,		3,			0,                  (BP_VESTIBULE), {
		{0,			WOODEN_BARRICADE,DUNGEON,   {1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{0,			0,			0,				{1,1},		1,			THROWING_WEAPON,		INCENDIARY_DART, 0,			1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_INCINERATION, 0,		1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)}}},
    // Statue in the doorway -- use a scroll of shattering to enter
	{{2, AMULET_LEVEL},	{1, 1},     6,		2,			0,                  (BP_VESTIBULE), {
		{0,			STATUE_INERT_DOORWAY,DUNGEON,       {1,1},1,		0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{0,			0,			0,				{1,1},		1,			SCROLL,		SCROLL_SHATTERING, 0,       1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY)}}},
    // Statue in the doorway -- bursts to reveal monster
	{{5, AMULET_LEVEL},	{2, 2},     6,		2,			0,                  (BP_VESTIBULE), {
		{0,			STATUE_DORMANT_DOORWAY,DUNGEON,		{1, 1},	1,		0,			-1,			0,				1,				HORDE_MACHINE_STATUE,0,	(MF_PERMIT_BLOCKING | MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_AT_ORIGIN | MF_ALTERNATIVE)},
		{0,			MACHINE_TRIGGER_FLOOR,DUNGEON,{0,0},	1,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)}}},
    // Throwing tutorial -- toss an item onto the pressure plate to retract the portcullis
	{{1, 4},			{70, 70},	0,      3,          0,                  (BP_VESTIBULE), {
        {DF_MEDIUM_HOLE, MACHINE_PRESSURE_PLATE, LIQUID, {1,1}, 1,		0,			0,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{0,			PORTCULLIS_CLOSED,DUNGEON,  {1,1},      1,			0,			0,			0,				3,				0,			0,			(MF_IMPREGNABLE | MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN | MF_ALTERNATIVE)},
        {0,         WORM_TUNNEL_OUTER_WALL,DUNGEON,{1,1},	1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_IMPREGNABLE | MF_ALTERNATIVE)}}},
*/
	// -- KEY HOLDERS --

	// Nested item library -- can check one item out at a time, and one is a disposable key to another reward room
	{{2, AMULET_LEVEL},	{30, 50},	20,		7,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_ADOPT_ITEM | BP_IMPREGNABLE),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			WALL,       DUNGEON,		{0,0},      0,			0,			-1,			0,				0,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE | MF_EVERYWHERE)},
		{0,			0,          0,              {1,1},		1,			0,          0,          0,				2,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING | MF_BUILD_VESTIBULE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{1,2},		1,			(WEAPON|ARMOR|STAFF),-1,	0,				2,				0,			(ITEM_MAX_CHARGES_KNOWN | ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{1,2},		1,			(STAFF|RING|CHARM),-1,	0,				2,				0,			(ITEM_IS_KEY | ITEM_KIND_AUTO_ID | ITEM_MAX_CHARGES_KNOWN | ITEM_PLAYER_AVOIDS),	(MF_GENERATE_ITEM | MF_NO_THROWING_WEAPONS | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
		{0,			ALTAR_CAGE_OPEN,DUNGEON,	{1,1},		1,			0,			-1,			0,				2,				0,			(ITEM_IS_KEY | ITEM_PLAYER_AVOIDS | ITEM_MAX_CHARGES_KNOWN),	(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_IMPREGNABLE)},
        {0,			STATUE_INERT,DUNGEON,		{1,3},		0,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_BUILD_IN_WALLS | MF_IMPREGNABLE)}}},
	// Secret room -- key on an altar in a secret room
	{{2, AMULET_LEVEL},	{15, 100},	30,		2,			0,                  (BP_ROOM | BP_ADOPT_ITEM), {
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				1,				0,			ITEM_PLAYER_AVOIDS,	(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{0,			SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)}}},
    // Thief -- empty altar, monster with item, often permanently fleeing; sometimes far away from the altar
    {{3, AMULET_LEVEL},	{0, 0},	10,		2,			0,                  (BP_ADOPT_ITEM),	{
        {DF_LUMINESCENT_FUNGUS,	ALTAR_INERT,DUNGEON,{1,1},	1,			0,			-1,			0,				2,				0,0,			(MF_ALTERNATIVE | MF_ADOPT_ITEM | MF_BUILD_AT_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_GENERATE_HORDE | MF_MONSTER_TAKE_ITEM | MF_MONSTER_FLEEING)},
        {DF_LUMINESCENT_FUNGUS,	ALTAR_INERT,DUNGEON,{1,1},	1,			0,			-1,			0,				2,				0,0,			(MF_ALTERNATIVE | MF_ADOPT_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_GENERATE_HORDE | MF_MONSTER_TAKE_ITEM | MF_MONSTER_FLEEING)},
        {DF_LUMINESCENT_FUNGUS,	ALTAR_INERT,DUNGEON,{1,1},	1,			0,			-1,			0,				2,				0,0,			(MF_ALTERNATIVE | MF_ADOPT_ITEM | MF_BUILD_AT_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_GENERATE_HORDE | MF_MONSTER_TAKE_ITEM)},
        {0,         STATUE_INERT,0,             {3, 5},     2,          0,          -1,         0,              2,              0,          0,          (MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)}}},

    // Rat trap -- getting the key triggers paralysis vents nearby and also causes rats to burst out of the walls
	{{2,8},             {30, 70},	2,		3,          0,                  (BP_ADOPT_ITEM | BP_ROOM),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,			MACHINE_PARALYSIS_VENT_HIDDEN,DUNGEON,{1,1},1,		0,			-1,			0,				2,				0,			0,			(MF_FAR_FROM_ORIGIN | MF_NOT_IN_HALLWAY)},
        {0,			RAT_TRAP_WALL_DORMANT,DUNGEON,{10,20},	5,			0,			-1,			MK_RAT,         1,				0,			0,			(MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_NOT_ON_LEVEL_PERIMETER)}}},
	// Collapsing floor area -- key on an altar in an area; take key to cause the floor of the area to collapse
	{{2, AMULET_LEVEL},	{45, 65},	10,		3,			0,                  (BP_ADOPT_ITEM | BP_TREAT_AS_BLOCKING),	{
		{0,			FLOOR_FLOODABLE,DUNGEON,	{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			ALTAR_SWITCH_RETRACTING,DUNGEON,{1,1},	1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{DF_ADD_MACHINE_COLLAPSE_EDGE_DORMANT,0,0,{3, 3},	2,			0,			-1,			0,				3,				0,			0,			(MF_FAR_FROM_ORIGIN | MF_NOT_IN_HALLWAY)}}},
	// Pit traps -- key on an altar, room full of pit traps
	{{2, AMULET_LEVEL},	{30, 100},	10,		3,			0,                  (BP_ROOM | BP_ADOPT_ITEM),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			TRAP_DOOR_HIDDEN,DUNGEON,	{30, 40},	1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{0,			SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)}}},
	// Alarming -- can trip alarms all over
	{{2, AMULET_LEVEL},	{30, 50},	10,		3,			0,                  (BP_ROOM | BP_ADOPT_ITEM),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			ALARM_TRAP_HIDDEN,DUNGEON,	{5,9},     1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING)},
		{0,			SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)}}},
	// Explosive situation -- key on an altar; take key to cause a methane gas vent to appear and a pilot light to ignite
	{{7, AMULET_LEVEL},	{60, 70},	14,		5,			0,                  (BP_ROOM | BP_PURGE_LIQUIDS | BP_SURROUND_WITH_WALLS | BP_ADOPT_ITEM),	{
		{0,			DOOR,       DUNGEON,		{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN)},
        {0,			FLOOR,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_FAR_FROM_ORIGIN)},
		{0,			MACHINE_METHANE_VENT_HIDDEN,DUNGEON,{1,1}, 1,		0,			-1,			0,				1,				0,			0,			MF_NEAR_ORIGIN},
		{0,			PILOT_LIGHT_DORMANT,DUNGEON,{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_FAR_FROM_ORIGIN | MF_BUILD_IN_WALLS)}}},
	// Burning grass -- key on an altar; take key to cause pilot light to ignite grass in room
	{{2, 7},			{40, 110},	15,		6,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_ADOPT_ITEM | BP_OPEN_INTERIOR),	{
		{DF_SMALL_DEAD_GRASS,ALTAR_SWITCH_RETRACTING,DUNGEON,{1,1},1,	0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_FAR_FROM_ORIGIN)},
		{DF_DEAD_FOLIAGE,0,		SURFACE,		{2,3},		0,			0,			-1,			0,				1,				0,			0,			0},
		{0,			FOLIAGE,	SURFACE,		{1,4},		0,			0,			-1,			0,				1,				0,			0,			0},
		{0,			GRASS,		SURFACE,		{10,25},	0,			0,			-1,			0,				1,				0,			0,			0},
		{0,			DEAD_GRASS,	SURFACE,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			PILOT_LIGHT_DORMANT,DUNGEON,{1,1},		1,			0,			-1,			0,				1,				0,			0,			MF_NEAR_ORIGIN | MF_BUILD_IN_WALLS}}},
    // Guardian gauntlet -- key in a room full of guardians, glyphs scattered and unavoidable.
	{{6, AMULET_LEVEL}, {50, 95},	10,		6,			0,                  (BP_ROOM | BP_ADOPT_ITEM),	{
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,	{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
		{0,			DOOR,       DUNGEON,        {1,1},		1,			0,			0,			0,				3,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{0,			0,          0,              {3,6},		3,			0,			-1,			MK_GUARDIAN,	2,				0,			0,			(MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,          0,              {1,2},		1,			0,			-1,			MK_WINGED_GUARDIAN,2,           0,			0,			(MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
        {0,         MACHINE_GLYPH,DUNGEON,      {10,15},   10,          0,          -1,         0,              1,              0,          0,          (MF_PERMIT_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},      0,          0,          -1,         0,              2,              0,          0,          (MF_EVERYWHERE | MF_PERMIT_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Guardian corridor -- key in a small room, with a connecting corridor full of glyphs, one guardian blocking the corridor.
    {{4, AMULET_LEVEL}, {85, 100},   9,     7,          0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR | BP_SURROUND_WITH_WALLS),        {
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,   {1,1},      1,          0,          -1,         MK_GUARDIAN,    3,              0,          0,          (MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN  | MF_ALTERNATIVE)},
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,   {1,1},      1,          0,          -1,         MK_WINGED_GUARDIAN,3,           0,          0,          (MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN  | MF_ALTERNATIVE)},
        {0,         MACHINE_GLYPH,DUNGEON,      {3,5},      2,          0,          0,          0,              2,              0,          0,          MF_NEAR_ORIGIN | MF_NOT_IN_HALLWAY},
        {0,         0,          0,              {1,1},      1,          0,          0,          0,              3,              0,          0,          MF_BUILD_AT_ORIGIN},
        {0,         WALL,DUNGEON,               {80,80},    1,          0,          -1,         0,              1,              0,          0,          (MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},      1,          0,          0,          0,              1,              0,          0,          (MF_PERMIT_BLOCKING | MF_EVERYWHERE)},
        {0,			MACHINE_GLYPH,DUNGEON,      {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_NOT_IN_HALLWAY | MF_BUILD_ANYWHERE_ON_LEVEL)}}},
    // Summoning circle -- key in a room with an eldritch totem, glyphs unavoidable.
	{{12, AMULET_LEVEL}, {50, 100},	2,		2,			0,                  (BP_ROOM | BP_OPEN_INTERIOR | BP_ADOPT_ITEM),	{
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,	{1,1},		1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
		{DF_GLYPH_CIRCLE,0,     0,              {1,1},		1,			0,			-1,			MK_ELDRITCH_TOTEM,3,			0,			0,			(MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Beckoning obstacle -- key surrounded by glyphs in a room with a mirrored totem.
	{{5, AMULET_LEVEL}, {60, 100},	10,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_ADOPT_ITEM), {
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,	{1,1},		1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN | MF_IN_VIEW_OF_ORIGIN)},
		{0,         0,          0,              {1,1},		1,			0,			-1,			MK_MIRRORED_TOTEM,3,			0,			0,			(MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_IN_VIEW_OF_ORIGIN)},
        {0,         0,          0,              {1,1},      1,          0,          -1,         0,              2,              0,          0,          (MF_BUILD_AT_ORIGIN)},
        {0,         MACHINE_GLYPH,DUNGEON,      {3,5},      2,          0,          -1,         0,              2,              0,          0,          (MF_TREAT_AS_BLOCKING)}}},
    // Worms in the walls -- key on altar; take key to cause underworms to burst out of the walls
	{{12,AMULET_LEVEL},	{7, 7},		7,		2,			0,                  (BP_ADOPT_ITEM),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{0,			WALL_MONSTER_DORMANT,DUNGEON,{5,8},		5,			0,			-1,			MK_UNDERWORM,	1,				0,			0,			(MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_NOT_ON_LEVEL_PERIMETER)}}},
	// Mud pit -- key on an altar, room full of mud, take key to cause bog monsters to spawn in the mud
	{{12, AMULET_LEVEL},{40, 90},	10,		3,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_PURGE_LIQUIDS),	{
		{DF_SWAMP,		0,		0,				{5,5},		0,			0,			-1,			0,				1,				0,			0,			0},
		{DF_SWAMP,	ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{DF_MUD_DORMANT,0,		0,				{3,4},		3,			0,			-1,			0,				1,				HORDE_MACHINE_MUD,0,	(MF_GENERATE_HORDE | MF_MONSTERS_DORMANT)}}},
    // Zombie crypt -- key on an altar; coffins scattered around; brazier in the room; take key to cause zombies to burst out of all of the coffins
	{{12, AMULET_LEVEL},{60, 90},	10,		8,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_PURGE_INTERIOR),	{
		{0,         DOOR,       DUNGEON,        {1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN)},
        {DF_BONES,  0,          0,              {3,4},      2,          0,          -1,         0,              1,              0,          0,          0},
        {DF_ASH,    0,          0,              {3,4},      2,          0,          -1,         0,              1,              0,          0,          0},
        {DF_AMBIENT_BLOOD,0,    0,              {1,2},		1,			0,			-1,			0,				1,				0,			0,			0},
		{DF_AMBIENT_BLOOD,0,    0,              {1,2},		1,			0,			-1,			0,				1,				0,			0,			0},
        {0,         ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
        {0,         BRAZIER,    DUNGEON,        {1,1},      1,          0,          -1,         0,              2,              0,          0,          (MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING)},
        {0,         COFFIN_CLOSED, DUNGEON,		{6,8},		1,			0,          0,          MK_ZOMBIE,		2,				0,			0,          (MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_MONSTERS_DORMANT)}}},
	// Haunted house -- key on an altar; take key to cause the room to darken, ectoplasm to cover everything and phantoms to appear
	{{16, AMULET_LEVEL},{45, 150},	13,		4,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			DARK_FLOOR_DORMANT,DUNGEON,	{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			DARK_FLOOR_DORMANT,DUNGEON,	{4,5},		4,			0,			-1,			MK_PHANTOM,		1,				0,			0,			(MF_MONSTERS_DORMANT)},
        {0,         HAUNTED_TORCH_DORMANT,DUNGEON,{5,10},   3,          0,          -1,         0,              2,              0,          0,          (MF_BUILD_IN_WALLS)}}},
    // Worm tunnels -- hidden lever causes tunnels to open up revealing worm areas and a key
    {{8, AMULET_LEVEL},{80, 175},	10,		6,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_MAXIMIZE_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {3,6},		3,			0,			-1,			MK_UNDERWORM,	1,				0,			0,			0},
		{0,			GRANITE,    DUNGEON,        {150,150},  1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
        {DF_WORM_TUNNEL_MARKER_DORMANT,GRANITE,DUNGEON,{0,0},0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE | MF_PERMIT_BLOCKING)},
        {DF_TUNNELIZE,WORM_TUNNEL_OUTER_WALL,DUNGEON,{1,1},	1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING)},
        {0,			WALL_LEVER_HIDDEN,DUNGEON,  {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS | MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_BUILD_ANYWHERE_ON_LEVEL)}}},
    // Electric crystals -- key caged on an altar, darkened crystal globes around the room, lightning the globes to release the key.
	{{6, AMULET_LEVEL},{40, 60},	10,		4,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_PURGE_INTERIOR),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,         ELECTRIC_CRYSTAL_OFF,DUNGEON,{3,4},		3,			0,			-1,			0,				3,				0,			0,			(MF_NOT_IN_HALLWAY | MF_IMPREGNABLE)},
        {0,			ALTAR_CAGE_RETRACTABLE,DUNGEON,{1,1},	1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_IMPREGNABLE | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
        {0,         TURRET_LEVER, DUNGEON,      {7,9},      4,			0,			-1,			MK_SPARK_TURRET,3,				0,          0,          (MF_BUILD_IN_WALLS | MF_MONSTERS_DORMANT)}}},

 /*
 	// Fun with fire -- trigger the fire trap and coax the fire over to the wooden barricade surrounding the altar and key
    {{3, 10},			{80, 100},	10,		6,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR), {
		{DF_SURROUND_WOODEN_BARRICADE,ALTAR_INERT,DUNGEON,{1,1},1,		0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			GRASS,		SURFACE,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE | MF_ALTERNATIVE)},
		{DF_SWAMP,	0,			0,				{4,4},		2,			0,			-1,			0,				2,				0,			0,			(MF_ALTERNATIVE | MF_FAR_FROM_ORIGIN)},
		{0,			FLAMETHROWER_HIDDEN,DUNGEON,{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_NEAR_ORIGIN)},
		{0,			GAS_TRAP_POISON_HIDDEN,DUNGEON,{3, 3},	1,			0,			-1,			0,				5,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_ALTERNATIVE)},
		{0,			0,			0,				{2,2},		1,			POTION,		POTION_INCINERATION, 0,           3,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)}}},
	// Flood room -- key on an altar in a room with pools of eel-infested waters; take key to flood room with shallow water
	{{3, AMULET_LEVEL},	{80, 180},	5,		4,			0,                  (BP_ROOM | BP_SURROUND_WITH_WALLS | BP_PURGE_LIQUIDS | BP_PURGE_PATHING_BLOCKERS | BP_ADOPT_ITEM),	{
		{0,			FLOOR_FLOODABLE,LIQUID,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				5,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{DF_SPREADABLE_WATER_POOL,0,0,          {2, 4},		1,			0,			-1,			0,				5,				HORDE_MACHINE_WATER_MONSTER,0,MF_GENERATE_HORDE},
        {DF_GRASS,	FOLIAGE,	SURFACE,		{3, 4},		3,			0,			-1,			0,				1,				0,			0,			0}}},
    // Fire trap room -- key on an altar, pools of water, fire traps all over the place.
	{{4, AMULET_LEVEL},	{80, 180},	5,		5,			0,                  (BP_ROOM | BP_SURROUND_WITH_WALLS | BP_PURGE_LIQUIDS | BP_PURGE_PATHING_BLOCKERS | BP_ADOPT_ITEM),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,         0,          0,              {1, 1},     1,          0,          -1,         0,              4,              0,          0,          MF_BUILD_AT_ORIGIN},
        {0,         FLAMETHROWER_HIDDEN,DUNGEON,{40, 60},   20,         0,          -1,         0,              1,              0,          0,          (MF_TREAT_AS_BLOCKING)},
		{DF_DEEP_WATER_POOL,0,  0,              {4, 4},		1,			0,			-1,			0,				4,				HORDE_MACHINE_WATER_MONSTER,0,MF_GENERATE_HORDE},
        {DF_GRASS,	FOLIAGE,	SURFACE,		{3, 4},		3,			0,			-1,			0,				1,				0,			0,			0}}},

	// Collapsing floor area -- key on an altar in an area; take key to cause the floor of the area to collapse
	{{2, AMULET_LEVEL},	{45, 65},	10,		3,			0,                  (BP_ADOPT_ITEM | BP_TREAT_AS_BLOCKING),	{
		{0,			FLOOR_FLOODABLE,DUNGEON,	{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			ALTAR_SWITCH_RETRACTING,DUNGEON,{1,1},	1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{DF_ADD_MACHINE_COLLAPSE_EDGE_DORMANT,0,0,{3, 3},	2,			0,			-1,			0,				3,				0,			0,			(MF_FAR_FROM_ORIGIN | MF_NOT_IN_HALLWAY)}}},
	// Pit traps -- key on an altar, room full of pit traps
	{{2, AMULET_LEVEL},	{30, 100},	10,		3,			0,                  (BP_ROOM | BP_ADOPT_ITEM),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			TRAP_DOOR_HIDDEN,DUNGEON,	{30, 40},	1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{0,			SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)}}},
	// Levitation challenge -- key on an altar, room filled with pit, levitation or lever elsewhere on level, bridge appears when you grab the key/lever.
	{{2, 13},			{75, 120},	10,		9,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			TORCH_WALL,	DUNGEON,		{1,4},		0,			0,			0,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS)},
		{0,			0,			0,				{1,1},		1,			0,			0,			0,				3,				0,			0,			MF_BUILD_AT_ORIGIN},
		{DF_ADD_DORMANT_CHASM_HALO,	CHASM,LIQUID,{120, 120},1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{DF_ADD_DORMANT_CHASM_HALO,	CHASM_WITH_HIDDEN_BRIDGE,LIQUID,{1,1},1,0,		0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_EVERYWHERE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_LEVITATION, 0,       1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
        {0,			WALL_LEVER_HIDDEN,DUNGEON,  {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS | MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_BUILD_ANYWHERE_ON_LEVEL | MF_ALTERNATIVE)}}},
	// Web climbing -- key on an altar, room filled with pit, spider at altar to shoot webs, bridge appears when you grab the key
	{{7, AMULET_LEVEL},	{55, 90},	0,		7,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			MK_SPIDER,		3,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_IN_VIEW_OF_ORIGIN)},
		{0,			TORCH_WALL,	DUNGEON,		{1,4},		0,			0,			0,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS)},
		{0,			0,			0,				{1,1},		1,			0,			0,			0,				3,				0,			0,			MF_BUILD_AT_ORIGIN},
		{DF_ADD_DORMANT_CHASM_HALO,	CHASM,LIQUID,	{120, 120},	1,		0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{DF_ADD_DORMANT_CHASM_HALO,	CHASM_WITH_HIDDEN_BRIDGE,LIQUID,{1,1},1,0,		0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_EVERYWHERE)}}},
	// Lava moat room -- key on an altar, room filled with lava, levitation/fire immunity/lever elsewhere on level, lava retracts when you grab the key/lever
	{{3, 13},			{75, 120},	10,		7,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			0,			0,				{1,1},		1,			0,			0,			0,				2,				0,			0,			(MF_BUILD_AT_ORIGIN)},
        {0,			LAVA,       LIQUID,         {60,60},	1,			0,			0,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{DF_LAVA_RETRACTABLE, 0, 0,             {1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_EVERYWHERE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_LEVITATION, 0,       1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_FIRE_IMMUNITY, 0,	1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
        {0,			WALL_LEVER_HIDDEN,DUNGEON,  {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS | MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_BUILD_ANYWHERE_ON_LEVEL | MF_ALTERNATIVE)}}},
	// Lava moat area -- key on an altar, surrounded with lava, levitation/fire immunity elsewhere on level, lava retracts when you grab the key
	{{3, 13},			{40, 60},	3,		5,			0,                  (BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR | BP_TREAT_AS_BLOCKING),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_BUILD_AT_ORIGIN | MF_TREAT_AS_BLOCKING)},
        {0,			LAVA,       LIQUID,         {60,60},	1,			0,			0,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{DF_LAVA_RETRACTABLE, 0, 0,             {1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_PERMIT_BLOCKING | MF_EVERYWHERE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_LEVITATION, 0,       1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,			0,				{1,1},		1,			POTION,		POTION_FIRE_IMMUNITY, 0,    1,				0,			0,			(MF_GENERATE_ITEM | MF_BUILD_ANYWHERE_ON_LEVEL | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)}}},
	// Poison gas -- key on an altar; take key to cause a caustic gas vent to appear and the door to be blocked; there is a hidden trapdoor or an escape item somewhere inside
	{{4, AMULET_LEVEL},	{35, 60},	7,		7,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_ADOPT_ITEM),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING)},
		{0,			MACHINE_POISON_GAS_VENT_HIDDEN,DUNGEON,{1,2}, 1,	0,			-1,			0,				2,				0,			0,			0},
		{0,			TRAP_DOOR_HIDDEN,DUNGEON,	{1,1},		1,			0,			-1,			0,				2,				0,			0,			MF_ALTERNATIVE},
		{0,			0,			0,				{1,1},		1,			SCROLL,		SCROLL_TELEPORT, 0,			2,				0,			0,			(MF_GENERATE_ITEM | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,			0,				{1,1},		1,			SCROLL,		SCROLL_TELEPORT, 0,         2,				0,			0,			(MF_GENERATE_ITEM | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
        {0,			WALL_LEVER_HIDDEN_DORMANT,DUNGEON,{1,1},1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS)},
        {0,			PORTCULLIS_DORMANT,DUNGEON,{1,1},       1,          0,			0,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING)}}},
	// Explosive situation -- key on an altar; take key to cause a methane gas vent to appear and a pilot light to ignite
	{{7, AMULET_LEVEL},	{80, 90},	14,		5,			0,                  (BP_ROOM | BP_PURGE_LIQUIDS | BP_SURROUND_WITH_WALLS | BP_ADOPT_ITEM),	{
		{0,			DOOR,       DUNGEON,		{1,1},		1,			0,			0,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN)},
        {0,			FLOOR,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
        {0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_FAR_FROM_ORIGIN)},
		{0,			MACHINE_METHANE_VENT_HIDDEN,DUNGEON,{1,1}, 1,		0,			-1,			0,				1,				0,			0,			MF_NEAR_ORIGIN},
		{0,			PILOT_LIGHT_DORMANT,DUNGEON,{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_FAR_FROM_ORIGIN | MF_BUILD_IN_WALLS)}}},
	// Burning grass -- key on an altar; take key to cause pilot light to ignite grass in room
	{{2, 7},			{40, 110},	15,		6,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_ADOPT_ITEM | BP_OPEN_INTERIOR),	{
		{DF_SMALL_DEAD_GRASS,ALTAR_SWITCH_RETRACTING,DUNGEON,{1,1},1,	0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_FAR_FROM_ORIGIN)},
		{DF_DEAD_FOLIAGE,0,		SURFACE,		{2,3},		0,			0,			-1,			0,				1,				0,			0,			0},
		{0,			FOLIAGE,	SURFACE,		{1,4},		0,			0,			-1,			0,				1,				0,			0,			0},
		{0,			GRASS,		SURFACE,		{10,25},	0,			0,			-1,			0,				1,				0,			0,			0},
		{0,			DEAD_GRASS,	SURFACE,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			PILOT_LIGHT_DORMANT,DUNGEON,{1,1},		1,			0,			-1,			0,				1,				0,			0,			MF_NEAR_ORIGIN | MF_BUILD_IN_WALLS}}},
    // Guardian water puzzle -- key held by a guardian, flood trap in the room, glyphs scattered. Lure the guardian into the water to have him drop the key.
	{{20, AMULET_LEVEL}, {35, 70},	2,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_ADOPT_ITEM),	{
        {0,         0,          0,              {1,1},      1,          0,          -1,         0,              2,              0,          0,          (MF_BUILD_AT_ORIGIN)},
		{0,			0,          0,              {1,1},		1,			0,			-1,			MK_GUARDIAN,	2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_MONSTER_TAKE_ITEM)},
		{0,			FLOOD_TRAP,DUNGEON,         {1,1},		1,			0,			-1,			0,				2,				0,          0,          (MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},      4,          0,          -1,         0,              2,              0,          0,          (MF_EVERYWHERE | MF_NOT_IN_HALLWAY)}}},
    // Guardian gauntlet -- key in a room full of guardians, glyphs scattered and unavoidable.
	{{6, AMULET_LEVEL}, {50, 95},	10,		6,			0,                  (BP_ROOM | BP_ADOPT_ITEM),	{
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,	{1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
		{0,			DOOR,       DUNGEON,        {1,1},		1,			0,			0,			0,				3,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{0,			0,          0,              {3,6},		3,			0,			-1,			MK_GUARDIAN,	2,				0,			0,			(MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
		{0,			0,          0,              {1,2},		1,			0,			-1,			MK_WINGED_GUARDIAN,2,           0,			0,			(MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_ALTERNATIVE)},
        {0,         MACHINE_GLYPH,DUNGEON,      {10,15},   10,          0,          -1,         0,              1,              0,          0,          (MF_PERMIT_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},      0,          0,          -1,         0,              2,              0,          0,          (MF_EVERYWHERE | MF_PERMIT_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Guardian corridor -- key in a small room, with a connecting corridor full of glyphs, one guardian blocking the corridor.
    {{4, AMULET_LEVEL}, {85, 100},   9,     7,          0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_OPEN_INTERIOR | BP_SURROUND_WITH_WALLS),        {
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,   {1,1},      1,          0,          -1,         MK_GUARDIAN,    3,              0,          0,          (MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN  | MF_ALTERNATIVE)},
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,   {1,1},      1,          0,          -1,         MK_WINGED_GUARDIAN,3,           0,          0,          (MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN  | MF_ALTERNATIVE)},
        {0,         MACHINE_GLYPH,DUNGEON,      {3,5},      2,          0,          0,          0,              2,              0,          0,          MF_NEAR_ORIGIN | MF_NOT_IN_HALLWAY},
        {0,         0,          0,              {1,1},      1,          0,          0,          0,              3,              0,          0,          MF_BUILD_AT_ORIGIN},
        {0,         WALL,DUNGEON,               {80,80},    1,          0,          -1,         0,              1,              0,          0,          (MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
        {0,         MACHINE_GLYPH,DUNGEON,      {1,1},      1,          0,          0,          0,              1,              0,          0,          (MF_PERMIT_BLOCKING | MF_EVERYWHERE)},
        {0,			MACHINE_GLYPH,DUNGEON,      {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_NOT_IN_HALLWAY | MF_BUILD_ANYWHERE_ON_LEVEL)}}},
    // Summoning circle -- key in a room with an eldritch totem, glyphs unavoidable.
	{{12, AMULET_LEVEL}, {50, 100},	0,		2,			0,                  (BP_ROOM | BP_OPEN_INTERIOR | BP_ADOPT_ITEM),	{
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,	{1,1},		1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
		{DF_GLYPH_CIRCLE,0,     0,              {1,1},		1,			0,			-1,			MK_ELDRITCH_TOTEM,3,			0,			0,			(MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Beckoning obstacle -- key surrounded by glyphs in a room with a mirrored totem.
	{{5, AMULET_LEVEL}, {60, 100},	10,		4,			0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_ADOPT_ITEM), {
        {DF_GLYPH_CIRCLE,ALTAR_INERT,DUNGEON,	{1,1},		1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN | MF_IN_VIEW_OF_ORIGIN)},
		{0,         0,          0,              {1,1},		1,			0,			-1,			MK_MIRRORED_TOTEM,3,			0,			0,			(MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_IN_VIEW_OF_ORIGIN)},
        {0,         0,          0,              {1,1},      1,          0,          -1,         0,              2,              0,          0,          (MF_BUILD_AT_ORIGIN)},
        {0,         MACHINE_GLYPH,DUNGEON,      {3,5},      2,          0,          -1,         0,              2,              0,          0,          (MF_TREAT_AS_BLOCKING)}}},
    // Worms in the walls -- key on altar; take key to cause underworms to burst out of the walls
	{{12,AMULET_LEVEL},	{7, 7},		7,		2,			0,                  (BP_ADOPT_ITEM),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
		{0,			WALL_MONSTER_DORMANT,DUNGEON,{5,8},		5,			0,			-1,			MK_UNDERWORM,	1,				0,			0,			(MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_NOT_ON_LEVEL_PERIMETER)}}},
	// Mud pit -- key on an altar, room full of mud, take key to cause bog monsters to spawn in the mud
	{{12, AMULET_LEVEL},{40, 90},	10,		3,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_PURGE_LIQUIDS),	{
		{DF_SWAMP,		0,		0,				{5,5},		0,			0,			-1,			0,				1,				0,			0,			0},
		{DF_SWAMP,	ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{DF_MUD_DORMANT,0,		0,				{3,4},		3,			0,			-1,			0,				1,				HORDE_MACHINE_MUD,0,	(MF_GENERATE_HORDE | MF_MONSTERS_DORMANT)}}},
    // Electric crystals -- key caged on an altar, darkened crystal globes around the room, lightning the globes to release the key.
	{{6, AMULET_LEVEL},{40, 60},	10,		4,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR | BP_PURGE_INTERIOR),	{
		{0,			CARPET,		DUNGEON,		{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,         ELECTRIC_CRYSTAL_OFF,DUNGEON,{3,4},		3,			0,			-1,			0,				3,				0,			0,			(MF_NOT_IN_HALLWAY | MF_IMPREGNABLE)},
        {0,			ALTAR_CAGE_RETRACTABLE,DUNGEON,{1,1},	1,			0,			-1,			0,				3,				0,			0,			(MF_ADOPT_ITEM | MF_IMPREGNABLE | MF_NOT_IN_HALLWAY | MF_FAR_FROM_ORIGIN)},
        {0,         TURRET_LEVER, DUNGEON,      {7,9},      4,			0,			-1,			MK_SPARK_TURRET,3,				0,          0,          (MF_BUILD_IN_WALLS | MF_MONSTERS_DORMANT)}}},
    // Zombie crypt -- key on an altar; coffins scattered around; brazier in the room; take key to cause zombies to burst out of all of the coffins
	{{12, AMULET_LEVEL},{60, 90},	10,		8,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_PURGE_INTERIOR),	{
		{0,         DOOR,       DUNGEON,        {1,1},		1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN)},
        {DF_BONES,  0,          0,              {3,4},      2,          0,          -1,         0,              1,              0,          0,          0},
        {DF_ASH,    0,          0,              {3,4},      2,          0,          -1,         0,              1,              0,          0,          0},
        {DF_AMBIENT_BLOOD,0,    0,              {1,2},		1,			0,			-1,			0,				1,				0,			0,			0},
		{DF_AMBIENT_BLOOD,0,    0,              {1,2},		1,			0,			-1,			0,				1,				0,			0,			0},
        {0,         ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
        {0,         BRAZIER,    DUNGEON,        {1,1},      1,          0,          -1,         0,              2,              0,          0,          (MF_NEAR_ORIGIN | MF_TREAT_AS_BLOCKING)},
        {0,         COFFIN_CLOSED, DUNGEON,		{6,8},		1,			0,          0,          MK_ZOMBIE,		2,				0,			0,          (MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY | MF_MONSTERS_DORMANT)}}},
	// Haunted house -- key on an altar; take key to cause the room to darken, ectoplasm to cover everything and phantoms to appear
	{{16, AMULET_LEVEL},{45, 150},	13,		4,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			DARK_FLOOR_DORMANT,DUNGEON,	{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			DARK_FLOOR_DORMANT,DUNGEON,	{4,5},		4,			0,			-1,			MK_PHANTOM,		1,				0,			0,			(MF_MONSTERS_DORMANT)},
        {0,         HAUNTED_TORCH_DORMANT,DUNGEON,{5,10},   3,          0,          -1,         0,              2,              0,          0,          (MF_BUILD_IN_WALLS)}}},
    // Worm tunnels -- hidden lever causes tunnels to open up revealing worm areas and a key
    {{8, AMULET_LEVEL},{80, 175},	10,		6,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_PURGE_INTERIOR | BP_MAXIMIZE_INTERIOR | BP_SURROUND_WITH_WALLS),	{
		{0,			ALTAR_INERT,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING)},
		{0,			0,          0,              {3,6},		3,			0,			-1,			MK_UNDERWORM,	1,				0,			0,			0},
		{0,			GRANITE,    DUNGEON,        {150,150},  1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
        {DF_WORM_TUNNEL_MARKER_DORMANT,GRANITE,DUNGEON,{0,0},0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE | MF_PERMIT_BLOCKING)},
        {DF_TUNNELIZE,WORM_TUNNEL_OUTER_WALL,DUNGEON,{1,1},	1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_PERMIT_BLOCKING)},
        {0,			WALL_LEVER_HIDDEN,DUNGEON,  {1,1},      1,			0,			-1,			0,				1,				0,			0,			(MF_BUILD_IN_WALLS | MF_IN_PASSABLE_VIEW_OF_ORIGIN | MF_BUILD_ANYWHERE_ON_LEVEL)}}},


    // Gauntlet -- key on an altar; take key to cause turrets to emerge
	{{5, 24},			{35, 90},	9,		2,			0,                  (BP_ADOPT_ITEM | BP_NO_INTERIOR_FLAG),	{
		{0,			ALTAR_SWITCH,DUNGEON,		{1,1},		1,			0,			-1,			0,				2,				0,			0,			(MF_ADOPT_ITEM | MF_NEAR_ORIGIN | MF_NOT_IN_HALLWAY | MF_TREAT_AS_BLOCKING)},
		{0,			TURRET_DORMANT,DUNGEON,		{4,6},		4,			0,			-1,			0,				2,				HORDE_MACHINE_TURRET,0,	(MF_TREAT_AS_BLOCKING | MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_IN_VIEW_OF_ORIGIN)}}},


	// Boss -- key is held by a boss atop a pile of bones in a secret room. A few fungus patches light up the area.
	{{5, AMULET_LEVEL},	{40, 100},	18,		3,			0,                  (BP_ROOM | BP_ADOPT_ITEM | BP_SURROUND_WITH_WALLS | BP_PURGE_LIQUIDS), {
		{DF_BONES,	SECRET_DOOR,DUNGEON,		{1,1},		1,			0,			0,			0,				3,				0,			0,			(MF_PERMIT_BLOCKING | MF_BUILD_AT_ORIGIN)},
		{DF_LUMINESCENT_FUNGUS,	STATUE_INERT,DUNGEON,{7,7},	0,			0,			-1,			0,				2,				0,			0,			(MF_TREAT_AS_BLOCKING)},
		{DF_BONES,	0,			0,				{1,1},		1,			0,			-1,			0,				1,				HORDE_MACHINE_BOSS,	0,	(MF_ADOPT_ITEM | MF_FAR_FROM_ORIGIN | MF_MONSTER_TAKE_ITEM | MF_GENERATE_HORDE | MF_MONSTER_SLEEPING)}}},
*/
	// -- FLAVOR MACHINES --

	// Bloodwort -- bloodwort stalk, some pods, and surrounding grass
	{{1,DEEPEST_LEVEL},	{5, 5},     0,          2,      0,                  (BP_TREAT_AS_BLOCKING), {
		{DF_GRASS,	BLOODFLOWER_STALK, SURFACE,	{1, 1},		1,			0,			-1,			0,				0,				0,			0,			(MF_BUILD_AT_ORIGIN | MF_NOT_IN_HALLWAY)},
		{DF_BLOODFLOWER_PODS_GROW_INITIAL,0, 0, {1, 1},     1,			0,			-1,			0,				1,				0,			0,          (MF_BUILD_AT_ORIGIN | MF_TREAT_AS_BLOCKING)}}},
	// Shrine -- safe haven constructed and abandoned by a past adventurer
	{{1,DEEPEST_LEVEL},	{15, 25},   0,          3,      0,                  (BP_ROOM | BP_PURGE_INTERIOR | BP_SURROUND_WITH_WALLS | BP_OPEN_INTERIOR), {
		{0,         SACRED_GLYPH,  DUNGEON,     {1, 1},		1,			0,			-1,			0,				3,				0,			0,			(MF_BUILD_AT_ORIGIN)},
		{0,         HAVEN_BEDROLL, SURFACE,     {1, 1},     1,			0,			-1,			0,				2,				0,			0,          (MF_FAR_FROM_ORIGIN | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)},
        {0,         BONES,      SURFACE,        {1, 1},     1,			(POTION|SCROLL|WEAPON|ARMOR|RING),-1,0,	2,				0,			0,          (MF_GENERATE_ITEM | MF_TREAT_AS_BLOCKING | MF_NOT_IN_HALLWAY)}}},
    // Idyll -- ponds and some grass and forest
	{{1,DEEPEST_LEVEL},	{80, 120},	0,		2,			0,                  BP_NO_INTERIOR_FLAG, {
		{DF_GRASS,	FOLIAGE,	SURFACE,		{3, 4},		3,			0,			-1,			0,				1,				0,			0,			0},
		{DF_DEEP_WATER_POOL,0,	0,				{2, 3},		2,			0,			-1,			0,				5,				0,			0,			(MF_NOT_IN_HALLWAY)}}},
	// Swamp -- mud, grass and some shallow water
	{{1,DEEPEST_LEVEL},	{50, 65},	0,		2,			0,                  BP_NO_INTERIOR_FLAG, {
		{DF_SWAMP,	0,			0,				{6, 8},		3,			0,			-1,			0,				1,				0,			0,			0},
		{DF_DEEP_WATER_POOL,0,	0,				{0, 1},		0,			0,			-1,			0,				3,				0,			0,			(MF_NOT_IN_HALLWAY | MF_TREAT_AS_BLOCKING)}}},
	// Camp -- hay, junk, urine, vomit
	{{1,DEEPEST_LEVEL},	{40, 50},	0,		4,			0,                  BP_NO_INTERIOR_FLAG, {
		{DF_HAY,	0,			0,				{1, 3},		1,			0,			-1,			0,				1,				0,			0,			(MF_NOT_IN_HALLWAY | MF_IN_VIEW_OF_ORIGIN)},
		{DF_JUNK,	0,			0,				{1, 2},		1,			0,			-1,			0,				3,				0,			0,			(MF_NOT_IN_HALLWAY | MF_IN_VIEW_OF_ORIGIN)},
		{DF_URINE,	0,			0,				{3, 5},		1,			0,			-1,			0,				1,				0,			0,			MF_IN_VIEW_OF_ORIGIN},
		{DF_VOMIT,	0,			0,				{0, 2},		0,			0,			-1,			0,				1,				0,			0,			MF_IN_VIEW_OF_ORIGIN}}},
	// Remnant -- carpet surrounded by ash and with some statues
	{{1,DEEPEST_LEVEL},	{80, 120},	0,		2,			0,                  BP_NO_INTERIOR_FLAG, {
		{DF_REMNANT, 0,			0,				{6, 8},		3,			0,			-1,			0,				1,				0,			0,			0},
		{0,			STATUE_INERT,DUNGEON,       {3, 5},		2,			0,			-1,			0,				1,				0,			0,			(MF_NOT_IN_HALLWAY | MF_TREAT_AS_BLOCKING)}}},
	// Dismal -- blood, bones, charcoal, some rubble
	{{1,DEEPEST_LEVEL},	{60, 70},	0,		3,			0,                  BP_NO_INTERIOR_FLAG, {
		{DF_AMBIENT_BLOOD, 0,	0,				{5,10},		3,			0,			-1,			0,				1,				0,			0,			MF_NOT_IN_HALLWAY},
		{DF_ASH,	0,			0,				{4, 8},		2,			0,			-1,			0,				1,				0,			0,			MF_NOT_IN_HALLWAY},
		{DF_BONES,	0,			0,				{3, 5},		2,			0,			-1,			0,				1,				0,			0,			MF_NOT_IN_HALLWAY}}},
	// Chasm catwalk -- narrow bridge over a chasm, possibly under fire from a turret or two
	{{1,DEEPEST_LEVEL-1},{40, 80},	0,		4,			0,                  (BP_REQUIRE_BLOCKING | BP_OPEN_INTERIOR | BP_NO_INTERIOR_FLAG), {
		{DF_CHASM_HOLE,	0,		0,				{80, 80},	1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{DF_CATWALK_BRIDGE,0,	0,				{0,0},		0,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)},
		{0,			MACHINE_TRIGGER_FLOOR, DUNGEON, {0,1},	0,			0,			0,			0,				1,				0,			0,			(MF_NEAR_ORIGIN | MF_PERMIT_BLOCKING)},
		{0,			TURRET_DORMANT,DUNGEON,		{1, 2},		1,			0,			-1,			0,				2,				HORDE_MACHINE_TURRET,0,	(MF_TREAT_AS_BLOCKING | MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_IN_VIEW_OF_ORIGIN)}}},
	// Lake walk -- narrow bridge of shallow water through a lake, possibly under fire from a turret or two
	{{1,DEEPEST_LEVEL},	{40, 80},	0,		3,			0,                  (BP_REQUIRE_BLOCKING | BP_OPEN_INTERIOR | BP_NO_INTERIOR_FLAG), {
		{DF_LAKE_CELL,	0,		0,				{80, 80},	1,			0,			-1,			0,				1,				0,			0,			(MF_TREAT_AS_BLOCKING | MF_REPEAT_UNTIL_NO_PROGRESS)},
		{0,			MACHINE_TRIGGER_FLOOR, DUNGEON, {0,1},	0,			0,			0,			0,				1,				0,			0,			(MF_NEAR_ORIGIN | MF_PERMIT_BLOCKING)},
		{0,			TURRET_DORMANT,DUNGEON,		{1, 2},		1,			0,			-1,			0,				2,				HORDE_MACHINE_TURRET,0,	(MF_TREAT_AS_BLOCKING | MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_IN_VIEW_OF_ORIGIN)}}},
    // Paralysis trap -- already-revealed pressure plate with a few hidden vents nearby.
    {{1,DEEPEST_LEVEL},	{35, 40},	0,		2,			0,                  (BP_NO_INTERIOR_FLAG), {
		{0,         GAS_TRAP_PARALYSIS, DUNGEON, {1,2},     1,          0,          0,			0,				3,				0,          0,			(MF_NEAR_ORIGIN | MF_NOT_IN_HALLWAY)},
		{0,			MACHINE_PARALYSIS_VENT_HIDDEN,DUNGEON,{3, 4},2,		0,			0,			0,				3,				0,          0,          (MF_FAR_FROM_ORIGIN | MF_NOT_IN_HALLWAY)}}},
    // Paralysis trap -- hidden pressure plate with a few vents nearby.
    {{1,DEEPEST_LEVEL},	{35, 40},	0,		2,			0,                  (BP_NO_INTERIOR_FLAG), {
		{0,         GAS_TRAP_PARALYSIS_HIDDEN, DUNGEON, {1,2},1,        0,          0,			0,				3,				0,          0,			(MF_NEAR_ORIGIN | MF_NOT_IN_HALLWAY)},
		{0,			MACHINE_PARALYSIS_VENT_HIDDEN,DUNGEON,{3, 4},2,		0,			0,			0,				3,				0,          0,          (MF_FAR_FROM_ORIGIN | MF_NOT_IN_HALLWAY)}}},
	// Statue comes alive -- innocent-looking statue that bursts to reveal a monster when the player approaches
	{{1,DEEPEST_LEVEL},	{5, 5},		0,		0,			0,                  (BP_NO_INTERIOR_FLAG), {
		{0,			STATUE_DORMANT,DUNGEON,		{1, 1},		1,			0,			-1,			0,				1,				HORDE_MACHINE_STATUE,0,	(MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_AT_ORIGIN | MF_ALTERNATIVE)},
		{0,			STATUE_DORMANT,DUNGEON,		{1, 1},		1,			0,			-1,			0,				1,				HORDE_MACHINE_STATUE,0,	(MF_GENERATE_HORDE | MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_ALTERNATIVE | MF_NOT_ON_LEVEL_PERIMETER)},
		{0,			MACHINE_TRIGGER_FLOOR,DUNGEON,{0,0},	2,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)}}},
	// Worms in the walls -- step on trigger region to cause underworms to burst out of the walls
	{{1,DEEPEST_LEVEL},	{7, 7},		0,		2,			0,                  (BP_NO_INTERIOR_FLAG), {
		{0,			WALL_MONSTER_DORMANT,DUNGEON,{1, 3},	1,			0,			-1,			MK_UNDERWORM,	1,				0,			0,			(MF_MONSTERS_DORMANT | MF_BUILD_IN_WALLS | MF_NOT_ON_LEVEL_PERIMETER)},
		{0,			MACHINE_TRIGGER_FLOOR,DUNGEON,{0,0},	2,			0,			-1,			0,				0,				0,			0,			(MF_EVERYWHERE)}}},
	// Sentinels
	{{1,DEEPEST_LEVEL},	{40, 40},	0,		2,			0,                  (BP_NO_INTERIOR_FLAG), {
		{0,			STATUE_DORMANT,DUNGEON,		{3, 3},		3,			0,			-1,			MK_SENTINEL,	2,				0,			0,			(MF_NOT_IN_HALLWAY | MF_TREAT_AS_BLOCKING | MF_IN_VIEW_OF_ORIGIN)},
		{DF_ASH,	0,			0,				{2, 3},		0,			0,			-1,			0,				0,				0,			0,			0}}},
};


// Monster definitions

// Defines all creatures, which include monsters and the player:
creatureType monsterCatalog[NUMBER_MONSTER_KINDS] = {
	//	name			ch		color			HP		def		acc		damage			reg	move	attack	blood			light	DFChance DFType         bolts       behaviorF, abilityF
	{0,	"you",	PLAYER_CHAR,	&playerInLightColor,30,	0,		100,	{1, 2, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MALE | MONST_FEMALE)},

    {0, "cute dog",		PET_DOG_CHAR,	&white,	13,		17,		80,		{1, 3, 1},		20,	95, 	100,	DF_RED_BLOOD,	0,		1,		DF_URINE,       {0}, (MONST_FEMALE | MONST_MALE | MONST_NO_POLYMORPH)},
	{0,	"adventurer",	PLAYER_CHAR,	&white,30,	0,		100,	{1, 2, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MALE | MONST_FEMALE | MONST_NO_POLYMORPH | MONST_CARRY_ITEM_100 | MONST_DISTANT_FOLLOWER | MONST_WILL_NOT_USE_STAIRS | MONST_CAST_SPELLS_SLOWLY)},

	{0, "rat",			'r',	&gray,			6,		0,		80,		{1, 3, 1},		20,	90,	100,	DF_RED_BLOOD,	0,		1,		DF_URINE,       {0}},
	{0, "kobold",		'k',	&goblinColor,	7,		0,		80,		{1, 4, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0}},
	{0,	"jackal",		'j',	&jackalColor,	8,		0,		70,		{2, 4, 1},		20,	50,		100,	DF_RED_BLOOD,	0,		1,		DF_URINE,              {0}},
	{0,	"eel",			'e',	&eelColor,		18,		27,		100,	{3, 7, 2},		5,	50,		100,	0,              0,		0,		0,              {0},
		(MONST_RESTRICTED_TO_LIQUID | MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_FLITS | MONST_NEVER_SLEEPS)},
	{0,	"monkey",		'm',	&ogreColor,		12,		17,		100,	{1, 3, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		1,		DF_URINE,       {0},
		(0), (MA_HIT_STEAL_FLEE)},
	{0, "bloat",		'b',	&poisonGasColor,4,		0,		100,	{0, 0, 0},		5,	100,	100,	DF_PURPLE_BLOOD,0,		0,		DF_BLOAT_DEATH, {0},
		(MONST_FLIES | MONST_FLITS), (MA_KAMIKAZE | MA_DF_ON_DEATH)},
	{0, "pit bloat",	'b',	&lightBlue,     4,		0,		100,	{0, 0, 0},		5,	100,	100,	DF_PURPLE_BLOOD,0,		0,		DF_HOLE_POTION, {0},
		(MONST_FLIES | MONST_FLITS), (MA_KAMIKAZE | MA_DF_ON_DEATH)},
	{0, "goblin",		'g',	&goblinColor,	15,		10,		70,		{2, 5, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
        (0),  (MA_ATTACKS_PENETRATE | MA_AVOID_CORRIDORS)},
	{0, "goblin conjurer",'g',	&goblinConjurerColor, 10,10,	70,		{2, 4, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MAINTAINS_DISTANCE | MONST_CAST_SPELLS_SLOWLY | MONST_CARRY_ITEM_25), (MA_CAST_SUMMON | MA_ATTACKS_PENETRATE | MA_AVOID_CORRIDORS)},
	{0, "goblin mystic",'g',	&goblinMysticColor, 10,	10,		70,		{2, 4, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {BOLT_SHIELDING},
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25), (MA_AVOID_CORRIDORS)},

	{0, "goblin thief",	'g',	&darkGray,      12,		10,		70,		{2, 4, 1},		10,	110,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
        (MONST_NEVER_SLEEPS),  (MA_ATTACKS_PENETRATE | MA_AVOID_CORRIDORS | MA_HIT_STEAL_FLEE)},
	{0, "goblin driver",	'g',	&orange,	15,		10,		70,		{2, 3, 1},		10,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MAINTAINS_DISTANCE), (MA_ATTACKS_EXTEND | MA_AVOID_CORRIDORS)},
	{0, "goblin totem",	TOTEM_CHAR,	&orange,	30,		0,		0,		{0, 0, 0},		0,	100,	300,	DF_RUBBLE_BLOOD,IMP_LIGHT,0,	0,              {BOLT_HASTE, BOLT_SPARK},
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_WILL_NOT_USE_STAIRS), (0)},
	{0, "pink jelly",	'J',	&pinkJellyColor,50,		0,		110,     {1, 2, 1},		0,	120,	100,	DF_PURPLE_BLOOD,0,		0,		0,              {0},
		(MONST_NEVER_SLEEPS), (MA_CLONE_SELF_ON_DEFEND)},
	{0, "toad",			't',	&toadColor,		18,		0,		90,		{1, 4, 1},		10,	100,	100,	DF_GREEN_BLOOD,	0,		0,		0,              {0},
		(0), (MA_HIT_HALLUCINATE)},
	{0, "vampire bat",	'v',	&gray,			18,		25,		100,	{2, 6, 1},		20,	75,		100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_FLIES | MONST_FLITS), (MA_TRANSFERENCE)},
	{0, "glass golem",		'G',	&wallCrystalColor,		10,	0,     100,	{11, 20, 1},		0,	100,	100,	DF_DEWAR_GLASS,0,		0,		0,              {0},
		(MONST_REFLECT_4 | MONST_DIES_IF_NEGATED)},
	{0, "rope golem",	'G',	&tanColor,			30,	0,     100,	    {4, 12, 1},		0,	75,	100,	DF_ROPE,0,		0,		0,              {0},
		(MONST_DIES_IF_NEGATED | MONST_FLITS | MONST_CAST_SPELLS_SLOWLY | MONST_IMMUNE_TO_WEBS), {MA_ATTACKS_EXTEND}},
	{0, "arrow turret", TURRET_CHAR,    &arrowTurretColor,		30,		0,		90,		{2, 6, 1},		0,	100,	250,	0,              IMP_LIGHT,		0,		0,              {BOLT_DISTANCE_ATTACK},
		(MONST_TURRET), (0)},
	{0, "acid mound",	'a',	&acidBackColor,	15,		10,		70,		{1, 3, 1},		5,	110,	100,	DF_ACID_BLOOD,	0,		0,		0,              {0},
		(MONST_DEFEND_DEGRADE_WEAPON), (MA_HIT_DEGRADE_ARMOR)},
	{0, "centipede",	'c',	&centipedeColor,20,		20,		80,		{4, 12, 1},		20,	100,	100,	DF_GREEN_BLOOD,	0,		0,		0,              {0},
		(0), (MA_CAUSES_WEAKNESS)},
	{0,	"ogre",			'O',	&ogreColor,		55,		60,		125,	{9, 13, 2},		20,	110,	200,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MALE | MONST_FEMALE), (MA_AVOID_CORRIDORS)},

	{0, "lemmer",   'l',	&red,		20,		10,     90,	{1, 2, 1},		5,	100,	100,	DF_GREEN_BLOOD,	0,		0,		0, {0},
		(MONST_MALE | MONST_FLEES_NEAR_DEATH), (MA_HIT_BLINDS)},
    {0,	"ink eel",		'e',	&black,		18,		27,		100,	{3, 7, 2},		5,	50,		100,	0,              0,		0,		0,              {0},
		(MONST_RESTRICTED_TO_LIQUID | MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_FLITS | MONST_NEVER_SLEEPS), {MA_HIT_BLINDS}},
	{0, "magma eel",	'e',	&white, 20,	27,     700,	{8, 12, 1},		5,	75,	75,	DF_ASH_BLOOD,	SALAMANDER_LIGHT, 100, DF_SALAMANDER_FLAME, {0},
		(MONST_RESTRICTED_TO_LIQUID | MONST_IMMUNE_TO_FIRE | MONST_SUBMERGES | MONST_NEVER_SLEEPS | MONST_FIERY), {MA_ATTACKS_EXTEND} },
	{0, "ankheg",   'A',	&tanColor,		40,		60,     90,	{2, 5, 1},		5,	100,	125,	DF_GREEN_BLOOD,	0,		0,		0, {BOLT_ACID_TURRET_ATTACK},
		(MONST_CAST_SPELLS_SLOWLY | MONST_IMMUNE_TO_WATER | MONST_IMMUNE_TO_WEBS), (MA_HIT_DEGRADE_ARMOR)},

	{0,	"bog monster",	'B',	&krakenColor,	55,		60,		5000,	{3, 4, 1},		3,	200,	100,	0,              0,		0,		0,              {0},
		(MONST_RESTRICTED_TO_LIQUID | MONST_SUBMERGES | MONST_FLITS | MONST_FLEES_NEAR_DEATH), (MA_SEIZES)},
	{0, "ogre totem",	TOTEM_CHAR,	&green,		70,		0,		0,		{0, 0, 0},		0,	100,	400,	DF_RUBBLE_BLOOD,LICH_LIGHT,0,	0,              {BOLT_HEALING, BOLT_SLOW_2},
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_WILL_NOT_USE_STAIRS), (0)},
	{0, "spider",		's',	&white,			20,		70,		90,		{3, 4, 2},		20,	100,	200,	DF_GREEN_BLOOD,	0,		0,		0,              {BOLT_SPIDERWEB},
		(MONST_IMMUNE_TO_WEBS | MONST_CAST_SPELLS_SLOWLY | MONST_ALWAYS_USE_ABILITY), (MA_POISONS)},
	{0, "spark turret", TURRET_CHAR, &sparkTurretColor,80,0,		100,	{0, 0, 0},		0,	100,	150,	0,              SPARK_TURRET_LIGHT,	0,	0,      {BOLT_SPARK},
		(MONST_TURRET), (0)},
	{0,	"will-o-the-wisp",'w',	&wispLightColor,10,		90,     100,	{5,	8, 2},		5,	90,	100,	DF_ASH_BLOOD,	WISP_LIGHT,	0,	0,              {0},
		(MONST_IMMUNE_TO_FIRE | MONST_FLIES | MONST_FLITS | MONST_NEVER_SLEEPS | MONST_FIERY | MONST_DIES_IF_NEGATED)},
	{0, "wraith",		'W',	&wraithColor,	50,		60,		120,	{6, 13, 2},		5,	50,		100,	DF_GREEN_BLOOD,	0,		0,		0,              {0},
		(MONST_FLEES_NEAR_DEATH)},
	{0, "zombie",		'Z',	&vomitColor,	80,		0,		120,	{7, 12, 1},		0,	100,	100,	DF_ROT_GAS_BLOOD,0,		100,	DF_ROT_GAS_PUFF, {0}},
	{0, "troll",		'T',	&trollColor,	65,		70,		125,	{10, 15, 3},	1,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MALE | MONST_FEMALE)},
	{0,	"ogre shaman",	'O',	&green,			45,		40,		100,	{5, 9, 1},		20,	100,	200,	DF_RED_BLOOD,	0,		0,		0,              {BOLT_HASTE, BOLT_SPARK},
		(MONST_MAINTAINS_DISTANCE | MONST_CAST_SPELLS_SLOWLY | MONST_MALE | MONST_FEMALE), (MA_CAST_SUMMON | MA_AVOID_CORRIDORS)},
	{0, "naga",			'N',	&trollColor,	75,		70,     150,	{7, 11, 4},		10,	100,	100,	DF_GREEN_BLOOD,	0,		100,	DF_PUDDLE,      {0},
		(MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_NEVER_SLEEPS | MONST_FEMALE), (MA_ATTACKS_ALL_ADJACENT)},
	{0, "salamander",	'S',	&salamanderColor,60,	70,     150,	{5, 11, 3},		10,	100,	100,	DF_ASH_BLOOD,	SALAMANDER_LIGHT, 100, DF_SALAMANDER_FLAME, {0},
		(MONST_IMMUNE_TO_FIRE | MONST_SUBMERGES | MONST_NEVER_SLEEPS | MONST_FIERY | MONST_MALE), (MA_ATTACKS_EXTEND)},
	{0, "explosive bloat",'b',	&orange,		10,		0,		100,	{0, 0, 0},		5,	100,	100,	DF_RED_BLOOD,	EXPLOSIVE_BLOAT_LIGHT,0, DF_BLOAT_EXPLOSION, {0},
		(MONST_FLIES | MONST_FLITS), (MA_KAMIKAZE | MA_DF_ON_DEATH)},
	{0, "dar blademaster",'d',	&purple,		35,		70,     160,	{5, 9, 2},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {BOLT_BLINKING},
		(MONST_CARRY_ITEM_25 | MONST_MALE | MONST_FEMALE), (MA_AVOID_CORRIDORS)},
	{0, "dar priestess", 'd',	&darPriestessColor,20,	60,		100,	{2, 5, 1},		20,	100,	100,	DF_RED_BLOOD,   0,		0,		0,              {BOLT_NEGATION, BOLT_HEALING, BOLT_HASTE, BOLT_SPARK},
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25 | MONST_FEMALE), (MA_AVOID_CORRIDORS)},
	{0, "dar battlemage",'d',	&darMageColor,	20,		60,		100,	{1, 3, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {BOLT_FIRE, BOLT_SLOW_2, BOLT_DISCORD},
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25 | MONST_MALE | MONST_FEMALE), (MA_AVOID_CORRIDORS)},
	{0, "acidic jelly",	'J',	&acidBackColor,	60,		0,		115,	{2, 6, 1},		0,	100,	100,	DF_ACID_BLOOD,	0,		0,		0,              {0},
		(MONST_DEFEND_DEGRADE_WEAPON), (MA_HIT_DEGRADE_ARMOR | MA_CLONE_SELF_ON_DEFEND)},
	{0,	"centaur",		'C',	&tanColor,		35,		50,		175,	{4, 8, 2},		20,	50,		100,	DF_RED_BLOOD,	0,		0,		0,              {BOLT_DISTANCE_ATTACK},
		(MONST_MAINTAINS_DISTANCE | MONST_MALE), (0)},
	{0, "underworm",	'U',	&wormColor,		80,		40,		160,	{18, 22, 2},	3,	150,	200,	DF_WORM_BLOOD,	0,		0,		0,              {0},
        (MONST_NEVER_SLEEPS)},
	{0, "sentinel",		STATUE_CHAR, &sentinelColor, 50,0,		0,		{0, 0, 0},		0,	100,	175,	DF_RUBBLE_BLOOD,SENTINEL_LIGHT,0,0,             {BOLT_HEALING, BOLT_SPARK},
		(MONST_TURRET | MONST_CAST_SPELLS_SLOWLY | MONST_DIES_IF_NEGATED), (0)},
	{0, "acid turret", TURRET_CHAR,	&acidTurretColor,35,	0,		250,	{1, 2, 1},      0,	100,	250,	0,              LICH_LIGHT,		0,		0,              {BOLT_ACID_TURRET_ATTACK},
		(MONST_TURRET), (MA_HIT_DEGRADE_ARMOR)},
    {0, "dart turret", TURRET_CHAR,	&dartTurretColor,20,	0,		140,	{1, 2, 1},      0,	100,	250,	0,              IFRIT_LIGHT,		0,		0,              {BOLT_POISON_DART},
		(MONST_TURRET), (MA_CAUSES_WEAKNESS)},
	{0, "nymph",		'n',	&green,			15,		10,     100,	{1, 2, 1},		20,	90,	100,	DF_GREEN_BLOOD,	SUN_LIGHT,	0,	0,              {},
		(MONST_IMMUNE_TO_WATER | MONST_IMMUNE_TO_WEBS | MONST_FEMALE), (MA_AVOID_CORRIDORS | MA_TRANSFERENCE | MA_HIT_STEAL_FLEE)},
	{0, "quivering jelly",	'J',	&white,	60,		0,		115,	{4, 8, 1},		0,	100,	100,	DF_ACID_BLOOD,	0,		0,		0,              {0},
		(0), (MA_HIT_SLOWS | MA_CLONE_SELF_ON_DEFEND)},

	{0, "fire elemental",'E',	&salamanderColor,	40,		60,		100,	{8, 18, 1},		20,	100,	100, DF_GAS_FIRE,	EXPLOSIVE_BLOAT_LIGHT,0, DF_GAS_FIRE, {BOLT_SPARK, BOLT_LIGHT_SOURCE, BOLT_FIRE},
		(MONST_NO_POLYMORPH | MONST_FLEES_NEAR_DEATH | MONST_NEVER_SLEEPS | MONST_IMMUNE_TO_FIRE | MONST_CAST_SPELLS_SLOWLY | MONST_MAINTAINS_DISTANCE | MONST_FIERY | MONST_DOES_NOT_MUTATE), (MA_DF_ON_DEATH)},
	{0, "water elemental",'E',	&shallowWaterBackColor,	40,		60,		100,	{11, 18, 1},		75,	100,	400,	DF_SHALLOW_WATER_POOL,	0,		400,		DF_ROLLING_WATER,              {},
		(MONST_NO_POLYMORPH | MONST_FLITS | MONST_FLEES_NEAR_DEATH | MONST_NEVER_SLEEPS | MONST_IMMUNE_TO_WATER | MONST_IMMUNE_TO_FIRE | MONST_SUBMERGES | MONST_DOES_NOT_MUTATE), (MA_DF_ON_DEATH)},
	{0, "earth elemental",'E',	&brown,	120,	80,		150,	{15, 20, 1},		50,	150,	150,	DF_RUBBLE_BLOOD,	SENTINEL_LIGHT,		0,		DF_RUBBLE,              {0},
		(MONST_NO_POLYMORPH | MONST_NEVER_SLEEPS | MONST_REFLECT_4 | MONST_IMMUNE_TO_FIRE | MONST_DOES_NOT_MUTATE), (MA_SEIZES|MA_DF_ON_DEATH)},
	{0, "air elemental",'E',	&grayFungusColor,	40,		60,		100,	{10, 15, 1},		5,	70,	80,    0,	SWIRLING_WIND_LIGHT,		0,		DF_STEAM_PUFF,              {BOLT_BLINKING, BOLT_FORCE},
		(MONST_NO_POLYMORPH | MONST_FLEES_NEAR_DEATH | MONST_NEVER_SLEEPS | MONST_MAINTAINS_DISTANCE | MONST_FLITS | MONST_FLIES | MONST_IMMUNE_TO_WEBS | MONST_DOES_NOT_MUTATE), (MA_ATTACKS_ALL_ADJACENT | MA_DF_ON_DEATH)},

	{0,	"kraken",		'K',	&krakenColor,	120,	0,		150,	{15, 20, 3},	1,	50,		100,	0,              0,		0,		0,              {0},
		(MONST_RESTRICTED_TO_LIQUID | MONST_IMMUNE_TO_WATER | MONST_SUBMERGES | MONST_FLITS | MONST_NEVER_SLEEPS | MONST_FLEES_NEAR_DEATH), (MA_SEIZES)},
	{0,	"lich",			'L',	&white,			35,		80,     175,	{2, 6, 1},		0,	100,	100,	DF_ASH_BLOOD,	LICH_LIGHT,	0,	0,              {BOLT_FIRE},
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25 | MONST_NO_POLYMORPH), (MA_CAST_SUMMON)},
	{0, "phylactery",	GEM_CHAR,&lichLightColor,30,	0,		0,		{0, 0, 0},		0,	100,	150,	DF_RUBBLE_BLOOD,LICH_LIGHT,	0,	0,              {0},
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_ALWAYS_HUNTING | MONST_WILL_NOT_USE_STAIRS | MONST_DIES_IF_NEGATED), (MA_CAST_SUMMON | MA_ENTER_SUMMONS)},
	{0, "pixie",		'p',	&pixieColor,	10,		90,     100,	{1, 3, 1},		20,	50,		100,	DF_GREEN_BLOOD,	PIXIE_LIGHT, 0,	0,              {BOLT_NEGATION, BOLT_SLOW_2, BOLT_DISCORD, BOLT_SPARK},
		(MONST_MAINTAINS_DISTANCE | MONST_FLIES | MONST_FLITS | MONST_MALE | MONST_FEMALE), (0)},
	{0,	"phantom",		'P',	&ectoplasmColor,35,		70,     160,	{12, 18, 4},    0,	50,		200,	DF_ECTOPLASM_BLOOD,	0,	2,		DF_ECTOPLASM_DROPLET, {0},
		(MONST_INVISIBLE | MONST_FLITS | MONST_FLIES | MONST_IMMUNE_TO_WEBS)},
	{0, "flame turret", TURRET_CHAR, &flameTurretColor,40,	0,		150,	{1, 2, 1},		0,	100,	250,	0,              LAVA_LIGHT,	0,	0,              {BOLT_FIRE},
		(MONST_TURRET), (0)},
	{0, "imp",			'i',	&pink,			35,		90,     225,	{4, 9, 2},		10,	100,	100,	DF_GREEN_BLOOD,	IMP_LIGHT,	0,	0,              {BOLT_BLINKING},
		(0), (MA_HIT_STEAL_FLEE)},
	{0,	"fury",			'f',	&darkRed,		19,		90,     200,	{6, 11, 4},		20,	50,		100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_NEVER_SLEEPS | MONST_FLIES)},
	{0, "revenant",		'R',	&ectoplasmColor,30,		0,		200,	{15, 20, 5},	0,	100,	100,	DF_ECTOPLASM_BLOOD,	0,	0,		0,              {0},
		(MONST_IMMUNE_TO_WEAPONS)},
	{0, "tentacle horror",'H',	&centipedeColor,120,	95,     225,	{25, 35, 3},	1,	100,	100,	DF_PURPLE_BLOOD,0,		0,		0,              {0}},
	{0, "stone golem",	'G',	&gray,			400,	70,     225,	{4, 8, 1},		0,	100,	100,	DF_RUBBLE_BLOOD,0,		0,		0,              {0},
		(MONST_REFLECT_4 | MONST_DIES_IF_NEGATED)},
	{0, "dragon",		'D',	&dragonColor,	150,	90,     250,	{25, 50, 4},	20,	50,		200,	DF_GREEN_BLOOD,	0,		0,		0,              {BOLT_DRAGONFIRE},
		(MONST_IMMUNE_TO_FIRE | MONST_CARRY_ITEM_100), (MA_ATTACKS_ALL_ADJACENT)},

	{0, "force totem",	TOTEM_CHAR,	&gray,		70,		0,		0,		{0, 0, 0},		0,	100,	400,	DF_RUBBLE_BLOOD,LICH_LIGHT,0,	0,              {BOLT_FORCE},
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_ALWAYS_HUNTING | MONST_WILL_NOT_USE_STAIRS | MONST_IMMUNE_TO_FIRE), (0)},

	// bosses
	{0, "goblin warlord",'g',	&blue,			30,		17,		100,	{3, 6, 1},		20,	100,	100,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MAINTAINS_DISTANCE | MONST_CARRY_ITEM_25), (MA_CAST_SUMMON | MA_ATTACKS_PENETRATE | MA_AVOID_CORRIDORS)},
	{0,	"black jelly",	'J',	&black,			120,	0,		130,	{3, 8, 1},		0,	100,	100,	DF_PURPLE_BLOOD,0,		0,		0,              {0},
		(0), (MA_HIT_SLOWS|MA_CLONE_SELF_ON_DEFEND)},
	{0, "vampire",		'V',	&white,			75,		60,     120,	{4, 15, 2},		6,	50,		100,	DF_RED_BLOOD,	0,		0,		DF_BLOOD_EXPLOSION, {BOLT_BLINKING, BOLT_DISCORD},
		(MONST_FLEES_NEAR_DEATH | MONST_MALE), (MA_TRANSFERENCE | MA_DF_ON_DEATH | MA_CAST_SUMMON | MA_ENTER_SUMMONS)},
	{0, "flamedancer",	'F',	&white,			65,		80,     120,	{3, 8, 2},		0,	100,	100,	DF_EMBER_BLOOD,	FLAMEDANCER_LIGHT,100,DF_FLAMEDANCER_CORONA, {BOLT_FIRE},
		(MONST_MAINTAINS_DISTANCE | MONST_IMMUNE_TO_FIRE | MONST_FIERY), (0)},

	// special effect monsters
	{0, "spectral blade",WEAPON_CHAR, &spectralBladeColor,1, 0,	150,	{1, 1, 1},		0,	50,		100,	0,              SPECTRAL_BLADE_LIGHT,0,0,       {0},
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_FLIES | MONST_WILL_NOT_USE_STAIRS | MB_DOES_NOT_TRACK_LEADER | MONST_DIES_IF_NEGATED | MONST_IMMUNE_TO_WEBS | MONST_NOT_LISTED_IN_SIDEBAR)},
	{0, "spectral sword",WEAPON_CHAR, &spectralImageColor, 1,0,	150,	{1, 1, 1},		0,	50,		100,	0,              SPECTRAL_IMAGE_LIGHT,0,0,       {0},
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_FLIES | MONST_WILL_NOT_USE_STAIRS | MB_DOES_NOT_TRACK_LEADER | MONST_DIES_IF_NEGATED | MONST_IMMUNE_TO_WEBS)},
    {0, "stone guardian",STATUE_CHAR, &white,   1000,   0,		200,	{12, 17, 2},	0,	100,	100,	DF_RUBBLE,      0,      100,      DF_GUARDIAN_STEP, {0},
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_ALWAYS_HUNTING | MONST_IMMUNE_TO_FIRE | MONST_IMMUNE_TO_WEAPONS | MONST_WILL_NOT_USE_STAIRS | MONST_DIES_IF_NEGATED | MONST_REFLECT_4 | MONST_ALWAYS_USE_ABILITY | MONST_GETS_TURN_ON_ACTIVATION)},
    {0, "winged guardian",STATUE_CHAR, &blue,   1000,   0,		200,	{12, 17, 2},	0,	100,	100,	DF_RUBBLE,      0,      100,      DF_SILENT_GLYPH_GLOW, {BOLT_BLINKING},
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_ALWAYS_HUNTING | MONST_IMMUNE_TO_FIRE | MONST_IMMUNE_TO_WEAPONS | MONST_WILL_NOT_USE_STAIRS | MONST_DIES_IF_NEGATED | MONST_REFLECT_4 | MONST_GETS_TURN_ON_ACTIVATION | MONST_ALWAYS_USE_ABILITY), (0)},
    {0, "guardian spirit",STATUE_CHAR, &spectralImageColor,1000,0,200,	{5, 12, 2},     0,	100,	100,	0,              SPECTRAL_IMAGE_LIGHT,100,0,     {0},
		(MONST_INANIMATE | MONST_NEVER_SLEEPS | MONST_IMMUNE_TO_FIRE | MONST_IMMUNE_TO_WEAPONS | MONST_DIES_IF_NEGATED | MONST_REFLECT_4 | MONST_ALWAYS_USE_ABILITY)},
    {0, "Warden of Yendor",'Y', &yendorLightColor,1000,   0,	300,	{12, 17, 2},	0,	200,	200,	DF_RUBBLE,      YENDOR_LIGHT, 100, 0,           {0},
		(MONST_NEVER_SLEEPS | MONST_ALWAYS_HUNTING | MONST_INVULNERABLE | MONST_NO_POLYMORPH)},
    {0, "eldritch totem",TOTEM_CHAR, &glyphColor,80,    0,		0,		{0, 0, 0},		0,	100,	100,	DF_RUBBLE_BLOOD,0,      0,      0,              {0},
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_ALWAYS_HUNTING | MONST_WILL_NOT_USE_STAIRS | MONST_GETS_TURN_ON_ACTIVATION | MONST_ALWAYS_USE_ABILITY), (MA_CAST_SUMMON)},
    {0, "mirrored totem",TOTEM_CHAR, &beckonColor,80,	0,		0,		{0, 0, 0},		0,	100,	100,	DF_RUBBLE_BLOOD,0,      100,	DF_MIRROR_TOTEM_STEP, {BOLT_BECKONING},
		(MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_ALWAYS_HUNTING | MONST_WILL_NOT_USE_STAIRS | MONST_GETS_TURN_ON_ACTIVATION | MONST_ALWAYS_USE_ABILITY | MONST_REFLECT_4 | MONST_IMMUNE_TO_WEAPONS | MONST_IMMUNE_TO_FIRE), (0)},

	// legendary "allies"
	{0,	"unicorn",		UNICORN_CHAR, &white,   40,		60,		175,	{2, 10, 2},		20,	50,		100,	DF_RED_BLOOD,	UNICORN_LIGHT,1,DF_UNICORN_POOP, {BOLT_HEALING, BOLT_SHIELDING},
		(MONST_MAINTAINS_DISTANCE | MONST_MALE | MONST_FEMALE), (0)},
	{0,	"ifrit",		'I',	&ifritColor,	40,		75,     175,	{5, 13, 2},		1,	50,		100,	DF_ASH_BLOOD,	IFRIT_LIGHT,0,	0,              {BOLT_DISCORD},
		(MONST_IMMUNE_TO_FIRE | MONST_FLIES | MONST_MALE), (0)},
	{0,	"phoenix",		'P',	&phoenixColor,	30,		70,     175,	{4, 10, 2},		0,	50,		100,	DF_ASH_BLOOD,	PHOENIX_LIGHT,0,0,              {0},
		(MONST_IMMUNE_TO_FIRE| MONST_FLIES)},
	{0, "phoenix egg",	GEM_CHAR,&phoenixColor,	150,	0,		0,		{0, 0, 0},		0,	100,	150,	DF_ASH_BLOOD,	PHOENIX_EGG_LIGHT,	0,	0,      {0},
		(MONST_IMMUNE_TO_FIRE| MONST_IMMUNE_TO_WEBS | MONST_NEVER_SLEEPS | MONST_IMMOBILE | MONST_INANIMATE | MONST_WILL_NOT_USE_STAIRS | MONST_NO_POLYMORPH | MONST_ALWAYS_HUNTING), (MA_CAST_SUMMON | MA_ENTER_SUMMONS)},
    {0,	"mangrove dryad",'M',	&tanColor,      70,		60,     175,	{2, 8, 2},		6,	100,	100,	DF_ASH_BLOOD,	0,      0,      0,              {BOLT_ANCIENT_SPIRIT_VINES},
		(MONST_IMMUNE_TO_WEBS | MONST_ALWAYS_USE_ABILITY | MONST_MAINTAINS_DISTANCE | MONST_MALE | MONST_FEMALE), (0)},
    {0,	"soul of Hadrian",	PLAYER_CHAR,	&turquoise,80,	60,		100,	{6, 16, 1},		20,	100,	50,	DF_RED_BLOOD,	0,		0,		0,              {0},
		(MONST_MALE | MONST_NO_POLYMORPH | MONST_CAST_SPELLS_SLOWLY | MONST_CARRY_ITEM_100), {MA_ATTACKS_PENETRATE}}, // unused, i think

    // "post-game" new baddies -- gsr
    //	name			ch		color			HP		def		acc		damage			reg	move	attack	blood			light	DFChance DFType         bolts       behaviorF, abilityF
	{0, "black dragon",	'D',	&darkGray,	200,	100,     250,	{12, 15, 4},	20,	70,		200,	DF_ECTOPLASM_BLOOD,	PHOENIX_LIGHT,		0,		0,              {BOLT_POISON_BREATH},
		(MONST_IMMUNE_TO_FIRE | MONST_CARRY_ITEM_100), (MA_ATTACKS_ALL_ADJACENT)},
	{0, "paralytic bloat",	'b',	&pink,     4,		0,		100,	{0, 0, 0},		5,	120,	100,	DF_RED_BLOOD,0,		0,		DF_PARALYSIS_GAS_CLOUD_POTION, {0},
		(MONST_FLIES | MONST_FLITS), (MA_KAMIKAZE | MA_DF_ON_DEATH)},
	{0, "crystal golem",		'G',	&forceFieldColor,			600,	100,     225,	{8, 16, 1},		0,	175,	175,	DF_DEWAR_GLASS,CRYSTAL_WALL_LIGHT,		0,		0,              {0},
		(MONST_REFLECT_4 | MONST_DIES_IF_NEGATED | MONST_IMMUNE_TO_FIRE)},
	{0, "quickling",			'q',	&pink,			40,		20,     225,	{11, 13, 1},		10,	25,	75,	DF_RED_BLOOD,	0,	0,	0,              {0},
		(MONST_MALE | MONST_FEMALE), (0)},
	{0, "shadow centipede",	'c',	&darkGray,30,		20,		100,		{5, 10, 3},		20,	100,	100,	0,	PHOENIX_LIGHT,		0,		0,              {0},
		(0), (MA_CAUSES_WEAKNESS)},
	{0, "dar apparition", 'd',	&apparitionColor,	20,		60,		100,	{5, 7, 9},		20,	100,	100,	DF_RED_BLOOD,	PHOENIX_LIGHT,		0,		0,              {BOLT_FIRE, BOLT_SLOW_2, BOLT_SPARK, BOLT_BECKONING},
		(MONST_CARRY_ITEM_100 | MONST_MALE | MONST_FEMALE), (MA_AVOID_CORRIDORS)},
	{0,	"rotting hound",	PET_DOG_CHAR,	&vomitColor,	35,	  0,	75,		{1, 11, 1},		20,	50,		100,	0,	0,		1,		0,              {0}},
    {0,	"nether jelly",	'J',	&ectoplasmColor,			50,	0,		130,	{2, 3, 1},		0,	150,	150,	DF_ECTOPLASM_BLOOD,0,		0,		DF_ECTOPLASM_DROPLET,              {0},
		(MONST_INVISIBLE), (MA_CLONE_SELF_ON_DEFEND)},

    {0, "demon-god Moloch",MOLOCH_CHAR, &salamanderColor,  100,   0,	100,	{14, 20, 1},	20,	50,	200,	DF_RUBBLE,      EXPLOSIVE_BLOAT_LIGHT, 100, 0,           {BOLT_DRAGONFIRE, BOLT_BLINKING, BOLT_SPARK, BOLT_BECKONING},
        (MONST_NEVER_SLEEPS | MONST_FIERY | MONST_FLIES | MONST_ALWAYS_HUNTING | MONST_IMMUNE_TO_FIRE | MONST_NO_POLYMORPH | MONST_INVULNERABLE | MONST_WILL_NOT_USE_STAIRS), (MA_CAST_SUMMON)},
};

// Monster words

const monsterWords monsterText[NUMBER_MONSTER_KINDS] = {
	{"A naked adventurer in an unforgiving place, bereft of equipment and confused about the circumstances.",
		"studying", "Studying",
		{"hit", {0}}},
	{"This ragged but optimistic pup looks at you with cheery eyes. As a rescue dog, $HESHE has already seen $HISHER fair share of adventure.",
		"tearing at", "Eating",
		{"claws", "bites", "scratches", {0}}},
	{"A fellow adventurer has made $HISHER way into the dungeon -- and seems to have made $HIMSELFHERSELF comfortable on this floor. Toting $HISHER own load of equipment, $HESHE looks at you and nods in recognition.",
		"studying", "Studying",
		{"hit", {0}}},


	{"The rat is a scavenger of the shallows, perpetually in search of decaying animal matter.",
		"gnawing at", "Eating",
		{"scratches", "bites", {0}}},
	{"The kobold is a lizardlike humanoid of the upper dungeon.",
		"poking at", "Examining",
		{"clubs", "bashes", {0}}},
	{"The jackal prowls the caverns for intruders to rend with $HISHER powerful jaws.",
		"tearing at", "Eating",
		{"claws", "bites", "mauls", {0}}},
	{"The eel slips silently through the subterranean lake, waiting for unsuspecting prey to set foot in $HISHER dark waters.",
		"eating", "Eating",
		{"shocks", "bites", {0}}},
	{"Mischievous trickster that $HESHE is, the monkey lives to steal shiny trinkets from passing adventurers.",
		"examining", "Examining",
		{"tweaks", "bites", "punches", {0}}},
	{"A bladder of deadly gas buoys the bloat through the air, $HISHER thin veinous membrane ready to rupture at the slightest stress.",
		"gazing at", "Gazing",
		{"bumps", {0}},
		"bursts, leaving behind an expanding cloud of caustic gas!"},
	{"This rare subspecies of bloat is filled with a peculiar vapor that, if released, will cause the floor to vanish out from underneath $HIMHER.",
		"gazing at", "Gazing",
		{"bumps", {0}},
		"bursts, causing the floor underneath $HIMHER to disappear!"},
	{"A filthy little primate, the tribalistic goblin often travels in packs and carries a makeshift stone spear.",
		"chanting over", "Chanting",
		{"cuts", "stabs", "skewers", {0}}},
	{"This goblin is covered with glowing sigils that pulse with power. $HESHE can call into existence phantom blades to attack $HISHER foes.",
		"performing a ritual on", "Performing ritual",
		{"thumps", "whacks", "wallops", {0}},
		{0},
		"gestures ominously!"},
	{"This goblin carries no weapon, and $HISHER eyes sparkle with golden light. $HESHE can invoke a powerful shielding magic to protect $HISHER escorts from harm.",
		"performing a ritual on", "Performing ritual",
		{"slaps", "punches", "kicks", {0}}},
	{"Much less tribal in nature than others, this goblin relies on $HISHER own cunning and agility to relive adventurers of their treasures. $HESHE wields a stone arrowhead as a makeshift dagger.",
		"studying", "Studying",
		{"stabs", "cuts", {0}}},
	{"This goblin uses $HISHER crude leather whip to control $HISHER tribe's livestock and humanoid slaves. When used as a weapon, the whip allows $HIMHER to maintain a safe distance while whittling away at $HISHER foes.",
		"chanting over", "Chanting",
		{"lashes", "whips", {0}}},
	{"Goblins have created this makeshift totem and imbued $HIMHER with a shamanistic power.",
		"gazing at", "Gazing",
		{"hits", {0}}},
	{"This mass of caustic pink goo slips across the ground in search of a warm meal.",
		"absorbing", "Feeding",
		{"smears", "slimes", "drenches"}},
	{"The enormous, warty toad secretes a powerful hallucinogenic slime to befuddle the senses of any creatures that come in contact with $HIMHER.",
		"eating", "Eating",
		{"slimes", "slams", {0}}},
	{"Often hunting in packs, leathery wings and keen senses guide the vampire bat unerringly to $HISHER prey.",
		"draining", "Feeding",
		{"nips", "bites", {0}}},
	{"Jagged pieces of glass have been loosely bound together by magic into a humanoid shape and animated for purposes unknown. The glass golem's physical structure may be fragile, but it is entirely composed of glass blades and serrations.",
		"cradling", "Cradling",
		{"cuts", "slices", "claws", {0}}},
	{"This strange humanoid construct is made entirely of thick strands of rope, essentially equipped with a series of whips and entanglements at $HISHER fingertips. $HISHER frayed ends and sauntering movements give $HIMHER a worn, erratic demeanor.",
		"cradling", "Cradling",
		{"whips", "strikes", "lashes", {0}}},
	{"A mechanical contraption embedded in the wall, the spring-loaded arrow turret will fire volley after volley of arrows at intruders.",
		"gazing at", "Gazing",
		{"shoots", {0}}},
	{"The acid mound squelches softly across the ground, leaving a trail of acidic goo in $HISHER path.",
		"liquefying", "Feeding",
		{"slimes", "douses", "drenches", {0}}},
	{"This monstrous centipede's incisors are imbued with a horrible venom that will slowly kill $HISHER prey.",
		"eating", "Eating",
		{"pricks", "stings", {0}}},
	{"This lumbering creature carries an enormous club that $HESHE can swing with incredible force.",
		"examining", "Studying",
		{"cudgels", "clubs", "batters", {0}}},
	{"Armed by nature with claws coated in an eye-stinging substance, this imp-like creature fights defensively but decisively.",
		"studying", "Studying",
		{"claws", "scratches", "grazes", {0}}},

	{"This eel traverses the deep waters. $HISHER ink-based defense mechanisms can temporarily blind an adventurer.",
		"eating", "Eating",
		{"slimes", "douses", {0}}},
	{"This rare type of eel dwells in lava, infrequently emerging to unleash $HISHER fiery rage on potential intruders from a distance.",
		"eating", "Eating",
		{"burns", "singes", {0}}},
	{"The ankheg is a large insect-like creature is known for burrowing through walls and spitting streams of acid at $HISHER prey.",
		"melting", "Melting",
		{"drenches", "burns", {0}}},

	{"The horrifying bog monster dwells beneath the surface of mud-filled swamps. When $HISHER prey ventures into the mud, the bog monster will ensnare the unsuspecting victim in $HISHER pale tentacles and squeeze its life away.",
		"draining", "Feeding",
		{"squeezes", "strangles", "crushes", {0}}},
	{"Ancient ogres versed in the eldritch arts have assembled this totem and imbued $HIMHER with occult power.",
		"gazing at", "Gazing",
		{"hits", {0}}},
	{"The spider's red eyes pierce the darkness in search of enemies to ensnare with $HISHER projectile webs and dissolve with deadly poison.",
		"draining", "Feeding",
		{"bites", "stings", {0}}},
	{"This contraption hums with electrical charge that $HISHER embedded crystals and magical sigils can direct at intruders in deadly arcs.",
		"gazing at", "Gazing",
		{"shocks", {0}}},
	{"An ethereal blue flame dances through the air, flickering and pulsing in time to an otherworldly rhythm.",
		"consuming", "Feeding",
		{"scorches", "burns", {0}}},
	{"The wraith's hollow eye sockets stare hungrily at the world from $HISHER emaciated frame, and $HISHER long, bloodstained nails grope ceaselessly at the air for a fresh victim.",
		"devouring", "Feeding",
		{"clutches", "claws", "bites", {0}}},
	{"The zombie is the accursed product of a long-forgotten ritual. Perpetually decaying flesh, hanging from $HISHER bones in shreds, releases a putrid and flammable stench that will induce violent nausea with one whiff.",
		"rending", "Eating",
		{"hits", "bites", {0}}},
	{"An enormous, disfigured creature covered in phlegm and warts, the troll regenerates very quickly and attacks with astonishing strength. Many adventures have ended at $HISHER misshapen hands.",
		"eating", "Eating",
		{"cudgels", "clubs", "bludgeons", "pummels", "batters"}},
	{"This ogre is bent with age, but what $HESHE has lost in physical strength, $HESHE has more than gained in occult power.",
		"performing a ritual on", "Performing ritual",
		{"cudgels", "clubs", {0}},
		{0},
		"chants in a harsh, guttural tongue!"},
	{"The serpentine naga live beneath the subterranean waters and emerge to attack unsuspecting adventurers.",
		"studying", "Studying",
		{"claws", "bites", "tail-whips", {0}}},
	{"A serpent wreathed in flames and carrying a burning lash, salamanders dwell in lakes of fire and emerge when they sense a nearby victim, leaving behind a trail of glowing embers.",
		"studying", "Studying",
		{"whips", "lashes", {0}}},
	{"This rare subspecies of bloat is little more than a thin membrane surrounding a bladder of highly explosive gases. The slightest stress will cause $HIMHER to rupture in spectacular and deadly fashion.",
		"gazing at", "Gazing",
		{"bumps", {0}},
		"detonates with terrifying force!"},
	{"An elf of the deep, the dar blademaster leaps toward $HISHER enemies with frightening speed to engage in deadly swordplay.",
		"studying", "Studying",
		{"grazes", "cuts", "slices", "slashes", "stabs"}},
	{"The dar priestess carries a host of religious relics that jangle as $HESHE walks.",
		"praying over", "Praying",
		{"cuts", "slices", {0}}},
	{"The dar battlemage's eyes glow like embers and $HISHER hands radiate an occult heat.",
		"transmuting", "Transmuting",
		{"cuts", {0}}},
	{"A jelly subsisting on a diet of acid mounds will eventually express the characteristics of $HISHER prey, corroding any weapons or armor that come in contact with $HIMHER.",
		"transmuting", "Transmuting",
		{"burns", {0}}},
	{"Half man and half horse, the centaur is an expert with the bow and arrow -- hunter and steed fused into a single creature.",
		"studying", "Studying",
		{"shoots", {0}}},
	{"A strange and horrifying creature of the earth's deepest places, larger than an ogre but capable of squeezing through tiny openings. When hungry, the underworm will burrow behind the walls of a cavern and lurk dormant and motionless -- often for months -- until $HESHE can feel the telltale vibrations of nearby prey.",
		"consuming", "Consuming",
		{"slams", "bites", "tail-whips", {0}}},
	{"An ancient statue of an unrecognizable humanoid figure, the sentinel holds aloft a crystal that gleams with ancient warding magic. Sentinels are always found in groups, and each will attempt to repair any damage done to the others.",
		"focusing on", "Focusing",
		{"hits", {0}}},
	{"A green-flecked nozzle is embedded in the wall, ready to spew a stream of corrosive acid at intruders.",
		"gazing at", "Gazing",
		{"douses", "drenches", {0}}},
	{"This spring-loaded contraption fires darts that are imbued with a strength-sapping poison.",
		"gazing at", "Gazing",
		{"pricks", {0}}},
	{"A spirit of nature who tends to the scant flora found within the dungeon, $HESHE takes on the appearance of a young maiden and waits to lure adventurers away from their possessions.",
		"draining", "Draining",
		{"slaps", "pokes", "drains", {0}}},
	{"This subspecies of jelly thrives in the dungeons with a slowing touch, leaving $HISHER prey helpless and ready to devour.",
		"transmuting", "Transmuting",
		{"douses","slimes","touches", {0}}},
	{"Almost shining too bright to observe directly, the everburning fire elemental leaves ash, char, and destruction wherever $HESHE moves.",
		"incinerating", "Incinerating",
		{"scalds","singes","scorches","chars", {0}}},
	{"A deep but hazily-defined pool of water creeps along the dungeon floor. As a liquid, the water elemental uses trickery and deception to circle around and attack $HISHER prey.",
		"absorbing", "Absorbing",
		{"smacks","douses","drenches", {0}}},
	{"This massive, stoic creature is the embodiment of earth itself. The earth elemental moves at glacial speeds but is rock-solid. If $HESHE is able to catch up to $HISHER prey, $HESHE will grapple then ruthlessly bludgeon $HISHER prey until it stops breathing.",
		"eating", "Eating",
		{"crushes","punches","slams","squeezes", {0}}},
	{"The ethereal air elemental roughly takes the form of a swirling column of air with subtle anthropomorphic features, circulating dust and jagged debris all around $HIM for good measure. $HISHER power lies in $HISHER ability to move swiftly and to blow torrents of forceful winds at $HISHER opponents.",
		"eroding", "Eroding",
		{"scrapes","cuts","minces","grinds", {0}}},

	{"This tentacled nightmare will emerge from the subterranean waters to ensnare and devour any creature foolish enough to set foot into $HISHER lake.",
		"devouring", "Feeding",
		{"slaps", "smites", "batters", {0}}},
	{"The desiccated form of an ancient sorcerer, animated by dark arts and lust for power, commands the obedience of the infernal planes. $HISHER essence is anchored to reality by a phylactery that is always in $HISHER possession, and the lich cannot die unless $HISHER phylactery is destroyed.",
		"enchanting", "Enchanting",
		{"touches", {0}},
		{0},
		"rasps a terrifying incantation!"},
	{"This gem was the fulcrum of a dark rite, performed centuries ago, that bound the soul of an ancient and terrible sorcerer. Hurry and destroy the gem, before the lich can gather its power and regenerate its corporeal form!",
		"enchanting", "Enchanting",
		{"touches", {0}},
		{0},
		"swirls with dark sorcery as the lich regenerates its form!"},
	{"A tiny humanoid sparkles in the gloom, the hum of $HISHER beating wings punctuated by intermittent peals of high-pitched laughter. What $HESHE lacks in physical endurance, $HESHE makes up for with $HISHER wealth of mischievous magical abilities.",
		"sprinkling dust on", "Dusting",
		{"pokes", {0}}},
	{"A silhouette of mournful rage against an empty backdrop, the phantom slips through the dungeon invisibly in clear air, leaving behind glowing droplets of ectoplasm and the cries of $HISHER unsuspecting victims.",
		"permeating", "Permeating",
		{"hits", {0}}},
	{"This infernal contraption spits blasts of flame at intruders.",
		"incinerating", "Incinerating",
		{"pricks", {0}}},
	{"This trickster demon moves with astonishing speed and delights in stealing from $HISHER enemies and blinking away.",
		"dissecting", "Dissecting",
		{"slices", "cuts", {0}}},
	{"A creature of inchoate rage made flesh, the fury's moist wings beat loudly in the darkness.",
		"flagellating", "Flagellating",
		{"drubs", "fustigates", "castigates", {0}}},
	{"This unholy specter stalks the deep places of the earth without fear, impervious to conventional attacks.",
		"desecrating", "Desecrating",
		{"hits", {0}}},
	{"This seething, towering nightmare of fleshy tentacles slinks through the bowels of the world. The tentacle horror's incredible strength and regeneration make $HIMHER one of the most fearsome creatures of the dungeon.",
		"sucking on", "Consuming",
		{"slaps", "batters", "crushes", {0}}},
	{"A statue animated by an ancient and tireless magic, the golem does not regenerate and attacks with only moderate strength, but $HISHER stone form can withstand incredible damage before collapsing into rubble.",
		"cradling", "Cradling",
		{"backhands", "punches", "kicks", {0}}},
	{"An ancient serpent of the world's deepest places, the dragon's immense form belies its lightning-quick speed and testifies to $HISHER breathtaking strength. An undying furnace of white-hot flames burns within $HISHER scaly hide, and few could withstand a single moment under $HISHER infernal lash.",
		"consuming", "Consuming",
		{"claws", "tail-whips", "bites", {0}}},

	{"This monolithic totem is minimalist in design; $HESHE is simply thick pole with a diamond adorning affixed on top. $HESHE vibrates constantly but subtly.",
		"gazing at", "Gazing",
		{"hits", {0}}},

	{"Taller, stronger and smarter than other goblins, the warlord commands the loyalty of $HISHER kind and can summon them into battle.",
		"chanting over", "Chanting",
		{"slashes", "cuts", "stabs", "skewers", {0}},
		{0},
		"lets loose a deafening war cry!"},
	{"This blob of jet-black goo is as rare as $HESHE is deadly. Few creatures of the dungeon can withstand $HISHER caustic assault. Beware.",
		"absorbing", "Feeding",
		{"smears", "slimes", "drenches"}},
	{"This vampire lives a solitary life deep underground, consuming any warm-blooded creature unfortunate to venture near $HISHER lair.",
		"draining", "Drinking",
		{"grazes", "bites", "buries $HISHER fangs in", {0}},
		{0},
		"spreads his cloak and bursts into a cloud of bats!"},
	{"An elemental creature from another plane of existence, the infernal flamedancer burns with such intensity that $HESHE is painful to behold.",
		"immolating", "Consuming",
		{"singes", "burns", "immolates", {0}}},

	{"Eldritch forces have coalesced to form this flickering, ethereal weapon.",
		"gazing at", "Gazing",
		{"nicks",  {0}}},
	{"Eldritch energies bound up in your armor have leapt forth to project this spectral image.",
		"gazing at", "Gazing",
		{"hits",  {0}}},
	{"Guarding the room is a weathered stone statue of a knight carrying a battleaxe, connected to the glowing glyphs on the floor by invisible strands of enchantment.",
		"gazing at", "Gazing",
		{"strikes",  {0}}},
	{"A statue of a sword-wielding angel surveys the room, connected to the glowing glyphs on the floor by invisible strands of enchantment.",
		"gazing at", "Gazing",
		{"strikes",  {0}}},
	{"A spectral outline of a knight carrying a battleaxe casts an ethereal light on its surroundings.",
		"gazing at", "Gazing",
		{"strikes",  {0}}},
    {"An immortal presence stalks through the dungeon, implacably hunting that which was taken and the one who took it.",
		"gazing at", "Gazing",
		{"strikes",  {0}}},
	{"This totem sits at the center of a summoning circle that radiates a strange energy.",
		"gazing at", "Gazing",
		{"strikes",  {0}},
        {0},
        "crackles with energy as you touch the glyph!"},
	{"A prism of shoulder-high mirrored surfaces gleams in the darkness.",
		"gazing at", "Gazing",
		{"strikes",  {0}}},

	{"The unicorn's flowing mane and tail shine with rainbow light, $HISHER horn glows with healing and protective magic, and $HISHER eyes implore you to always chase your dreams. Unicorns are rumored to be attracted to virgins -- is there a hint of accusation in $HISHER gaze?",
		"consecrating", "Consecrating",
		{"pokes", "stabs", "gores", {0}}},
	{"A whirling desert storm given human shape, the ifrit's twin scimitars flicker in the darkness and $HISHER eyes burn with otherworldly zeal.",
		"absorbing", "Absorbing",
		{"cuts", "slashes", "lacerates", {0}}},
	{"This legendary bird shines with a brilliant light, and $HISHER wings crackle and pop like embers as they beat the air. When $HESHE dies, legend has it that an egg will form and a newborn phoenix will rise from its ashes.",
		"cremating", "Cremating",
		{"pecks", "scratches", "claws", {0}}},
	{"Cradled in a nest of cooling ashes, the translucent membrane of the phoenix egg reveals a yolk that glows ever brighter by the second.",
		"cremating", "Cremating",
		{"touches", {0}},
		{0},
		"bursts as a newborn phoenix rises from the ashes!"},
	{"This mangrove dryad is as old as the earth, and $HISHER gnarled figure houses an ancient power. When angered, $HESHE can call upon the forces of nature to bind $HISHER foes and tear them to shreds.",
		"absorbing", "Absorbing",
		{"whips", "lashes", "thrashes", "lacerates", {0}}},
	{"The soul of the legendary warrior Hadrian stands before you, sword in hand and buckler held steady. $HISHER stature alone makes it apparent that $HESHE lives up to $HISHER reputation, which has been passed down from generation to generation.",
		"studying", "Studying",
		{"slashes", "bashes", "cuts", "strikes", {0}}},

	{"Though this creature is clearly undead, the dark hide on this dragon shimmers in what little light trickles in to the lower depths of the dungeon. $HISHER penetrating yellow eyes and noxious breath round out its horrifying form.",
		"consuming", "Consuming",
		{"claws", "tail-whips", "bites", {0}}},
	{"This floating membrane is filled with paralytic gas. $HESHE can be weaponized if burst from a distance, but $HESHE can also be quite deadly to adventurers distracted by other monsters and unaware of $HISHER presence.",
		"gazing at", "Gazing",
		{"bumps", {0}},
		"bursts, leaving behind an expanding cloud of numbing gas!"},
	{"With strength and endurance greater than stone, the crystal golem glistens as $HESHE saunters around in the lower depths of the dungeon.",
		"cradling", "Cradling",
		{"backhands", "punches", "kicks", {0}}},
	{"A curious creature, the quickling is tiny and fragile -- but extremely fast. $HESHE can easily outpace even a hastened adventurer and unleash a flurry of quick punches and strikes.",
		"studying", "Studying",
		{"barrages", "strikes", "punches", {0}}},
	{"The shadow centipede's form blends in with the dark dungeon floor, and it moves more like a snake than an insect. Its hazy form is difficult to distinguish, let alone strike down -- especially when moving. Without lightning-quick reflexes, an adventurer could easily succumb to $HISHER strength-sapping venom.",
		"eating", "Eating",
		{"pricks", "stings", {0}}},

	{"This powerful elf-turned-apparition seems to exist only halfway on your plane of existence. $HESHE can invoke a host of powerful magic spells.",
		"draining", "Draining",
		{"cuts", "slices", "slashes", "claws"}},
    {"An undead perversion of the typical surface-dwelling dog, its eyes glow as noticeably as its flesh decays. Like their living counterparts, they often travel in packs. They often coordinate to take down intruders despite their visceral, savage nature.",
		"tearing at", "Eating",
		{"claws", "bites", "mauls", {0}}},

	{"This slimy mass lurks in the shadows, invisible to the naked eye. Although weak, when it splits, it can disorient an adventure.",
		"absorbing", "Feeding",
		{"smears", "slimes", "drenches"}},

	{"Having bided his time for aeons in another plane, the cruel demon-god Moloch has vowed to obtain and the Amulet of Yendor for by any means necessary. $HESHE will stop at nothing to use $HISHER brutish strength and powerful demonic magic to obtain the coveted artifact and ascend over all other gods.",
		"consecrating", "Consecrating",
		{"claws", "slashes", "gores", {0}},
		{0},
        "projects a deafening roar!"},
};

// Mutation definitions

const mutation mutationCatalog[NUMBER_MUTATORS] = {
    //Title         textColor       healthFactor    moveSpdMult attackSpdMult   defMult damMult DF% DFtype  light   monstFlags  abilityFlags    forbiddenFlags      forbiddenAbilities
    {"hallucinogenic",     &pinkJellyColor,        100,         100,         100,            100,     75,     -1, 0,      0,      (0), (MA_HIT_HALLUCINATE), (MONST_MAINTAINS_DISTANCE), 0,
        "A rare mutation has bestowed $HIMHER chemical makeup with a light touch of pantherine, causing $HIMHER to induce hallucinations on attack."},
    {"floating",     &telepathyColor,        100,         75,         75,            100,     100,     -1, 0,      0,      0,      (MONST_FLIES|MONST_FLITS), (0), (MONST_FLIES|MONST_SUBMERGES|MONST_INVISIBLE),
        "An unseen force causes $HIMHER to levitate, allowing $HIMHER to float over pits and lava at the cost of some navigational ability."},
    {"explosive",   &orange,        50,             100,        100,            50,     100,    0,  DF_MUTATION_EXPLOSION, EXPLOSIVE_BLOAT_LIGHT, 0, MA_DF_ON_DEATH, MONST_SUBMERGES, 0,
        "A rare mutation will cause $HIMHER to explode violently when $HESHE dies."},
    {"infested",    &purple,   50,             100,        100,            50,     100,    0,  DF_MUTATION_LICHEN, 0, 0,   MA_DF_ON_DEATH, 0,               0,
        "$HESHE has been infested by deadly lichen spores; poisonous fungus will spread from $HISHER corpse when $HESHE dies."},
    {"caustic",    &poisonGasColor,   50,             100,        100,            50,     100,    0,  DF_BLOAT_DEATH, 0, 0,   MA_DF_ON_DEATH, 0,               MA_DF_ON_DEATH,
        "A rare mutation fills $HESHE with caustic gas, which will spread when $HESHE dies."},
    {"combustible",    &methaneColor,   50,             80,        80,            100,     100,    0,  DF_METHANE_GAS_ARMAGEDDON, 0, 0,   MA_DF_ON_DEATH, 0,               MA_DF_ON_DEATH,
        "A rare mutation has covered $HISHER with methane-filled cysts, which will release as a massive cloud when $HESHE dies."},
    {"juggernaut",  &brown,         300,            200,        200,            75,     200,    -1, 0,      0,      0,          0,              (MONST_MAINTAINS_DISTANCE|MONST_DIES_IF_NEGATED), 0,
        "A rare mutation has hardened $HISHER flesh, increasing $HISHER health and power but compromising $HISHER speed."},
    {"aquatic",   &shallowWaterBackColor,    80,           100,         200,            100,     100,     -1, 0,      0,      (MONST_IMMUNE_TO_WATER|MONST_SUBMERGES), (0), (MONST_FLIES|MONST_IMMUNE_TO_WATER|MONST_SUBMERGES), (0),
        "A rare mutation has bestowed $HIMHER with gills."},
    {"grappling",   &tanColor,      150,            100,        100,            50,     100,    -1, 0,      0,      0,          MA_SEIZES,      MONST_MAINTAINS_DISTANCE, MA_SEIZES,
        "A rare mutation has caused suckered tentacles to sprout from $HISHER frame, increasing $HISHER health and allowing $HIMHER to grapple with $HISHER prey."},
    {"vampiric",    &red,           100,            100,        100,            100,    100,    -1, 0,      0,      0,          MA_TRANSFERENCE, MONST_MAINTAINS_DISTANCE, MA_TRANSFERENCE,
        "A rare mutation allows $HIMHER to heal $HIMSELFHERSELF with every attack."},
    {"toxic",       &green,         100,            100,        200,            100,    20,     -1, 0,      0,      0,          (MA_CAUSES_WEAKNESS | MA_POISONS), MONST_MAINTAINS_DISTANCE, (MA_CAUSES_WEAKNESS | MA_POISONS),
        "A rare mutation causes $HIMHER to poison $HISHER victims and sap their strength with every attack."},
    {"beguiled",       &darkPurple,         60,            100,        200,            75,    20,     -1, 0,      0,      0,          (MA_HIT_SLOWS|MA_CAUSES_WEAKNESS|MA_POISONS|MA_HIT_BLINDS|MA_HIT_HALLUCINATE), MONST_MAINTAINS_DISTANCE, 0,
        "$HESHE has been bound against $HISHER will to a malevolent, otherworldly presence, causing $HISHER attacks to essentially condemn $HISHER victims."},
    {"reflective",  &turquoise, 100,            100,        100,            100,    100,    -1, 0,      CRYSTAL_WALL_LIGHT,      MONST_REFLECT_4, 0,         (MONST_REFLECT_4|MONST_ALWAYS_USE_ABILITY|MONST_IMMUNE_TO_WEAPONS), 0,
        "A rare mutation has coated $HISHER flesh with reflective scales."},
    {"blinding",   &gray,    80,           100,         100,            100,     100,     -1, 0,      0,      (0), (MA_HIT_BLINDS), (0), (MA_HIT_BLINDS),
        "A rare mutation causes ink to seep through $HISHER appendages."},
    {"glass",       &wallCrystalColor,    25,            110,         150,           10,     300,    -1, 0,      0,                    MONST_REFLECT_4, 0,         (MONST_REFLECT_4|MONST_ALWAYS_USE_ABILITY|MONST_IMMUNE_TO_WEAPONS), 0,
        "A rare mutation has replaced $HISHER flesh with sharp but fragile shards of glass."},
    {"fireproof", &fireForeColor, 90,             100,        100,            100,    100,     -1, 0,      LAVA_LIGHT,      (MONST_IMMUNE_TO_FIRE), 0, (MONST_FIERY|MONST_IMMUNE_TO_FIRE|MONST_DIES_IF_NEGATED), 0,
        "A rare mutation has caused $HISHER to seep fireproof mucous through $HISHER pores."},
    {"berserk",     &explosiveAuraColor,        100,            75,         50,            25,     150,     -1, 0,      0,      (MONST_ALWAYS_HUNTING|MONST_NEVER_SLEEPS), (MA_ATTACKS_ALL_ADJACENT|MA_ATTACKS_PENETRATE), (0), (0),
        "Some sort of chaotic spirit seems to have possessed $HIMHER, sending $HIMHER into a blind primal rage."},
    {"skeletal",    &grayFungusColor,        90,            100,         100,            100,     100,     -1, 0,      0,      (MONST_DIES_IF_NEGATED|MONST_IMMUNE_TO_FIRE|MONST_NEVER_SLEEPS), (MA_ATTACKS_ALL_ADJACENT|MA_ATTACKS_PENETRATE), (MONST_DIES_IF_NEGATED), (MA_CLONE_SELF_ON_DEFEND),
        "$HESHE has been stripped of $HISHER fleshy material, leaving only $HISHER magically animated skeleton."},
    {"acidic",   &acidBackColor,    80,           100,         100,            100,     100,     -1, 0,      0,      (MONST_DEFEND_DEGRADE_WEAPON), (MA_HIT_DEGRADE_ARMOR), (MONST_DEFEND_DEGRADE_WEAPON|MONST_DIES_IF_NEGATED), (MA_HIT_DEGRADE_ARMOR),
        "A rare mutation causes acid to seep through the surface of $HISHER body, making $HIMHER corrosive to the touch."},
};

// Horde definitions

const hordeType hordeCatalog[NUMBER_HORDES] = {
	// leader		#members	member list								member numbers					minL	maxL	freq	spawnsIn		machine			flags
	{MK_MONKEY,			0,		{0},									{{0}},							2,		9,		5},
	{MK_RAT,			0,		{0},									{{0}},							1,		5,		15},
	{MK_RAT,			1,		{MK_RAT},								{{2, 5, 1}},					3,		9,		10},
	{MK_RAT,			1,		{MK_RAT},								{{6, 9, 1}},					7,		11,		5},
	{MK_KOBOLD,			0,		{0},									{{0}},							1,		6,		15},
	{MK_JACKAL,			0,		{0},									{{0}},							1,		3,		10},
	{MK_JACKAL,			1,		{MK_JACKAL},							{{2, 5, 1}},					3,		7,		5},
	{MK_EEL,			0,		{0},									{{0}},							2,		17,		10,		DEEP_WATER},
	{MK_MONKEY,			0,		{0},									{{0}},							2,		9,		5},
	{MK_BLOAT,			0,		{0},									{{0}},							2,		13,		3},
	{MK_PIT_BLOAT,		0,		{0},									{{0}},							2,		13,		1},
	{MK_BLOAT,			1,		{MK_BLOAT},								{{0, 2, 1}},					14,		26,		3},
	{MK_PIT_BLOAT,		1,		{MK_PIT_BLOAT},							{{0, 2, 1}},					14,		26,		1},
	{MK_EXPLOSIVE_BLOAT,0,		{0},									{{0}},							10,		26,		1},
	{MK_GOBLIN,			0,		{0},									{{0}},							3,		10,		10},
	{MK_GOBLIN_CONJURER,0,		{0},									{{0}},							3,		13,		6},
	{MK_GOBLIN_DRIVER,  0,		{0},									{{0}},							3,		10,		5},
	{MK_GOBLIN_THIEF,   0,		{0},									{{0}},							3,		10,		5},
	{MK_TOAD,			0,		{0},									{{0}},							4,		11,		10},
	{MK_PINK_JELLY,		0,		{0},									{{0}},							4,		13,		10},
	{MK_GOBLIN_TOTEM,	1,		{MK_GOBLIN},							{{2,4,1}},						5,		13,		10,		0,				MT_CAMP_AREA,	HORDE_NO_PERIODIC_SPAWN},
	{MK_ARROW_TURRET,	0,		{0},									{{0}},							5,		13,		10,		WALL,	0,                      HORDE_NO_PERIODIC_SPAWN},
	{MK_MONKEY,			1,		{MK_MONKEY},							{{1,2,1}},						5,		13,		2},
    {MK_VAMPIRE_BAT,	0,		{0},                                    {{0}},                          6,		13,		3},
    {MK_VAMPIRE_BAT,	1,		{MK_VAMPIRE_BAT},						{{1,2,1}},						6,		13,		7,      0,              0,              HORDE_NEVER_OOD},
    {MK_GLASS_GOLEM,	0,		{0},                                    {{0}},                          11,		19,		3},
    {MK_ROPE_GOLEM,	    0,		{0},                                    {{0}},                          10,		17,		2},
	{MK_ACID_MOUND,		0,		{0},									{{0}},							6,		13,		10},
	{MK_GOBLIN,			3,		{MK_GOBLIN, MK_GOBLIN_MYSTIC, MK_JACKAL},{{2, 3, 1}, {1,2,1}, {1,2,1}},	6,		12,		4},
	{MK_GOBLIN_CONJURER,2,		{MK_GOBLIN_CONJURER, MK_GOBLIN_MYSTIC},	{{0,1,1}, {1,1,1}},				7,		15,		4},
	{MK_CENTIPEDE,		0,		{0},									{{0}},							7,		14,		10},
	{MK_BOG_MONSTER,	0,		{0},									{{0}},							7,		14,		8,		MUD,            0,              HORDE_NEVER_OOD},
	{MK_OGRE,			0,		{0},									{{0}},							7,		13,		10},
	{MK_LEPRECHAUN,		0,		{0},									{{0}},							7,		17,		3}, // gsr
	{MK_EEL,			1,		{MK_EEL},								{{2, 4, 1}},					8,		22,		7,		DEEP_WATER},
	{MK_INK_EEL,		0,		{0},	    							{{0}},      					12,		24,		7,		DEEP_WATER},
	{MK_MAGMA_EEL,		1,		{MK_MAGMA_EEL},							{{0}},      					15,		25,		15,		LAVA},
	{MK_ACID_MOUND,		1,		{MK_ACID_MOUND},						{{2, 4, 1}},					9,		13,		3},
	{MK_SPIDER,			0,		{0},									{{0}},							9,		16,		0},
	{MK_DAR_BLADEMASTER,1,		{MK_DAR_BLADEMASTER},					{{0, 1, 1}},					10,		14,		3},
	{MK_WILL_O_THE_WISP,0,		{0},									{{0}},							10,		17,		10},
	{MK_WRAITH,			0,		{0},									{{0}},							10,		17,		10},
	{MK_GOBLIN_TOTEM,	6,		{MK_GOBLIN_TOTEM, MK_GOBLIN_CONJURER, MK_GOBLIN_MYSTIC, MK_GOBLIN, MK_GOBLIN_THIEF, MK_GOBLIN_DRIVER}, {{1,2,1},{1,2,1},{1,2,1},{3,5,1},{0,1,1},{0,1,1}},10,17,8,0,MT_CAMP_AREA,	HORDE_NO_PERIODIC_SPAWN},
	{MK_SPARK_TURRET,	0,		{0},									{{0}},							11,		18,		10,		WALL,	0,                      HORDE_NO_PERIODIC_SPAWN},
	{MK_ZOMBIE,			0,		{0},									{{0}},							11,		18,		10},
	{MK_TROLL,			0,		{0},									{{0}},							12,		19,		10},
	{MK_NYMPH,			0,		{0},									{{0}},							12,		17,		0},
	{MK_OGRE_TOTEM,		1,		{MK_OGRE},								{{2,4,1}},						12,		19,		6,		0,			0,					HORDE_NO_PERIODIC_SPAWN},
	{MK_BOG_MONSTER,	1,		{MK_BOG_MONSTER},						{{2,4,1}},						12,		26,		10,		MUD},
	{MK_NAGA,			0,		{0},									{{0}},							13,		20,		10,		DEEP_WATER},
	{MK_SALAMANDER,		0,		{0},									{{0}},							13,		20,		10,		LAVA},
	{MK_OGRE_SHAMAN,	1,		{MK_OGRE},								{{1, 3, 1}},					14,		20,		10},
	{MK_CENTAUR,		1,		{MK_CENTAUR},							{{1, 1, 1}},					14,		21,		10},
	{MK_ACID_JELLY,		0,		{0},									{{0}},							14,		21,		10},
	{MK_ACID_TURRET,	0,		{0},									{{0}},							15,		22,		10,		WALL,	0,                      HORDE_NO_PERIODIC_SPAWN},
    {MK_DART_TURRET,	0,		{0},									{{0}},							15,		22,		10,		WALL,	0,                      HORDE_NO_PERIODIC_SPAWN},
	{MK_PIXIE,			0,		{0},									{{0}},							14,		21,		8},
	{MK_FLAME_TURRET,	0,		{0},									{{0}},							14,		24,		10,		WALL,	0,                      HORDE_NO_PERIODIC_SPAWN},
	{MK_ANKHEG,		    0,		{0},							        {{0}},      					18,		MOLOCH_LAIR_LEVEL-1,		5},
	{MK_DAR_BLADEMASTER,2,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS},	{{0, 1, 1}, {0, 1, 1}},			15,		17,		10},
    {MK_PINK_JELLY,     2,		{MK_PINK_JELLY, MK_DAR_PRIESTESS},      {{0, 1, 1}, {1, 2, 1}},			17,		23,		7},
	{MK_KRAKEN,			0,		{0},									{{0}},							15,		30,		5,		DEEP_WATER},
	{MK_FORCE_TOTEM,	0,		{0},									{{0}},							12,		MOLOCH_LAIR_LEVEL-1,		5, HORDE_NO_PERIODIC_SPAWN},
	{MK_PHANTOM,		0,		{0},									{{0}},							16,		MOLOCH_LAIR_LEVEL-1,		10},
	{MK_WRAITH,			1,		{MK_WRAITH},							{{1, 4, 1}},					16,		MOLOCH_LAIR_LEVEL-1,		8},
	{MK_IMP,			0,		{0},									{{0}},							17,		30,		10},
	{MK_DAR_BLADEMASTER,3,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE},{{1,2,1},{1,1,1},{1,1,1}},18,25,10},
	{MK_FURY,			1,		{MK_FURY},								{{2, 4, 1}},					18,		38,		8},
	{MK_SLIME_JELLY,	0,		{0},									{{0}},							12,		25,		10},
	{MK_REVENANT,		0,		{0},									{{0}},							19,		35,		10},
	{MK_GOLEM,			0,		{0},									{{0}},							21,		30,		10},
	{MK_TENTACLE_HORROR,0,		{0},									{{0}},							22,		MOLOCH_LAIR_LEVEL-1,		10},
	{MK_PHYLACTERY,		0,		{0},									{{0}},							22,		MOLOCH_LAIR_LEVEL-1,		10},
	{MK_DRAGON,			0,		{0},									{{0}},							24,		MOLOCH_LAIR_LEVEL-1,		7},
	{MK_DRAGON,			1,		{MK_DRAGON},							{{1,1,1}},						27,		MOLOCH_LAIR_LEVEL-1,		3},
	{MK_GOLEM,			3,		{MK_GOLEM, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE}, {{1, 2, 1}, {0,1,1},{0,1,1}},27,35,	8},
    {MK_KRAKEN,			1,		{MK_KRAKEN},							{{5, 10, 2}},					28,		33,    4,		DEEP_WATER},
	{MK_TENTACLE_HORROR,2,		{MK_TENTACLE_HORROR, MK_REVENANT},		{{1, 3, 1}, {2, 4, 1}},			32,		MOLOCH_LAIR_LEVEL-1,    2},
	{MK_DRAGON,			1,		{MK_DRAGON},							{{3, 5, 1}},					34,		MOLOCH_LAIR_LEVEL-1,    2},


	// "post-game" -- gsr
	{MK_BLACK_DRAGON,	0,		{0},									{{0}},							NETHER_LEVEL - 2,		MOLOCH_LAIR_LEVEL-1,		3},
	{MK_PARALYTIC_BLOAT,0,		{0},									{{0}},							NETHER_LEVEL - 2,		MOLOCH_LAIR_LEVEL-1,		1},
	{MK_CRYSTAL_GOLEM,	0,		{0},									{{0}},							NETHER_LEVEL - 2,		MOLOCH_LAIR_LEVEL-1,		2},
	{MK_QUICKLING,  	0,		{0},									{{0}},							NETHER_LEVEL - 4,		MOLOCH_LAIR_LEVEL-1,		3},
	{MK_QUICKLING,  	1,		{MK_QUICKLING},							{{1,4,1}},						32,		            MOLOCH_LAIR_LEVEL-1,		2},
	{MK_SHADOW_CENIPEDE,0,		{0},									{{0}},							NETHER_LEVEL - 5,		MOLOCH_LAIR_LEVEL-1,		2},
	{MK_SHADOW_CENIPEDE,0,		{MK_SHADOW_CENIPEDE},					{{1,3,1}},						30,		            MOLOCH_LAIR_LEVEL-1,        2},
	{MK_DAR_APPARITION,0,		{0},									{{0}},							NETHER_LEVEL - 1,		MOLOCH_LAIR_LEVEL-1,		1},
	{MK_HELLHOUND,  	1,		{MK_HELLHOUND},							{{1,4,1}},						28,		            MOLOCH_LAIR_LEVEL-1,		2},
	{MK_HELLHOUND,  	1,		{0},            					    {0},    						28,		            MOLOCH_LAIR_LEVEL-1,		2},
	{MK_INVISIBLE_JELLY,  	1,		{0},            					{0},    						34,		            MOLOCH_LAIR_LEVEL-1,		2},

	// Virtually guarantee that these guys will be the only ones inhabiting the elemental plane, though others do hang around --gsr
	{MK_FIRE_ELEMENTAL,0,		{0},									{{0}},							ELEMENTAL_LEVEL-1,		ELEMENTAL_LEVEL-1,		100},
	{MK_WATER_ELEMENTAL,0,		{0},									{{0}},							ELEMENTAL_LEVEL-1,		ELEMENTAL_LEVEL-1,		100},
	{MK_EARTH_ELEMENTAL,0,		{0},									{{0}},							ELEMENTAL_LEVEL-1,		ELEMENTAL_LEVEL-1,		100},
	{MK_AIR_ELEMENTAL,0,		{0},									{{0}},							ELEMENTAL_LEVEL-1,		ELEMENTAL_LEVEL-1,		100},

	{MK_FIRE_ELEMENTAL,0,		{0},									{{0}},							24,		MOLOCH_LAIR_LEVEL-1,		3},
	{MK_WATER_ELEMENTAL,0,		{0},									{{0}},							24,		MOLOCH_LAIR_LEVEL-1,		3},
	{MK_EARTH_ELEMENTAL,0,		{0},									{{0}},							24,		MOLOCH_LAIR_LEVEL-1,		3},
	{MK_AIR_ELEMENTAL,0,		{0},									{{0}},							24,		MOLOCH_LAIR_LEVEL-1,		3},

	// summons
	{MK_GOBLIN_CONJURER,1,		{MK_SPECTRAL_BLADE},					{{3, 5, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
	{MK_OGRE_SHAMAN,	1,		{MK_OGRE},								{{1, 1, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED},
	{MK_VAMPIRE,		1,		{MK_VAMPIRE_BAT},						{{3, 3, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED},
	{MK_LICH,			1,		{MK_FURY},								{{2, 3, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED},
	{MK_PHYLACTERY,		1,		{MK_LICH},								{{1,1,1}},						0,		0,		10,		0,			0,					HORDE_IS_SUMMONED},
	{MK_GOBLIN_CHIEFTAN,3,		{MK_GOBLIN_CONJURER, MK_GOBLIN, MK_GOBLIN_THIEF},		{{1,1,1}, {3,4,1}, {1,1,1}},				0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_SUMMONED_AT_DISTANCE},
	{MK_PHOENIX_EGG,	1,		{MK_PHOENIX},							{{1,1,1}},						0,		0,		10,		0,			0,					HORDE_IS_SUMMONED},
    {MK_ELDRITCH_TOTEM, 1,		{MK_SPECTRAL_BLADE},					{{4, 7, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
    {MK_ELDRITCH_TOTEM, 1,		{MK_FURY},                              {{2, 5, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
    {MK_MOLOCH,         1,		{MK_VAMPIRE_BAT},                       {{2, 4, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
    {MK_MOLOCH,         1,		{MK_FURY},                              {{1, 2, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
    {MK_MOLOCH,         1,		{MK_EXPLOSIVE_BLOAT},                   {{2, 4, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},
    {MK_MOLOCH,         1,		{MK_HELLHOUND},                         {{1, 1, 1}},					0,		0,		10,		0,			0,					HORDE_IS_SUMMONED | HORDE_DIES_ON_LEADER_DEATH},


	// captives
	{MK_GOBLIN,			1,		{MK_KOBOLD},							{{1, 2, 1}},					2,		5,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD}, // gsr
	{MK_MONKEY,			1,		{MK_KOBOLD},							{{1, 2, 1}},					2,		5,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_GOBLIN,			1,		{MK_GOBLIN},							{{1, 2, 1}},					3,		7,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_OGRE,			1,		{MK_GOBLIN},							{{3, 5, 1}},					4,		10,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_GOBLIN_MYSTIC,	1,		{MK_KOBOLD},							{{3, 7, 1}},					5,		11,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_OGRE,			1,		{MK_OGRE},								{{1, 2, 1}},					8,		15,		2,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_TROLL,			1,		{MK_TROLL},								{{1, 2, 1}},					12,		19,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_CENTAUR,		1,		{MK_TROLL},								{{1, 2, 1}},					12,		19,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_TROLL,			2,		{MK_OGRE, MK_OGRE_SHAMAN},				{{2, 3, 1}, {0, 1, 1}},			14,		19,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_DAR_BLADEMASTER,1,		{MK_TROLL},								{{1, 2, 1}},					12,		19,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_NAGA,			1,		{MK_SALAMANDER},						{{1, 2, 1}},					13,		20,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_SALAMANDER,		1,		{MK_NAGA},								{{1, 2, 1}},					13,		20,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_TROLL,			1,		{MK_SALAMANDER},						{{1, 2, 1}},					13,		19,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_IMP,			1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_PIXIE,			1,		{MK_IMP, MK_PHANTOM},					{{1, 2, 1}, {1, 2, 1}},			14,		21,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_DAR_BLADEMASTER,1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_DAR_BLADEMASTER,1,		{MK_IMP},								{{2, 3, 1}},					18,		26,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_DAR_PRIESTESS,	1,		{MK_FURY},								{{2, 4, 1}},					18,		26,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_DAR_BATTLEMAGE,	1,		{MK_IMP},								{{2, 3, 1}},					18,		26,		1,		0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_TENTACLE_HORROR,3,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE},{{1,2,1},{1,1,1},{1,1,1}},20,26,1,	0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},
	{MK_GOLEM,			3,		{MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE},{{1,2,1},{1,1,1},{1,1,1}},18,25,1,	0,			0,					HORDE_LEADER_CAPTIVE | HORDE_NEVER_OOD},

	// machine water monsters
	{MK_EEL,			0,		{0},									{{0}},							2,		7,		10,		DEEP_WATER,	0,					HORDE_MACHINE_WATER_MONSTER},
	{MK_EEL,			1,		{MK_EEL},								{{2, 4, 1}},					5,		15,		10,		DEEP_WATER,	0,					HORDE_MACHINE_WATER_MONSTER},
	{MK_KRAKEN,			0,		{0},									{{0}},							12,		DEEPEST_LEVEL,	10,	DEEP_WATER,	0,				HORDE_MACHINE_WATER_MONSTER},
	{MK_KRAKEN,			1,		{MK_EEL},								{{1, 2, 1}},					12,		DEEPEST_LEVEL,	8,	DEEP_WATER,	0,				HORDE_MACHINE_WATER_MONSTER},

    // for general store starting ally
	{MK_MONKEY,			0,		{0},									{{0}},							1,		1,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_OGRE,			0,		{0},									{{0}},							1,		1,		8,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_GOBLIN_DRIVER,	0,		{0},									{{0}},							1,		1,		9,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_GOBLIN_THIEF,	0,		{0},									{{0}},							1,		1,		9,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_GOBLIN_MYSTIC,	0,		{0},									{{0}},							1,		1,		7,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_GOBLIN_CONJURER,0,		{0},									{{0}},							1,		1,		2,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_TOAD,   		0,		{0},									{{0}},							1,		1,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_TROLL,   		0,		{0},									{{0}},							1,		1,		1,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_LEPRECHAUN,   	0,		{0},									{{0}},							1,		1,		5,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_CENTIPEDE,      0,		{0},									{{0}},							1,		1,		5,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_VAMPIRE_BAT,    0,		{0},									{{0}},							1,		1,		8,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},
	{MK_GLASS_GOLEM,    0,		{0},									{{0}},							1,		1,		3,		MONSTER_CAGE_CLOSED, 0,			HORDE_LEADER_CAPTIVE | HORDE_MACHINE_KENNEL},


	// dungeon captives -- no captors
	{MK_OGRE,			0,		{0},									{{0}},							2,		5,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_NAGA,			0,		{0},									{{0}},							2,		8,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_GOBLIN_MYSTIC,	0,		{0},									{{0}},							2,		8,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_TROLL,			0,		{0},									{{0}},							10,		20,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_DAR_BLADEMASTER,0,		{0},									{{0}},							8,		14,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_DAR_PRIESTESS,	0,		{0},									{{0}},							8,		14,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_WRAITH,			0,		{0},									{{0}},							11,		17,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_GOLEM,			0,		{0},									{{0}},							17,		23,		10,		0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_TENTACLE_HORROR,0,		{0},									{{0}},							20,		AMULET_LEVEL,10,0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},
	{MK_DRAGON,			0,		{0},									{{0}},							23,		AMULET_LEVEL,10,0,			0,					HORDE_MACHINE_CAPTIVE | HORDE_LEADER_CAPTIVE},

	// machine statue monsters
	{MK_GOBLIN,			0,		{0},									{{0}},							1,		6,		10,		STATUE_DORMANT, 0,				HORDE_MACHINE_STATUE},
	{MK_OGRE,			0,		{0},									{{0}},							6,		12,		10,		STATUE_DORMANT, 0,				HORDE_MACHINE_STATUE},
	{MK_WRAITH,			0,		{0},									{{0}},							10,		17,		10,		STATUE_DORMANT, 0,				HORDE_MACHINE_STATUE},
	{MK_NAGA,			0,		{0},									{{0}},							12,		19,		10,		STATUE_DORMANT, 0,				HORDE_MACHINE_STATUE},
	{MK_TROLL,			0,		{0},									{{0}},							14,		21,		10,		STATUE_DORMANT, 0,				HORDE_MACHINE_STATUE},
	{MK_GOLEM,			0,		{0},									{{0}},							21,		30,		10,		STATUE_DORMANT, 0,				HORDE_MACHINE_STATUE},
	{MK_DRAGON,			0,		{0},									{{0}},							29,		DEEPEST_LEVEL,	10,	STATUE_DORMANT, 0,			HORDE_MACHINE_STATUE},
    {MK_TENTACLE_HORROR,0,		{0},									{{0}},							29,		DEEPEST_LEVEL,	10,	STATUE_DORMANT, 0,			HORDE_MACHINE_STATUE},

	// machine turrets
	{MK_ARROW_TURRET,	0,		{0},									{{0}},							5,		13,		10,		TURRET_DORMANT, 0,				HORDE_MACHINE_TURRET},
	{MK_SPARK_TURRET,	0,		{0},									{{0}},							11,		18,		10,		TURRET_DORMANT, 0,				HORDE_MACHINE_TURRET},
	{MK_ACID_TURRET,	0,		{0},									{{0}},							15,		22,		10,		TURRET_DORMANT, 0,				HORDE_MACHINE_TURRET},
	{MK_DART_TURRET,	0,		{0},									{{0}},							15,		22,		10,		TURRET_DORMANT, 0,				HORDE_MACHINE_TURRET},
	{MK_FLAME_TURRET,	0,		{0},									{{0}},							17,		24,		10,		TURRET_DORMANT, 0,				HORDE_MACHINE_TURRET},

	// machine mud monsters
	{MK_BOG_MONSTER,	0,		{0},									{{0}},							12,		26,		10,		MACHINE_MUD_DORMANT, 0,			HORDE_MACHINE_MUD},
	{MK_KRAKEN,			0,		{0},									{{0}},							17,		26,		3,		MACHINE_MUD_DORMANT, 0,			HORDE_MACHINE_MUD},

	// kennel monsters
	{MK_MONKEY,			0,		{0},									{{0}},							1,		5,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_GOBLIN,			0,		{0},									{{0}},							1,		8,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_GOBLIN_CONJURER,0,		{0},									{{0}},							2,		9,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_GOBLIN_MYSTIC,	0,		{0},									{{0}},							2,		9,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_OGRE,			0,		{0},									{{0}},							5,		15,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_OGRE_SHAMAN,    0,		{0},									{{0}},							8,		20,     10,     MONSTER_CAGE_CLOSED, 0,		    HORDE_MACHINE_KENNEL},
	{MK_TROLL,			0,		{0},									{{0}},							10,		19,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_NAGA,			0,		{0},									{{0}},							9,		20,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_SALAMANDER,		0,		{0},									{{0}},							9,		20,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_IMP,			0,		{0},									{{0}},							15,		26,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_PIXIE,			0,		{0},									{{0}},							11,		21,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_MACHINE_KENNEL},
	{MK_DAR_BLADEMASTER,0,		{0},									{{0}},							9,		AMULET_LEVEL, 10, MONSTER_CAGE_CLOSED, 0,		HORDE_MACHINE_KENNEL},
	{MK_DAR_PRIESTESS,	0,		{0},									{{0}},							12,		AMULET_LEVEL, 10, MONSTER_CAGE_CLOSED, 0,		HORDE_MACHINE_KENNEL},
	{MK_DAR_BATTLEMAGE,	0,		{0},									{{0}},							13,		AMULET_LEVEL, 10, MONSTER_CAGE_CLOSED, 0,		HORDE_MACHINE_KENNEL},
	{MK_DRAGON,	        0,		{0},									{{0}},							23,		AMULET_LEVEL, 5, MONSTER_CAGE_CLOSED, 0,		HORDE_MACHINE_KENNEL},


	// vampire bloodbags
	{MK_MONKEY,			0,		{0},									{{0}},							1,		5,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_GOBLIN,			0,		{0},									{{0}},							1,		8,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_GOBLIN_CONJURER,0,		{0},									{{0}},							2,		9,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_GOBLIN_MYSTIC,	0,		{0},									{{0}},							2,		9,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_OGRE,			0,		{0},									{{0}},							5,		15,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_TROLL,			0,		{0},									{{0}},							10,		19,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_NAGA,			0,		{0},									{{0}},							9,		20,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_IMP,			0,		{0},									{{0}},							15,		AMULET_LEVEL,10,MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_PIXIE,			0,		{0},									{{0}},							11,		21,		10,		MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_DAR_BLADEMASTER,0,		{0},									{{0}},							9,		AMULET_LEVEL,10,MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_DAR_PRIESTESS,	0,		{0},									{{0}},							12,		AMULET_LEVEL,10,MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},
	{MK_DAR_BATTLEMAGE,	0,		{0},									{{0}},							13,		AMULET_LEVEL,10,MONSTER_CAGE_CLOSED, 0,			HORDE_VAMPIRE_FODDER},

	// bosses
	{MK_BLACK_JELLY,	1,		{0},									{{0}},    10,		DEEPEST_LEVEL,		2,		0,		0},
	{MK_VAMPIRE,		1,		{0},									{{0}},    20,		DEEPEST_LEVEL,	    2,      0,		0},
	{MK_FLAMEDANCER,	1,		{0},									{{0}},    13,		DEEPEST_LEVEL,	    2,      0,		0},

	// legendary "allies"
	{MK_UNICORN,		0,		{0},									{{0}},							15,		DEEPEST_LEVEL,	5, 0,		0,					HORDE_MACHINE_BOSS},
	{MK_IFRIT,			0,		{0},									{{0}},							15,		DEEPEST_LEVEL,	5,	0,		0,					HORDE_MACHINE_BOSS},
	{MK_PHOENIX_EGG,	0,		{0},									{{0}},							15,		DEEPEST_LEVEL,	5,	0,		0,					HORDE_MACHINE_BOSS},
    {MK_ANCIENT_SPIRIT,	0,		{0},									{{0}},							15,		DEEPEST_LEVEL,	5,	0,		0,					HORDE_MACHINE_BOSS},
    {MK_WARRIOR_SOUL,	0,		{0},									{{0}},							15,		DEEPEST_LEVEL,	5,	0,		0,					HORDE_MACHINE_BOSS},

    // goblin warren
    {MK_GOBLIN,			0,		{0},									{{0}},							1,		10,		10,     0,              0,              HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN_CHIEFTAN,3,		{MK_GOBLIN, MK_GOBLIN_MYSTIC, MK_GOBLIN_DRIVER},{{2, 3, 1}, {1,2,1}, {1,2,1}},	6,		12,		4,      0,              0,              HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN_CONJURER,0,		{0},									{{0}},							1,		10,		6,      0,              0,              HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN_TOTEM,	1,		{MK_GOBLIN},							{{2,4,1}},						5,		13,		10,		0,				MT_CAMP_AREA,	HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN,			3,		{MK_GOBLIN, MK_GOBLIN_MYSTIC, MK_JACKAL},{{2, 3, 1}, {1,2,1}, {1,2,1}},	6,		12,		4,      0,              0,              HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN_CONJURER,2,		{MK_GOBLIN_CONJURER, MK_GOBLIN_MYSTIC},	{{0,1,1}, {1,1,1}},				7,		15,		4,      0,              0,              HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN_TOTEM,	4,		{MK_GOBLIN_TOTEM, MK_GOBLIN_CONJURER, MK_GOBLIN_MYSTIC, MK_GOBLIN}, {{1,2,1},{1,2,1},{1,2,1},{3,5,1}},10,17,8,0,MT_CAMP_AREA,	HORDE_MACHINE_GOBLIN_WARREN},
	{MK_GOBLIN,			1,		{MK_GOBLIN},							{{1, 2, 1}},					3,		7,		1,		0,              0,				HORDE_MACHINE_GOBLIN_WARREN | HORDE_LEADER_CAPTIVE},
};

// Monster class definitions

const monsterClass monsterClassCatalog[MONSTER_CLASS_COUNT] = {
    // name             frequency   maxDepth    member list
    {"abomination",     10,         -1,         {MK_BOG_MONSTER, MK_UNDERWORM, MK_KRAKEN, MK_TENTACLE_HORROR}},
    {"dar",             10,         22,         {MK_DAR_BLADEMASTER, MK_DAR_PRIESTESS, MK_DAR_BATTLEMAGE}},
    {"animal",          10,         10,         {MK_RAT, MK_MONKEY, MK_JACKAL, MK_EEL, MK_INK_EEL, MK_TOAD, MK_CENTIPEDE, MK_SPIDER, MK_ANKHEG}},
    {"goblin",          10,         10,         {MK_GOBLIN, MK_GOBLIN_CONJURER, MK_GOBLIN_MYSTIC, MK_GOBLIN_TOTEM, MK_GOBLIN_CHIEFTAN, MK_GOBLIN_DRIVER, MK_GOBLIN_THIEF}},
    {"ogre",            10,         16,         {MK_OGRE, MK_OGRE_SHAMAN, MK_OGRE_TOTEM}},
    {"dragon",          10,         -1,         {MK_DRAGON, MK_BLACK_DRAGON}},
    {"undead",          10,         -1,         {MK_ZOMBIE, MK_WRAITH, MK_VAMPIRE, MK_PHANTOM, MK_LICH, MK_REVENANT, MK_SHADOW_CENIPEDE, MK_BLACK_DRAGON, MK_DAR_APPARITION}},
    {"jelly",           10,         15,         {MK_PINK_JELLY, MK_BLACK_JELLY, MK_ACID_JELLY, MK_SLIME_JELLY, MK_INVISIBLE_JELLY}},
    {"turret",          5,          18,         {MK_ARROW_TURRET, MK_SPARK_TURRET, MK_DART_TURRET, MK_FLAME_TURRET}},
    {"infernal",        10,         -1,         {MK_FLAMEDANCER, MK_IMP, MK_REVENANT, MK_FURY, MK_PHANTOM, MK_MAGMA_EEL, MK_BLACK_DRAGON}},
    {"waterborne",      10,         17,         {MK_EEL, MK_NAGA, MK_KRAKEN}},
    {"fireborne",       10,         12,         {MK_WILL_O_THE_WISP, MK_SALAMANDER, MK_MAGMA_EEL, MK_FLAMEDANCER}},
    {"troll",           10,         15,         {MK_TROLL}},
};

// ITEMS

// Item flavors

char itemTitles[NUMBER_SCROLL_KINDS][30];

const char titlePhonemes[NUMBER_TITLE_PHONEMES][30] = {
	"glorp",
	"snarg",
	"gana",
	"flin",
	"herba",
	"pora",
	"nuglo",
	"greep",
	"nur",
	"lofa",
	"poder",
	"nidge",
	"pus",
	"wooz",
	"flem",
	"bloto",
	"porta",
    "ermah",
    "gerd",
    "nurt",
    "flurx",
};

char itemColors[NUMBER_ITEM_COLORS][30];

const char itemColorsRef[NUMBER_ITEM_COLORS][30] = {
	"crimson",
	"scarlet",
	"orange",
	"yellow",
	"green",
	"blue",
	"indigo",
	"violet",
	"puce",
	"mauve",
	"burgundy",
	"turquoise",
	"aquamarine",
	"gray",
	"pink",
	"white",
	"lavender",
	"tan",
	"brown",
	"cyan",
	"black"
};

char itemWoods[NUMBER_ITEM_WOODS][30];

const char itemWoodsRef[NUMBER_ITEM_WOODS][30] = {
	"teak",
	"oak",
	"redwood",
	"rowan",
	"willow",
	"mahogany",
	"pinewood",
	"maple",
	"bamboo",
	"ironwood",
	"pearwood",
	"birch",
	"cherry",
	"eucalyptus",
	"walnut",
	"cedar",
	"rosewood",
	"yew",
	"sandalwood",
    "hickory",
    "hemlock",
};

char itemMetals[NUMBER_ITEM_METALS][30];

const char itemMetalsRef[NUMBER_ITEM_METALS][30] = {
	"bronze",
	"steel",
	"brass",
	"pewter",
	"nickel",
	"copper",
	"aluminum",
	"tungsten",
	"titanium",
    "cobalt",
    "chromium",
    "silver",
};

char itemGems[NUMBER_ITEM_GEMS][30];

const char itemGemsRef[NUMBER_ITEM_GEMS][30] = {
	"diamond",
	"opal",
	"garnet",
	"ruby",
	"amethyst",
	"topaz",
	"onyx",
	"tourmaline",
	"sapphire",
	"obsidian",
	"malachite",
	"aquamarine",
	"emerald",
	"jade",
	"alexandrite",
	"agate",
	"bloodstone",
	"jasper"
};

// Item definitions

//typedef struct itemTable {
//	char *name;
//	char *flavor;
//	short frequency;
//	short marketValue;
//	short number;
//	randomRange range;
//} itemTable;

const itemTable keyTable[NUMBER_KEY_TYPES] = {
	{"door key",			"", "", 1, 1,	0, {0,0,0}, true, false, "The notches on this ancient iron key are well worn; its leather lanyard is battered by age. What door might it open?"},
	{"cage key",			"", "", 1, 1,	0, {0,0,0}, true, false, "The rust accreted on this iron key has been stained with flecks of blood; it must have been used recently. What cage might it open?"},
	{"crystal orb",			"", "", 1, 1,	0, {0,0,0}, true, false, "A faceted orb, seemingly cut from a single crystal, sparkling and perpetually warm to the touch. What manner of device might such an object activate?"},
};

const itemTable foodTable[NUMBER_FOOD_KINDS] = {
//	{"ration of food",		"", "", 3, 25,	1800, {0,0,0}, true, false, "A ration of food. Was it left by former adventurers? Is it a curious byproduct of the subterranean ecosystem?"},
//	{"mango",				"", "", 1, 15,	1550, {0,0,0}, true, false, "An odd fruit to be found so deep beneath the surface of the earth, but only slightly less filling than a ration of food."}
	{"ration of food",		"", "", 3, 3,	NUTRITION_FOOD, {0,0,0}, true, false, "A ration of food. Was it left by former adventurers? Is it a curious byproduct of the subterranean ecosystem?"},
	{"mango",				"", "", 1, 2,	NUTRITION_MANGO, {0,0,0}, true, false, "An odd fruit to be found so deep beneath the surface of the earth, but only slightly less filling than a ration of food."}
};

// Short sword and mace never spawn. This gives us two weapons per category. Consistency!
const itemTable weaponTable[NUMBER_WEAPON_KINDS] = {
	{"dagger",				"", "",  8, 11,		11,	{3,	4,	1},		true, false, "A simple iron dagger with a well-worn wooden handle. Daggers will deal quintuple damage upon a successful sneak attack instead of triple damage."},
	{"kris",				"", "",  9, 33,		15,	{5,	6,	1},		true, false, "This jagged blade is as deadly as it is jarring. It will deal quintuple damage upon a successful sneak attack instead of triple damage."},

	{"short sword",		    "", "",  0, 30,		12, {4,	7,	1},		true, false, "Though not the most impressive of blades, this short sword is still formidable."},
 	{"sword",				"", "", 10, 44,		14, {7,	9,	1},		true, false, "The razor-sharp length of steel blade shines reassuringly."},
	{"broadsword",			"", "",  7, 99,		19,	{14, 22, 1},	true, false, "This towering blade inflicts heavy damage by investing its heft into every cut."},

    {"whip",			    "", "", 10, 44,		14, {3,	5,	1},		true, false, "This lash from this coil of braided leather can tear bark from trees, and it will reach opponents up to five spaces away."},
    {"barbed whip",			"", "",  7, 65,		19, {10, 14,1},		true, false, "A copious number of sharp iron barbs are embedded in this hefty leather whip. It will reach opponents up to five spaces away."},

    {"rapier",				"", "", 10, 44,		15, {3,	5,	1},		true, false, "This blade is thin and flexible, designed for deft and rapid maneuvers. It inflicts less damage than comparable weapons, but permits you to attack twice as quickly. If there is one space between you and an enemy and you step directly toward it, you will perform a devastating lunge attack, which deals treble damage and never misses."},
    {"estoc",	    		"", "", 10, 44,		17, {6,	9,	1},		true, false, "More rigid and hefty than a rapier, a properly wielded estoc is a very devastating dueling weapon. It inflicts less damage than comparable weapons, but permits you to attack twice as quickly. If there is one space between you and an enemy and you step directly toward it, you will perform a devastating lunge attack, which deals treble damage and never misses."},

    {"nunchaku",			"", "", 10, 35,		14, {5, 7,	1},		true, false, "A pair of wooden sticks attached with a short rope. As you hold one stick, the other can be whirled in synchronicity with your movement, allowing you a free attack whenever moving between two spaces that are adjacent to an enemy."},
    {"flail",				"", "",  8, 44,		17, {10,13,	1},		true, false, "The spiked iron ball can be whirled at the end of its chain in synchronicity with your movement, allowing you a free attack whenever moving between two spaces that are adjacent to an enemy."},

	{"club",				"", "", 10, 41,		13, {9,  15, 1},	true, false, "The stone club is one of the simplest weapons available. Because of its heft, it takes two turns when it hits."},
	{"mace",				"", "",  0, 66,		16, {16, 20, 1},	true, false, "The iron flanges at the head of this weapon inflict substantial damage with every weighty blow. Because of its heft, it takes two turns when it hits."},
	{"war hammer",			"", "", 10, 110,	20, {25, 35, 1},	true, false, "Few creatures can withstand the crushing blow of this towering mass of lead and steel, but only the strongest of adventurers can effectively wield it. Because of its heft, it takes two turns when it hits."},

	{"spear",				"", "", 10, 33,		13, {4, 5, 1},		true, false, "A slender wooden rod tipped with sharpened iron. The reach of the spear permits you to simultaneously attack an adjacent enemy and the enemy directly behind it."},
	{"war pike",			"", "",  5, 88,		18, {11, 15, 1},	true, false, "A long steel pole ending in a razor-sharp point. The reach of the pike permits you to simultaneously attack an adjacent enemy and the enemy directly behind it."},

	{"axe",					"", "", 10, 55,		15, {7, 9, 1},		true, false, "The blunt iron edge on this axe glints in the darkness. The arc of its swing permits you to attack all adjacent enemies simultaneously."},
	{"war axe",				"", "",  6, 99,		19, {12, 17, 1},	true, false, "The enormous steel head of this war axe puts considerable heft behind each stroke. The arc of its swing permits you to attack all adjacent enemies simultaneously."},
};

const itemTable throwingWeaponTable[NUMBER_THROWING_WEAPON_KINDS] = {
	{"dart",				"", "",	30,	2,			10,	{4,	7,	1},		true, false, "These simple metal spikes are weighted to fly true and sting their prey with a flick of the wrist."},
    {"poison dart",		    "", "",	18, 4,			12,	{1,	2,	1},		true, false, "These poison-tipped darts can be used to disable opponents from a distance."},
	{"incendiary dart",		"", "",	16, 4,			12,	{1,	2,	1},		true, false, "The barbed spike on each of these darts is designed to stick to its target while the compounds strapped to its length explode into flame."},
	{"tranquilizer dart",	"", "",	8,  4,			12,	{1,	2,	1},		true, false, "These small ballistic needles contain an anesthetic chemical that will paralyze most monsters for a brief period of time."},
	{"shuriken",			"", "",	11, 3,			14,	{6, 10, 3},		true, false, "These light, balanced circular throwing weapons each bear a series of blades exhibiting perfect rotational symmetry."},
	{"javelin",				"", "",	10, 3,			16,	{7, 12, 1},		true, false, "This length of metal is weighted to keep the spike at its tip foremost as it sails through the air."},
};


const itemTable armorTable[NUMBER_ARMOR_KINDS] = {
    {"cloak",       	"", "", 10,	5,		5,	{5,5,0},		true, false, "This simple garment covers you almost completely, but it offers very little protection."},
	{"leather armor",	"", "", 10,	20,		12,	{30,30,0},		true, false, "This lightweight armor offers basic protection."},
	{"scale mail",		"", "", 10, 45,		12, {40,40,0},		true, false, "Bronze scales cover the surface of treated leather, offering greater protection than plain leather with minimal additional weight."},
	{"chain mail",		"", "", 10, 50,		13, {50,50,0},		true, false, "Interlocking metal links make for a tough but flexible suit of armor."},
	{"banded mail",		"", "", 10, 80,		15, {70,70,0},		true, false, "Overlapping strips of metal horizontally encircle a chain mail base, offering an additional layer of protection at the cost of greater weight."},
	{"splint mail",		"", "", 10, 200,	17, {90,90,0},		true, false, "Thick plates of metal are embedded into a chain mail base, providing the wearer with substantial protection."},
	{"plate armor",		"", "", 10, 350,	19, {110,110,0},	true, false, "Enormous plates of metal are joined together into a suit that provides unmatched protection to any adventurer strong enough to bear its staggering weight."}
};

const char weaponRunicNames[NUMBER_WEAPON_RUNIC_KINDS][30] = {
	"teleportation",
	"speed",
	"quietus",
	"paralysis",
	"multiplicity",
//	"slowing",
//	"confusion",
    "force",
	"slaying",
	"poison",
	"flames",
	"mercy",
	"the double edge", // gsr
	"plenty"
};

const char armorRunicNames[NUMBER_ARMOR_ENCHANT_KINDS][30] = {
	"multiplicity",
	"mutuality",
	"absorption",
	"reprisal",
	"immunity",
	"reflection",
    "respiration",
    "free action",//"dampening",
	"force", // gsr
	"shadows", // gsr
	"water walking",
	"burden",
	"vulnerability",
	"frailty", // gsr
	"teleportation", // gsr
    "immolation",
    "doom", // gsr
};

itemTable scrollTable[NUMBER_SCROLL_KINDS] = {
	{"enchanting",			itemTitles[0], "",	0,	55,	0,{0,0,0}, false, false, "This indispensable scroll will imbue a single item with a powerful and permanent magical charge. A staff will increase in power and in number of charges; a weapon will inflict more damage or find its mark more frequently; a suit of armor will deflect additional blows; the effect of a ring on its wearer will intensify. Weapons and armor will also require less strength to use, and any curses on the item will be lifted."}, // frequency is dynamically adjusted
	{"identify",			itemTitles[1], "",	20,	30,	0,{0,0,0}, false, false, "The scrying magic on this parchment will permanently reveal all of the secrets of a single item."},
	{"teleportation",		itemTitles[2], "",	10,	50,	0,{0,0,0}, false, false, "The spell on this parchment instantly transports the reader to a random location on the dungeon level. It can be used to escape a dangerous situation, but the unlucky reader might find himself in an even more dangerous place."},
	{"remove curse",		itemTitles[3], "",	12,	15,	0,{0,0,0}, false, false, "The incantation on this scroll will instantly strip from the reader's weapon, armor, rings and carried items any evil enchantments that might prevent the wearer from removing them."},
	{"recharging",			itemTitles[4], "",	8,	37,	0,{0,0,0}, false, false, "The power bound up in this parchment will instantly recharge all of your staffs and charms."},
//	{"protect armor",		itemTitles[5], "",	10,	40,	0,{0,0,0}, false, false, "The armor worn by the reader of this scroll will be permanently proofed against degradation from acid."},
//	{"protect weapon",		itemTitles[6], "",	10,	40,	0,{0,0,0}, false, false, "The weapon held by the reader of this scroll will be permanently proofed against degradation from acid."},
	{"protect equipment",	itemTitles[5], "",	10,	100,	0,{0,0,0}, false, false, "Any weapon in hand and armor worn by the reader of this scroll will be permanently proofed against degradation from acid."},
    {"sanctuary",           itemTitles[6], "",  10, 50,    0,{0,0,0}, false, false, "When recited over plain ground, the sacred rite of protection memorialized on this scroll will imbue the area with powerful warding glyphs. Monsters will not willingly set foot on the affected area."},
	{"magic mapping",		itemTitles[7], "",	12,	50,	0,{0,0,0}, false, false, "When this scroll is read, a purple-hued image of crystal clarity will be etched into your memory, alerting you to the precise layout of the level and revealing all hidden secrets. The locations of items and creatures will remain unknown."},
	{"negation",			itemTitles[8], "",	0,	40,	0,{0,0,0}, false, false, "This scroll contains a powerful anti-magic. When it is released, all creatures (including yourself) and all items lying on the ground within your field of view will be exposed to its blast and stripped of magic -- and creatures animated purely by magic will die. Potions, scrolls, items being held by other creatures and items in your inventory will not be affected."},
	{"shattering",			itemTitles[9],"",	10,	50,	0,{0,0,0}, false, false, "The blast of sorcery unleashed by this scroll will alter the physical structure of nearby stone, causing it to dissolve away over the ensuing minutes."},
    {"discord",             itemTitles[10], "",	9,	40,	0,{0,0,0}, false, false, "Reading this scroll will unleash a powerful blast of mind magic. Any creatures within line of sight will turn against their companions and attack indiscriminately for 30 turns."},
	{"great identify",		itemTitles[11], "",	2,	200,	0,{0,0,0}, false, false, "This extremely rare parchment will reveal the secrets of every item in the reader's pack when read aloud."},
	{"domination",		    itemTitles[12], "",	2,	180,	0,{0,0,0}, false, false, "A sweet pink aroma released by this scroll will bind the closest enemy to the reader's will, turning it into a steadfast ally."},
 	{"chaos",		        itemTitles[13], "",	2,	200,	0,{0,0,0}, false, false, "This scroll will cause absolute chaos all around, transporting every creature on the level to a random location. It might flush out hidden monsters, or it could make for a last resort. It could also be combined with telepathic power to indirectly map out portions of the current level."},
	{"aggravate monsters",	itemTitles[14], "",	6,	3,		0,{0,0,0}, false, false, "When read aloud, this scroll will unleash a piercing shriek that will awaken all monsters and alert them to the reader's location."},
	{"summon monsters",		itemTitles[15], "",	10,	3,		0,{0,0,0}, false, false, "The incantation on this scroll will call out to creatures in other planes of existence, drawing them through the fabric of reality to confront the reader."},
};

itemTable charmTable[NUMBER_CHARM_KINDS] = {
	{"health",          "", "",	3,	90,	0,{1,2,1}, true, false, "This handful of dried bloodwort and mandrake root has been bound together with leather cord and imbued with a powerful healing magic."},
	{"protection",		"", "",	0,	80,	0,{1,2,1}, true, false, "Four copper rings have been joined into a tetrahedron. The construct is oddly warm to the touch."},
	{"haste",           "", "",	5,	75,	0,{1,2,1}, true, false, "Various animals have been etched into the surface of this brass bangle. It emits a barely audible hum."},
	{"fire immunity",	"", "",	3,	75,	0,{1,2,1}, true, false, "Eldritch flames flicker within this polished crystal bauble."},
	{"invisibility",	"", "",	5,	70,	0,{1,2,1}, true, false, "This intricate figurine depicts a strange humanoid creature. It has a face on both sides of its head, but all four eyes are closed."},
//	{"telepathy",		"", "",	3,	70,	0,{1,2,1}, true, false, "Seven tiny glass eyes roll freely within this glass sphere. Somehow, they always come to rest facing outward."},
	{"levitation",      "", "",	4,	70,	0,{1,2,1}, true, false, "Sparkling dust and fragments of feather waft and swirl endlessly inside this small glass sphere."},
    {"shattering",      "", "",	0,	70,	0,{1,2,1}, true, false, "This turquoise crystal, fixed to a leather lanyard, hums with an arcane energy that sets your teeth on edge."},
    {"guardian",        "", "",	5,	70,	0,{1,2,1}, true, false, "When you touch this tiny granite statue, a rhythmic booming sound echoes in your head."},
    {"fear",            "", "",	3,	70,	0,{1,2,1}, true, false, "When you gaze into the murky interior of this obsidian cube, you feel as though something predatory is watching you."},
    {"teleportation",   "", "",	4,	70,	0,{1,2,1}, true, false, "The surface of this nickel sphere has been etched with a perfect grid pattern. Somehow, the squares of the grid are all exactly the same size."},
    {"recharging",      "", "",	0,	70,	0,{1,2,1}, true, false, "A strip of bronze has been wound around a rough wooden sphere. Each time you touch it, you feel a tiny electric shock."},
//    {"negation",        "", "",	5,	70,	0,{1,2,1}, true, false, "A featureless gray disc hangs from a leather lanyard. When you touch it, your hand briefly goes numb."},
    {"identify",        "", "",	0,	85,	0,{1,2,1}, true, false, "This pocket-sized tome is bound with worn leather. A bookmark of crimson silk flows from the top of its spine, dancing and waving subtly as if it were caught by a breeze."},
    {"discord",         "", "",	2,	85,	0,{1,2,1}, true, false, "A series of jagged lines dance around the surface of this otherwise pristine shard of crystal."},
    {"beckoning",       "", "",	0,	70,	0,{1,2,1}, true, false, "When you place your hand near this gray sphere, you can feel a strange force pulling your hand toward it."},
    {"provocation",     "", "",	0,	50,	0,{1,2,1}, true, false, "This small brass bell seems to be perfectly soundproof, both inside and out. It is completely silent even as it brushes against other equipment in your pack."},
};

itemTable potionTable[NUMBER_POTION_KINDS] = {
//	{"life",				itemColors[0], "",	0,	500,	0,{0,0,0}, false, false, "A swirling elixir that will instantly heal you, cure you of ailments, and permanently increase your maximum health."}, // frequency is dynamically adjusted
//    {"strength",			itemColors[1], "",	0,	400,	0,{0,0,0}, false, false, "This powerful medicine will course through your muscles, permanently increasing your strength by one point."}, // frequency is dynamically adjusted
    {"empowerment",			itemColors[0], "",	0,	50,	0,{0,0,0}, false, false, "This powerful elixir will permanently increase your strength by one point, instantly heal you, cure you of all ailments, and permanently increase your maximum health. It can also be thrown at an ally (or an unfriendly creature!) to empower it similarly."}, // frequency is dynamically adjusted
	{"telepathy",           itemColors[1], "",	15,	35,	0,{0,0,0}, false, false, "After drinking this, your mind will become attuned to the psychic signature of distant creatures, enabling you to sense biological presences through walls. Its effects will not reveal inanimate objects, such as totems, turrets and traps.\n\nThrowing it directly at a creature will form a one-way telepathic bond, allowing you to permanently track its movements.\n\nShattering it on the ground or against a wall will psychically reveal to you a potion of the level around the point of impact."},
	{"levitation",			itemColors[2], "",	15,	25,	0,{0,0,0}, false, false, "Drinking this curious liquid will cause you to hover in the air, able to drift effortlessly over lava, water, chasms and traps. Flames, gases and spiderwebs fill the air, however, and cannot be bypassed while airborne. Creatures that dwell in water or mud will be unable to attack you while you levitate.\n\nWhen thrown at a creature, this concoction will also allow it to float or fly for a long time."},
	{"detect magic",		itemColors[3], "",	15,	50,	0,{0,0,0}, false, false, "This drink will sensitize your mind to the radiance of magic. Items imbued with helpful enchantments will be marked on the map with a full magical sigil; items corrupted by curses or intended to inflict harm on the bearer will be marked on the map with a hollow sigil. The Amulet of Yendor, if in the vicinity, will be revealed by its unique aura."},
	{"speed",				itemColors[4], "",	20,	50,	0,{0,0,0}, false, false, "Quaffing the contents of this flask will enable you to move at blinding speed for several minutes. Throwing it at a creature will grant it the same effect.\n\nSince the flask contains an energetic compound, shattering it on the ground will produce a tiny ember."},
	{"fire immunity",		itemColors[5], "",	15,	50,	0,{0,0,0}, false, false, "This potion will render you impervious to heat and permit you to wander through fire and lava and ignore otherwise deadly bolts of flame. It can also be thrown at a creature (most importantly, an ally) to grant it fire immunity instead. (It will not guard against the concussive impact of an explosion, however.)"},
    {"invisibility",		itemColors[6], "",	20,	40,	0,{0,0,0}, false, false, "Drinking this potion will render you temporarily invisible. Enemies more than two spaces away will be unable to track you. It can be thrown at a creature to make it invisible for quite some time.\n\nAllowing the potion to shatter will instead create a cloud of supernatural darkness where it lands, and enemies will have difficulty seeing or following you if you take refuge under its cover."},
    {"healing",				itemColors[7], "",  25,	50,	0,{0,0,0}, false, false, "When this flask is shattered, a thick cloud of healing spores will emerge. When consumed directly, the concentrated substance will heal you significantly and rid you of many negative status ailments. It can also be thrown at an ally to heal its wounds instead."},
    {"respiration",			itemColors[8], "",  15,	20,	0,{0,0,0}, false, false, "When consumed, the contents of this flask will render you immune to harmful gases temporarily. If shattered, it will dissipate gas surrounding the immediate area around the point of impact."},
//    {"caustic gas",         itemColors[9], "",	    15,	200,	0,{0,0,0}, false, false, "Uncorking or shattering this pressurized glass will cause its contents to explode into a deadly cloud of caustic purple gas. You might choose to fling this potion at distant enemies instead of uncorking it by hand."},
	{"paralysis",			itemColors[9], "",	15, 25,	0,{0,0,0}, false, false, "Upon exposure to open air, the liquid in this flask will vaporize into a numbing pink haze. Anyone who inhales the cloud will be paralyzed instantly, unable to move for some time after the cloud dissipates. This item can be thrown at distant enemies to catch them within the effect of the gas."},
	{"hallucination",       itemColors[10], "",	15,	10,	0,{0,0,0}, false, false, "This flask contains a mysterious compound that has a variety of effects based on altering reality. If consumed, you will drunkenly wander through a rainbow wonderland, temporarily unable to discern the form of any creatures or items you see.\n\nShattering it will release a cloud of hallucinogenic gas, which will cause discord among creatures.\n\nBut when thrown directly at a creature, that creature will absorb the compound and transform into another creature at random. The tamest of creatures might turn into the most fearsome, and the horror of the transformation will turn any affected allies against you."},
//	{"confusion",			itemColors[12], "",	15,	45,	0,{0,0,0}, false, false, "This unstable chemical will quickly vaporize into a glittering cloud upon contact with open air, causing any creature that inhales it to lose control of the direction of its movements until the effect wears off (although its ability to aim projectile attacks will not be affected). Its vertiginous intoxication can cause creatures and adventurers to careen into one another or into chasms or lava pits, so extreme care should be taken when under its effect. Its contents can be weaponized by throwing the flask at distant enemies."},
	{"incineration",		itemColors[11], "",	18,	50,	0,{0,0,0}, false, false, "This flask contains an unstable compound which will burst violently into flame upon exposure to open air. You might throw the flask at distant enemies -- or into a deep lake, to cleanse the cavern with scalding steam."},
//	{"darkness",			itemColors[14], "",	7,	15,	0,{0,0,0}, false, false, "Drinking this potion will plunge you into darkness. At first, you will be completely blind to anything not illuminated by an independent light source, but over time your vision will regain its former strength. Throwing the potion will create a cloud of supernatural darkness, and enemies will have difficulty seeing or following you if you take refuge under its cover."},
//	{"descent",				itemColors[15], "",	15,	50,	0,{0,0,0}, false, false, "When this flask is uncorked by hand or shattered by being thrown, the fog that seeps out will temporarily cause the ground in the vicinity to vanish."},
//	{"creeping death",		itemColors[12], "",	7,	45,	0,{0,0,0}, false, false, "When the cork is popped or the flask is thrown, tiny spores will spill across the ground and begin to grow a deadly lichen. Anything that touches the lichen will be poisoned by its clinging tendrils, and the lichen will slowly grow to fill the area. Fire will purge the infestation."},
    {"caustic gas",         itemColors[12], "",	20,	20,0,{0,0,0}, false, false, "Uncorking or shattering this pressurized glass will cause its contents to explode into a deadly cloud of caustic, flammable purple gas. You might choose to fling this potion at distant enemies instead of uncorking it by hand."},
    {"methane",             itemColors[13], "",	10,	20,0,{0,0,0}, false, false, "This flask contains a massive amount of flammable methane gas kept under intense pressure. Uncorking or shattering it will cause its contents to billow out and spread quickly and all around. One small flicker of flame will cause the entire cloud to explode violently."},
};

// There aren't any wands, and so this is kinda useless --gsr
itemTable wandTable[NUMBER_WAND_KINDS] = {
	{"teleportation",	itemMetals[0], "",	3,	80,	BOLT_TELEPORT,      {3,5,1}, false, false, "A blast from this wand will teleport a creature to a random place on the level. This can be particularly effective against aquatic or mud-bound creatures, which are helpless on dry land."},
	{"slowness",		itemMetals[1], "",	3,	80,	BOLT_SLOW,          {2,5,1}, false, false, "This wand will cause a creature to move at half its ordinary speed for 30 turns."},
	{"polymorphism",	itemMetals[2], "",	3,	70,	BOLT_POLYMORPH,     {3,5,1}, false, false, "This mischievous magic can transform any creature into another creature at random. Beware: the tamest of creatures might turn into the most fearsome. The horror of the transformation will turn any affected allies against you."},
	{"negation",		itemMetals[3], "",	3,	55,	BOLT_NEGATION,      {4,6,1}, false, false, "This powerful anti-magic will strip a creature of a host of magical traits, including flight, invisibility, acidic corrosiveness, telepathy, magical speed or slowness, hypnosis, magical fear, immunity to physical attack, fire resistance and the ability to blink. Spellcasters will lose their magical abilities and magical totems will be rendered inert. Creatures animated purely by magic will die."},
	{"domination",		itemMetals[4], "",	1,	100,	BOLT_DOMINATION,    {1,2,1}, false, false, "This wand can forever bind an enemy to the caster's will, turning it into a steadfast ally. However, the magic works only against enemies that are near death."},
	{"beckoning",		itemMetals[5], "",	3,	50,	BOLT_BECKONING,     {2,4,1}, false, false, "The force of this wand will yank the targeted creature into direct proximity."},
	{"plenty",			itemMetals[6], "",	2,	70,	BOLT_PLENTY,        {1,2,1}, false, false, "The creature at the other end of this mischievous bit of metal will be beside itself -- literally! Cloning an enemy is ill-advised, but the effect can be invaluable on a powerful ally."},
//	{"invisibility",	itemMetals[7], "",	3,	10,	BOLT_INVISIBILITY,  {3,5,1}, false, false, "A charge from this wand will render a creature temporarily invisible to the naked eye. Only with telepathy or in the silhouette of a thick gas will an observer discern the creature's hazy outline."},
    {"empowerment",     itemMetals[7], "",	2,	10,	BOLT_EMPOWERMENT,   {1,1,1}, false, false, "This sacred magic will permanently improve the mind and body of any monster it hits. A wise adventurer will use it on allies, making them stronger in combat and able to learn a new talent from a fallen foe. If the bolt is reflected back at you, it will have no effect."},
};

itemTable staffTable[NUMBER_STAFF_KINDS] = {
	{"lightning",		itemWoods[0], "",	15,	530,	BOLT_LIGHTNING,     {2,4,1}, false, false, "This staff conjures forth deadly arcs of electricity, which deal damage to any number of creatures in a straight line. The bolt of electricity will also bounce off one solid obstacle in a random direction."},
	{"firebolt",		itemWoods[1], "",	15,	430,	BOLT_FIRE,          {2,4,1}, false, false, "This staff unleashes bursts of magical fire. It will ignite flammable terrain, and will damage and burn a creature that it hits. Creatures with an immunity to fire will be unaffected by the bolt."},
	{"poison",			itemWoods[2], "",	8,	420,	BOLT_POISON,        {2,4,1}, false, false, "The vile blast of this twisted bit of wood will imbue its target with a deadly venom. Each turn, a creature that is poisoned will suffer one point of damage per dose of poison it has received, and poisoned creatures will not regenerate lost health until the poison clears."},
	{"tunneling",		itemWoods[3], "",	15,	300,	BOLT_TUNNELING,     {2,4,1}, false, false, "Bursts of magic from this staff will pass harmlessly through creatures but will reduce walls and other inanimate obstructions to rubble."},
	{"blinking",		itemWoods[4], "",	11,	420,	BOLT_BLINKING,      {2,4,1}, false, false, "This staff will allow you to teleport in the chosen direction. Creatures and inanimate obstructions will block the teleportation. Be careful around dangerous terrain, as nothing will prevent you from teleporting to a fiery death in a lake of lava."},
	{"obstruction",		itemWoods[5], "",	10,	400,	BOLT_OBSTRUCTION,   {2,4,1}, false, false, "A mass of impenetrable green crystal will spring forth from the point at which this staff is aimed, obstructing any who wish to move through the affected area and temporarily entombing any who are already there. The crystal will dissolve into the air as time passes. Higher level staffs will create larger obstructions."},
	{"discord",			itemWoods[6], "",	5,	300,	BOLT_DISCORD,       {2,4,1}, false, false, "The purple light from this staff will alter the perception of a creature to lash out indiscriminately. Strangers and allies alike will turn on the victim."},
	{"conjuration",		itemWoods[7], "",	3,	500,	BOLT_CONJURATION,   {2,4,1}, false, false, "A flick of this staff summons a number of phantom blades to fight on your behalf."},
	{"force",   		itemWoods[8], "",	10,	500,	BOLT_FORCE,         {2,4,1}, false, false, "The heavyweight beam from this staff will send a monster backward, with distance depending on the staff's enchantment. Targeted creatures could very well slam into walls or other creatures -- or perhaps even pushed into lava or traps."},
	{"slowness",		itemWoods[9], "",	12,	100,	BOLT_SLOW,          {2,4,1}, false, false, "This staff will cause a creature to move at half its ordinary speed for some time."},
	{"beckoning",		itemWoods[10], "",	12,	50,	    BOLT_BECKONING,     {2,4,1}, false, false, "The force of this staff will yank the targeted creature into direct proximity. This may be used to draw fleeing monsters or reckless allies toward you."},
    {"domination",		itemWoods[11], "",	0,	950,	BOLT_DOMINATION,    {1,2,1}, false, false, "This staff can forever bind an enemy to the caster's will, turning it into a steadfast ally. However, the magic works only against enemies that are near death."},
};

itemTable ringTable[NUMBER_RING_KINDS] = {
	{"clairvoyance",	itemGems[0], "",	4,	290,	0,{1,3,1}, false, false, "Wearing this ring will permit you to see through nearby walls and doors, within a radius determined by the level of the ring. A cursed ring of clairvoyance will blind you to your immediate surroundings."},
	{"stealth",			itemGems[1], "",	6,	280,	0,{1,3,1}, false, false, "This ring will reduce your stealth range, making enemies less likely to notice you and more likely to lose your trail. Staying motionless and lurking in the shadows will make you even harder to spot. Cursed rings of stealth will increase your stealth range, making you easier to spot and to track."},
	{"regeneration",	itemGems[2], "",	4,	475,	0,{1,3,1}, false, false, "This ring increases the body's regenerative properties, allowing one to recover lost health at an accelerated rate. Cursed rings will decrease or even halt one's natural regeneration."},
	{"transference",	itemGems[3], "",	4,	375,	0,{1,3,1}, false, false, "Landing a melee attack while wearing this ring will heal you in proportion to the damage inflicted. Cursed rings will cause you to lose health with each attack you land."},
	{"perception",		itemGems[4], "",	7,	170,	0,{1,3,1}, false, false, "This ring will enhance your perception. It enables you to see farther in the dimming light of the deeper dungeon levels and to notice hidden secrets (traps, secret doors and hidden levers) more often and from a greater distance. Cursed rings will dull your perception."},
	{"wisdom",			itemGems[5], "",	4,	470,	0,{1,3,1}, false, false, "Your staffs will recharge at an accelerated rate in the energy field that radiates from this ring. Cursed rings of wisdom will instead cause your staffs to recharge more slowly."},
    {"reaping",         itemGems[6], "",	0,	700,	0,{1,3,1}, false, false, "The blood magic in this ring will recharge your staffs and charms in proportion to the damage you inflict directly. Cursed rings of reaping will drain your staffs and charms when you inflict damage directly."},
    {"propulsion",      itemGems[7], "",    6,  600,    0,{1,3,1}, false, false, "Wearing this ring will enable you to throw items far greater distances, especially useful for throwing weapons and deadly potions at monsters. In addition, any item thrown that is not shattered or destroyed has a chance to return to your pack, based upon the ring's enchantment level. Cursed rings will decrease your maximum throwing distance."},
    {"speed",           itemGems[8], "",    4,  600,    0,{1,3,1}, false, false, "This ring will incrementally enhance your movement and attack speed. Although the effect is relatively small, it may allow you to outpace a fearsome foe or catch up to a fleeing one, given enough time. Cursed rings will instead slow you down."},
    {"enchantment",     itemGems[9], "",    1,  980,    0,{1,3,1}, false, false, "If you are wearing a ring on your other finger, this ring will increase its effective enchantment level. Cursed rings will decrease the effective enchantment instead. Wearing two rings of enchantment will have no effect."},
};

// Bolt definitions

const bolt boltCatalog[NUMBER_BOLT_KINDS] = {
    {{0}},
    //name                      bolt description                ability description                         char    foreColor       backColor           boltEffect      magnitude       pathDF      targetDF    forbiddenMonsterFlags       flags
    {"teleportation spell",     "casts a teleport spell",       "can teleport other creatures",             0,      NULL,           &blue,              BE_TELEPORT,    10,             0,          0,          MONST_IMMOBILE,             (BF_TARGET_ENEMIES)},
    {"slowing spell",           "casts a slowing spell",        "can slow $HISHER enemies",                 0,      NULL,           &green,             BE_SLOW,        10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"polymorph spell",         "casts a polymorphism spell",   "can polymorph other creatures",            0,      NULL,           &purple,            BE_POLYMORPH,   10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"negation magic",          "casts a negation spell",       "can cast negation",                        0,      NULL,           &pink,              BE_NEGATION,    10,             0,          0,          0,                          (BF_TARGET_ENEMIES)},
    {"domination spell",        "casts a domination spell",     "can dominate other creatures",             0,      NULL,           &dominationColor,   BE_DOMINATION,  10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"beckoning spell",         "casts a beckoning spell",      "can cast beckoning",                       0,      NULL,           &beckonColor,       BE_BECKONING,   10,             0,          0,          MONST_IMMOBILE,             (BF_TARGET_ENEMIES)},
    {"spell of plenty",         "casts a spell of plenty",      "can duplicate other creatures",            0,      NULL,           &rainbow,           BE_PLENTY,      10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ALLIES | BF_NOT_LEARNABLE)},
    {"invisibility magic",      "casts invisibility magic",     "can turn creatures invisible",             0,      NULL,           &darkBlue,          BE_INVISIBILITY, 10,            0,          0,          MONST_INANIMATE,            (BF_TARGET_ALLIES | BF_NOT_LEARNABLE)},
    {"empowerment sorcery",     "casts empowerment",            "can cast empowerment",                     0,      NULL,           &empowermentColor,  BE_EMPOWERMENT, 10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ALLIES | BF_NOT_LEARNABLE)},
    {"lightning",               "casts lightning",              "can hurl lightning bolts",                 0,      NULL,           &lightningColor,    BE_DAMAGE,      10,             0,          0,          0,                          (BF_PASSES_THRU_CREATURES | BF_TARGET_ENEMIES | BF_ELECTRIC)},
    {"flame",                   "casts a gout of flame",        "can hurl gouts of flame",                  0,      NULL,           &fireBoltColor,     BE_DAMAGE,      4,              0,          0,          MONST_IMMUNE_TO_FIRE,       (BF_TARGET_ENEMIES | BF_FIERY)},
    {"poison ray",              "casts a poison ray",           "can cast poisonous bolts",                 0,      NULL,           &poisonColor,       BE_POISON,      10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"tunneling magic",         "casts tunneling",              "can tunnel",                               0,      NULL,           &brown,             BE_TUNNELING,   10,             0,          0,          0,                          (BF_PASSES_THRU_CREATURES)},
    {"blink trajectory",        "blinks",                       "can blink",                                0,      NULL,           &white,             BE_BLINKING,    5,              0,          0,          0,                          (BF_HALTS_BEFORE_OBSTRUCTION)},
    {"entrancement ray",        "casts entrancement",           "can cast entrancement",                    0,      NULL,           &yellow,            BE_ENTRANCEMENT,10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"obstruction magic",       "casts obstruction",            "can cast obstruction",                     0,      NULL,           &forceFieldColor,   BE_OBSTRUCTION, 10,             0,          0,          0,                          (BF_HALTS_BEFORE_OBSTRUCTION)},
    {"spell of discord",        "casts a spell of discord",     "can cast discord",                         0,      NULL,           &discordColor,      BE_DISCORD,     10,             0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"conjuration magic",       "casts a conjuration bolt",     "can cast conjuration",                     0,      NULL,           &spectralBladeColor, BE_CONJURATION,10,             0,          0,          MONST_IMMUNE_TO_WEAPONS,    (BF_HALTS_BEFORE_OBSTRUCTION | BF_TARGET_ENEMIES)},
    {"healing magic",           "casts healing",                "can heal $HISHER allies",                  0,      NULL,           &darkRed,           BE_HEALING,     5,              0,          0,          0,                          (BF_TARGET_ALLIES)},
    {"haste spell",             "casts a haste spell",          "can haste $HISHER allies",                 0,      NULL,           &orange,            BE_HASTE,       2,              0,          0,          MONST_INANIMATE,            (BF_TARGET_ALLIES)},
    {"slowing spell",           "casts a slowing spell",        "can slow $HISHER enemies",                 0,      NULL,           &green,             BE_SLOW,        2,              0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
    {"protection magic",        "casts protection",             "can cast protection",                      0,      NULL,           &shieldingColor,    BE_SHIELDING,   5,              0,          0,          MONST_INANIMATE,            (BF_TARGET_ALLIES)},
    {"spiderweb",               "launches a sticky web",        "can launch sticky webs",                   '*',    &white,         NULL,               BE_NONE,        10,             DF_WEB_SMALL, DF_WEB_LARGE, (MONST_IMMOBILE | MONST_IMMUNE_TO_WEBS),   (BF_TARGET_ENEMIES | BF_NEVER_REFLECTS | BF_NOT_LEARNABLE)},
    {"spark",                   "shoots a spark",               "can throw sparks of lightning",            0,      NULL,           &lightningColor,    BE_DAMAGE,      1,              0,          0,          0,                          (BF_PASSES_THRU_CREATURES | BF_TARGET_ENEMIES | BF_ELECTRIC)},
    {"dragonfire",              "breathes a gout of white-hot flame", "can breathe gouts of white-hot flame", 0,    NULL,           &dragonFireColor,   BE_DAMAGE,      18,             DF_OBSIDIAN, 0,         MONST_IMMUNE_TO_FIRE,       (BF_TARGET_ENEMIES | BF_FIERY | BF_NOT_LEARNABLE)},
    {"arrow",                   "shoots an arrow",              "attacks from a distance",                  WEAPON_CHAR, &gray,     NULL,               BE_ATTACK,      1,              0,          0,          MONST_IMMUNE_TO_WEAPONS,    (BF_TARGET_ENEMIES | BF_NEVER_REFLECTS | BF_NOT_LEARNABLE)},
    {"poisoned dart",           "fires a dart",                 "fires strength-sapping darts",             WEAPON_CHAR, &centipedeColor, NULL,         BE_ATTACK,      1,              0,          0,          0,                          (BF_TARGET_ENEMIES | BF_NEVER_REFLECTS | BF_NOT_LEARNABLE)},
    {"acid spray",              "sprays a stream of acid",      "sprays streams of acid",                   '*',    &acidBackColor, NULL,               BE_ATTACK,      1,              0,          0,          0,                          (BF_TARGET_ENEMIES | BF_NEVER_REFLECTS | BF_NOT_LEARNABLE)},
    {"growing vines",           "releases carnivorous vines into the ground", "conjures carnivorous vines", GRASS_CHAR, &tanColor,  NULL,               BE_NONE,        5,              DF_ANCIENT_SPIRIT_GRASS, DF_ANCIENT_SPIRIT_VINES, (MONST_INANIMATE | MONST_IMMUNE_TO_WEBS),   (BF_TARGET_ENEMIES | BF_NEVER_REFLECTS)},
    {"whip",                    "whips",                        "wields a whip",                            '*',    &tanColor,      NULL,               BE_ATTACK,      1,              0,          0,          MONST_IMMUNE_TO_WEAPONS,    (BF_TARGET_ENEMIES | BF_NEVER_REFLECTS | BF_NOT_LEARNABLE | BF_DISPLAY_CHAR_ALONG_LENGTH)},

    {"poisonous breath",        "breathes poisonous fumes", "can breathe poisonous fumes",                  0,    NULL,           &poisonColor,     BE_POISON,            5,              0,          0,         0,       (BF_TARGET_ENEMIES | BF_NOT_LEARNABLE)},
    {"force wave",              "fires a force wave",           "can emit a forceful wave",                 0,      NULL,           &beckonColor,            BE_FORCE,       2,              0,          0,          MONST_INANIMATE,            (BF_TARGET_ENEMIES)},
};

// Feat definitions

const feat featTable[FEAT_COUNT] = {
//    {"Pure Mage",       "Ascend without using fists or a weapon.", true},
//    {"Pure Warrior",    "Ascend without using a staff, wand or charm.", true},
    {"Bashful",         "Ascend without using fists or a weapon.", true},
    {"Skeptic",         "Ascend without using a staff, wand or charm.", true},
    {"Pacifist",        "Ascend without attacking a creature.", true},
    {"Archivist",       "Ascend without drinking a potion or reading a scroll.", true},
    {"Companion",       "Journey with an ally through 20 depths.", false},
    {"Specialist",      "Enchant an item up to or above +16.", false},
    {"Jellymancer",     "Obtain at least 90 jelly allies simultaneously.", false},
    {"Indomitable",     "Ascend without taking damage.", true},
    {"Mystic",          "Ascend without eating.", true},
    {"Dragonslayer",    "Kill a dragon with a melee attack.", false},
    {"Paladin",         "Ascend without attacking an unaware or fleeing creature.", true},
};

// Miscellaneous definitions

const char monsterBehaviorFlagDescriptions[32][COLS] = {
	"is invisible",								// MONST_INVISIBLE
	"is an inanimate object",					// MONST_INANIMATE
	"cannot move",								// MONST_IMMOBILE
	"",                                         // MONST_CARRY_ITEM_100
	"",                                         // MONST_CARRY_ITEM_25
	"",                                         // MONST_ALWAYS_HUNTING
	"flees at low health",						// MONST_FLEES_NEAR_DEATH
	"",											// MONST_ATTACKABLE_THRU_WALLS
	"corrodes weapons when hit",				// MONST_DEFEND_DEGRADE_WEAPON
	"is immune to weapon damage",				// MONST_IMMUNE_TO_WEAPONS
	"flies",									// MONST_FLIES
	"moves erratically",						// MONST_FLITS
	"is immune to fire",						// MONST_IMMUNE_TO_FIRE
	"",											// MONST_CAST_SPELLS_SLOWLY
	"cannot be entangled",						// MONST_IMMUNE_TO_WEBS
	"can reflect magic spells",                 // MONST_REFLECT_4
	"never sleeps",								// MONST_NEVER_SLEEPS
	"burns unceasingly",						// MONST_FIERY
	"is invulnerable",                          // MONST_INVULNERABLE
	"is at home in water",						// MONST_IMMUNE_TO_WATER
	"cannot venture onto dry land",				// MONST_RESTRICTED_TO_LIQUID
	"submerges",								// MONST_SUBMERGES
	"keeps $HISHER distance",					// MONST_MAINTAINS_DISTANCE
	"",											// MONST_WILL_NOT_USE_STAIRS
	"is animated purely by magic",				// MONST_DIES_IF_NEGATED
	"",                                         // MONST_MALE
	"",                                         // MONST_FEMALE
    "",                                         // MONST_NOT_LISTED_IN_SIDEBAR
    "moves only when activated",                // MONST_GETS_TURN_ON_ACTIVATION
    "",
    "",
};

const char monsterAbilityFlagDescriptions[32][COLS] = {
	"can induce hallucinations",				// MA_HIT_HALLUCINATE
	"can steal items",							// MA_HIT_STEAL_FLEE
	"can possess $HISHER summoned allies",		// MA_ENTER_SUMMONS
	"corrodes armor when $HESHE hits",			// MA_HIT_DEGRADE_ARMOR
	"can summon allies",						// MA_CAST_SUMMON
	"immobilizes $HISHER prey",					// MA_SEIZES
	"injects poison when $HESHE hits",			// MA_POISONS
	"",											// MA_DF_ON_DEATH
	"divides in two when struck",				// MA_CLONE_SELF_ON_DEFEND
	"dies when $HESHE attacks",					// MA_KAMIKAZE
	"recovers health when $HESHE inflicts damage",// MA_TRANSFERENCE
    "saps strength when $HESHE inflicts damage",// MA_CAUSE_WEAKNESS

    "attacks up to two opponents in a line",    // MA_ATTACKS_PENETRATE
    "attacks all adjacent opponents at once",   // MA_ATTACKS_ALL_ADJACENT
    "attacks enemies at a distance",            // MA_ATTACKS_EXTEND
    "avoids attacking in corridors in a group", // MA_AVOID_CORRIDORS
    "can blind opponents",                      // MA_HIT_BLINDS
    "can slow down opponents",                  // MA_HIT_SLOWS
};

const char monsterBookkeepingFlagDescriptions[32][COLS] = {
	"",											// MB_WAS_VISIBLE
	"is telepathically bonded with you",		// MB_TELEPATHICALLY_REVEALED
	"",											// MB_PREPLACED
	"",											// MB_APPROACHING_UPSTAIRS
	"",											// MB_APPROACHING_DOWNSTAIRS
	"",											// MB_APPROACHING_PIT
	"",											// MB_LEADER
	"",											// MB_FOLLOWER
	"",											// MB_CAPTIVE
	"has been immobilized",						// MB_SEIZED
	"is currently holding $HISHER prey immobile",// MB_SEIZING
	"is submerged",								// MB_SUBMERGED
	"",											// MB_JUST_SUMMONED
	"",											// MB_WILL_FLASH
	"is anchored to reality by $HISHER summoner",// MB_BOUND_TO_LEADER
};
