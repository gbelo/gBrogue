#ifdef BROGUE_TCOD
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>
#include "libtcod.h"
#include "platform.h"

#if TCOD_TECHVERSION >= 0x01050103
#define USE_NEW_TCOD_API
#endif

extern playerCharacter rogue;
extern TCOD_renderer_t renderer;
extern short brogueFontSize;

struct mouseState {int x, y, lmb, rmb;};

static int isFullScreen = false;
static int hasMouseMoved = false;

static TCOD_key_t bufferedKey = {TCODK_NONE};
static struct mouseState brogueMouse = {0, 0, 0, 0};
static struct mouseState missedMouse = {-1, -1, 0, 0};

static int desktop_width, desktop_height;

static void loadFont(int detectSize)
{
	char font[60];

	if (detectSize) {
		int fontWidths[13] = {112, 128, 144, 160, 176, 192, 208, 224, 240, 256, 272, 288, 304}; // widths of the font graphics (divide by 16 to get individual character width)
		int fontHeights[13] = {176, 208, 240, 272, 304, 336, 368, 400, 432, 464, 496, 528, 528}; // heights of the font graphics (divide by 16 to get individual character height)

		const SDL_VideoInfo* vInfo = SDL_GetVideoInfo();
		int screenWidth = desktop_width = vInfo->current_w;
		int screenHeight = desktop_height = vInfo->current_h;

		// adjust for title bars and whatever -- very approximate, but better than the alternative
		screenWidth -= 6;
		screenHeight -= 48;

		if (brogueFontSize < 1 || brogueFontSize > 13) {
			for (
				brogueFontSize = 13;
				brogueFontSize > 1 && (fontWidths[brogueFontSize - 1] * COLS / 16 >= screenWidth || fontHeights[brogueFontSize - 1] * ROWS / 16 >= screenHeight);
				brogueFontSize--
			);
		}

	}

	sprintf(font, "fonts/font-%i.png", brogueFontSize);

	TCOD_console_set_custom_font(font, (TCOD_FONT_TYPE_GREYSCALE | TCOD_FONT_LAYOUT_ASCII_INROW), 0, 0);
	TCOD_console_init_root(COLS, ROWS, "gBrogue", false, renderer);

	TCOD_console_map_ascii_codes_to_font(0, 255, 0, 0);
	TCOD_console_set_keyboard_repeat(175, 30);
	TCOD_mouse_show_cursor(1);

	SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
}

static void gameLoop()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf ("Could not start SDL.\n");
		return;
	}

	loadFont(true);
	rogueMain();

	TCOD_console_delete(NULL);
}

static void tcod_plotChar(uchar inputChar,
			  short xLoc, short yLoc,
			  short foreRed, short foreGreen, short foreBlue,
			  short backRed, short backGreen, short backBlue) {

	TCOD_color_t fore;
	TCOD_color_t back;

	fore.r = (uint8) foreRed * 255 / 100;
	fore.g = (uint8) foreGreen * 255 / 100;
	fore.b = (uint8) foreBlue * 255 / 100;
	back.r = (uint8) backRed * 255 / 100;
	back.g = (uint8) backGreen * 255 / 100;
	back.b = (uint8) backBlue * 255 / 100;

	if (inputChar == STATUE_CHAR) {
		inputChar = 223;
	} else if (inputChar > 255) {
		switch (inputChar) {
#ifdef USE_UNICODE
			case FLOOR_CHAR: inputChar = 128 + 0; break;
			case CHASM_CHAR: inputChar = 128 + 1; break;
			case TRAP_CHAR: inputChar = 128 + 2; break;
			case FIRE_CHAR: inputChar = 128 + 3; break;
			case FOLIAGE_CHAR: inputChar = 128 + 4; break;
			case AMULET_CHAR: inputChar = 128 + 5; break;
			case SCROLL_CHAR: inputChar = 128 + 6; break;
			case RING_CHAR: inputChar = 128 + 7; break;
			case WEAPON_CHAR: inputChar = 128 + 8; break;
			case GEM_CHAR: inputChar = 128 + 9; break;
			case TOTEM_CHAR: inputChar = 128 + 10; break;
			case BAD_MAGIC_CHAR: inputChar = 128 + 12; break;
			case GOOD_MAGIC_CHAR: inputChar = 128 + 13; break;

			case DOWN_ARROW_CHAR: inputChar = 144 + 1; break;
			case LEFT_ARROW_CHAR: inputChar = 144 + 2; break;
			case RIGHT_ARROW_CHAR: inputChar = 144 + 3; break;
			case UP_TRIANGLE_CHAR: inputChar = 144 + 4; break;
			case DOWN_TRIANGLE_CHAR: inputChar = 144 + 5; break;
			case OMEGA_CHAR: inputChar = 144 + 6; break;
			case THETA_CHAR: inputChar = 144 + 7; break;
			case LAMDA_CHAR: inputChar = 144 + 8; break;
			case KOPPA_CHAR: inputChar = 144 + 9; break; // is this right?
			case CHARM_CHAR: inputChar = 144 + 9; break;
			case LOZENGE_CHAR: inputChar = 144 + 10; break;
			case CROSS_PRODUCT_CHAR: inputChar = 144 + 11; break;
#endif
			default: inputChar = '?'; break;
		}
	}
	TCOD_console_put_char_ex(NULL, xLoc, yLoc, (int) inputChar, fore, back);
}

static void initWithFont(int fontSize)
{
	loadFont(false);
	refreshScreen();
}

static boolean processSpecialKeystrokes(TCOD_key_t k, boolean text) {
	if (k.vk == TCODK_PRINTSCREEN) {
		// screenshot
		TCOD_sys_save_screenshot(NULL);
		return true;
	} else if ( (k.vk == TCODK_F12) || ((k.lalt || k.ralt) && (k.vk == TCODK_ENTER || k.vk == TCODK_KPENTER) )) {
		if (!isFullScreen) {
			int font_w, font_h;

			TCOD_sys_get_char_size(&font_w, &font_h);
			if (desktop_width < font_w * COLS || desktop_height < font_h * ROWS) {
				// refuse to go full screen if the font is too big
				return true;
			}
		}

		isFullScreen = !isFullScreen;
		TCOD_console_set_fullscreen(isFullScreen);
		return true;
	} else if ((k.vk == TCODK_PAGEUP
				|| ((!text) && k.vk == TCODK_CHAR && (k.c == '=' || k.c == '+')))
			   && brogueFontSize < 13) {

		if (isFullScreen) {
			TCOD_console_set_fullscreen(0);
			isFullScreen = 0;
		}

		brogueFontSize++;
		TCOD_console_delete(NULL);

		initWithFont(brogueFontSize);

		TCOD_console_flush();
		return true;
	} else if ((k.vk == TCODK_PAGEDOWN
				|| ((!text) && k.vk == TCODK_CHAR && k.c == '-'))
			   && brogueFontSize > 1) {

		if (isFullScreen) {
			TCOD_console_set_fullscreen(0);
			isFullScreen = 0;
		}

		brogueFontSize--;
		TCOD_console_delete(NULL);

		initWithFont(brogueFontSize);
		TCOD_console_flush();
		return true;
	}
	return false;
}


struct mapsymbol {
	int in_vk, out_vk;
	int in_c, out_c;
	struct mapsymbol *next;
};

static struct mapsymbol *keymap = NULL;

static void rewriteKey(TCOD_key_t *key, boolean text) {
	if (key->vk == TCODK_CHAR && (SDL_GetModState() & KMOD_CAPS)) {
		// cancel out caps lock
		if (!key->shift) {
			key->c = tolower(key->c);
		}
	}

	struct mapsymbol *s = keymap;
	while (s != NULL) {
		if (key->vk == s->in_vk) {
			if (s->in_vk != TCODK_CHAR || (!text && s->in_c == key->c)) {
				// we have a match!
				key->vk = s->out_vk;
				key->c = s->out_c;

				// only apply one remapping
				return;
			}
		}
		s = s->next;
	}
}

static void getModifiers(rogueEvent *returnEvent) {
	Uint8 *keystate = SDL_GetKeyState(NULL);
	returnEvent->controlKey = keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL];
	returnEvent->shiftKey = keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT];
}


// returns true if input is acceptable
static boolean processKeystroke(TCOD_key_t key, rogueEvent *returnEvent, boolean text)
{
	if (processSpecialKeystrokes(key, text)) {
		return false;
	}

	returnEvent->eventType = KEYSTROKE;
	getModifiers(returnEvent);
	switch (key.vk) {
		case TCODK_CHAR:
		case TCODK_0:
		case TCODK_1:
		case TCODK_2:
		case TCODK_3:
		case TCODK_4:
		case TCODK_5:
		case TCODK_6:
		case TCODK_7:
		case TCODK_8:
		case TCODK_9:
			returnEvent->param1 = (unsigned short) key.c;
			if (returnEvent->shiftKey && returnEvent->param1 >= 'a' && returnEvent->param1 <= 'z') {
				returnEvent->param1 += 'A' - 'a';
			}
			break;
		case TCODK_SPACE:
			returnEvent->param1 = ACKNOWLEDGE_KEY;
			break;
		case TCODK_ESCAPE:
			returnEvent->param1 = ESCAPE_KEY;
			break;
		case TCODK_UP:
			returnEvent->param1 = UP_ARROW;
			break;
		case TCODK_DOWN:
			returnEvent->param1 = DOWN_ARROW;
			break;
		case TCODK_RIGHT:
			returnEvent->param1 = RIGHT_ARROW;
			break;
		case TCODK_LEFT:
			returnEvent->param1 = LEFT_ARROW;
			break;
		case TCODK_ENTER:
			returnEvent->param1 = RETURN_KEY;
			break;
		case TCODK_KPENTER:
			returnEvent->param1 = ENTER_KEY;
			break;
		case TCODK_BACKSPACE:
			returnEvent->param1 = DELETE_KEY;
			break;
		case TCODK_TAB:
			returnEvent->param1 = TAB_KEY;
			break;
		case TCODK_KP0:
            returnEvent->param1 = NUMPAD_0;
            break;
		case TCODK_KP1:
            returnEvent->param1 = NUMPAD_1;
            break;
        case TCODK_KP2:
            returnEvent->param1 = NUMPAD_2;
            break;
        case TCODK_KP3:
            returnEvent->param1 = NUMPAD_3;
            break;
        case TCODK_KP4:
            returnEvent->param1 = NUMPAD_4;
            break;
        case TCODK_KP5:
            returnEvent->param1 = NUMPAD_5;
            break;
        case TCODK_KP6:
            returnEvent->param1 = NUMPAD_6;
            break;
        case TCODK_KP7:
            returnEvent->param1 = NUMPAD_7;
            break;
        case TCODK_KP8:
            returnEvent->param1 = NUMPAD_8;
            break;
        case TCODK_KP9:
            returnEvent->param1 = NUMPAD_9;
            break;
		default:
			return false;
	}
	return true;
}

static boolean tcod_pauseForMilliseconds(short milliseconds)
{
	TCOD_mouse_t mouse;
	TCOD_console_flush();
	TCOD_sys_sleep_milli((unsigned int) milliseconds);

	#ifdef USE_NEW_TCOD_API
	if (bufferedKey.vk == TCODK_NONE) {
		TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &bufferedKey, &mouse);
	} else {
		TCOD_sys_check_for_event(TCOD_EVENT_MOUSE, 0, &mouse);
	}
	#else
	if (bufferedKey.vk == TCODK_NONE) {
		bufferedKey = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
	}
	#endif

	if (missedMouse.lmb == 0 && missedMouse.rmb == 0) {
		mouse = TCOD_mouse_get_status();
		if (mouse.lbutton_pressed || mouse.rbutton_pressed) {
			missedMouse.x = mouse.cx;
			missedMouse.y = mouse.cy;
			if (mouse.lbutton_pressed) missedMouse.lmb = MOUSE_DOWN;
			if (mouse.rbutton_pressed) missedMouse.rmb = MOUSE_DOWN;
		}
	}

	return (bufferedKey.vk != TCODK_NONE || missedMouse.lmb || missedMouse.rmb);
}


#define PAUSE_BETWEEN_EVENT_POLLING		36//17

static void tcod_nextKeyOrMouseEvent(rogueEvent *returnEvent, boolean textInput, boolean colorsDance)
{
	boolean tryAgain;
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	uint32 theTime, waitTime;
	short x, y;

	TCOD_console_flush();

	key.vk = TCODK_NONE;

	if (noMenu && rogue.nextGame == NG_NOTHING) rogue.nextGame = NG_NEW_GAME;

	for (;;) {
		theTime = TCOD_sys_elapsed_milli();

		if (TCOD_console_is_window_closed()) {
			rogue.gameHasEnded = true; // causes the game loop to terminate quickly
			rogue.nextGame = NG_QUIT; // causes the menu to drop out immediately
			returnEvent->eventType = KEYSTROKE;
			returnEvent->param1 = ESCAPE_KEY;
			return;
		}

		tryAgain = false;

		if (bufferedKey.vk != TCODK_NONE) {
			rewriteKey(&bufferedKey, textInput);
			if (processKeystroke(bufferedKey, returnEvent, textInput)) {
				bufferedKey.vk = TCODK_NONE;
				return;
			} else {
				bufferedKey.vk = TCODK_NONE;
			}
		}

		if (missedMouse.lmb) {
			returnEvent->eventType = missedMouse.lmb;

			returnEvent->param1 = missedMouse.x;
			returnEvent->param2 = missedMouse.y;
			if (TCOD_console_is_key_pressed(TCODK_CONTROL)) {
				returnEvent->controlKey = true;
			}
			if (TCOD_console_is_key_pressed(TCODK_SHIFT)) {
				returnEvent->shiftKey = true;
			}

			missedMouse.lmb = missedMouse.lmb == MOUSE_DOWN ? MOUSE_UP : 0;
			return;
		}

		if (missedMouse.rmb) {
			returnEvent->eventType = missedMouse.rmb == MOUSE_DOWN ? RIGHT_MOUSE_DOWN : RIGHT_MOUSE_UP;

			returnEvent->param1 = missedMouse.x;
			returnEvent->param2 = missedMouse.y;
			if (TCOD_console_is_key_pressed(TCODK_CONTROL)) {
				returnEvent->controlKey = true;
			}
			if (TCOD_console_is_key_pressed(TCODK_SHIFT)) {
				returnEvent->shiftKey = true;
			}

			missedMouse.rmb = missedMouse.rmb == MOUSE_DOWN ? MOUSE_UP : 0;
			return;
		}

		if (!(serverMode || (SDL_GetAppState() & SDL_APPACTIVE))) {
			TCOD_sys_sleep_milli(100);
		} else {
			if (colorsDance) {
				shuffleTerrainColors(3, true);
				commitDraws();
			}
			TCOD_console_flush();
		}

		#ifdef USE_NEW_TCOD_API
		TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &key, &mouse);
		#else
		key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
		#endif

		rewriteKey(&key, textInput);
		if (processKeystroke(key, returnEvent, textInput)) {
			return;
		}

		mouse = TCOD_mouse_get_status();

		if (serverMode || (SDL_GetAppState() & SDL_APPACTIVE)) {
			x = mouse.cx;
			y = mouse.cy;
		} else {
			x = 0;
			y = 0;
		}

		if (
			mouse.lbutton_pressed || mouse.rbutton_pressed
			|| mouse.lbutton != brogueMouse.lmb || mouse.rbutton != brogueMouse.rmb
			|| brogueMouse.x !=x || brogueMouse.y != y) {

			returnEvent->param1 = x;
			returnEvent->param2 = y;

			getModifiers(returnEvent);

			if (mouse.lbutton_pressed) {
				if (!brogueMouse.lmb) {
					// we missed a mouseDown event -- better make up for it!
					missedMouse.x = x;
					missedMouse.y = y;
					missedMouse.lmb = MOUSE_UP;
					returnEvent->eventType = MOUSE_DOWN;
				} else {
					returnEvent->eventType = MOUSE_UP;
				}
			} else if (mouse.lbutton && !brogueMouse.lmb) {
				returnEvent->eventType = MOUSE_DOWN;
			} else {
				returnEvent->eventType = MOUSE_ENTERED_CELL;
			}

			if (mouse.rbutton_pressed) {
				if (!brogueMouse.rmb) {
					// we missed a mouseDown event -- better make up for it!
					missedMouse.x = x;
					missedMouse.y = y;
					missedMouse.rmb = MOUSE_UP;
					returnEvent->eventType = RIGHT_MOUSE_DOWN;
				} else {
					returnEvent->eventType = RIGHT_MOUSE_UP;
				}
			} else if (mouse.rbutton && !brogueMouse.rmb) {
				returnEvent->eventType = RIGHT_MOUSE_DOWN;
			}

			brogueMouse.x = x;
			brogueMouse.y = y;
			brogueMouse.lmb = mouse.lbutton;
			brogueMouse.rmb = mouse.rbutton;

			if (returnEvent->eventType == MOUSE_ENTERED_CELL && !hasMouseMoved) {
				hasMouseMoved = true;
			} else {
				return;
			}
		}

		waitTime = PAUSE_BETWEEN_EVENT_POLLING + theTime - TCOD_sys_elapsed_milli();

		if (waitTime > 0 && waitTime <= PAUSE_BETWEEN_EVENT_POLLING) {
			TCOD_sys_sleep_milli(waitTime);
		}
	}
}



// tcodkeys is derived from console_types.h
char *tcodkeys[] = {
	"NONE",
	"ESCAPE",
	"BACKSPACE",
	"TAB",
	"ENTER",
	"SHIFT",
	"CONTROL",
	"ALT",
	"PAUSE",
	"CAPSLOCK",
	"PAGEUP",
	"PAGEDOWN",
	"END",
	"HOME",
	"UP",
	"LEFT",
	"RIGHT",
	"DOWN",
	"PRINTSCREEN",
	"INSERT",
	"DELETE",
	"LWIN",
	"RWIN",
	"APPS",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"KP0",
	"KP1",
	"KP2",
	"KP3",
	"KP4",
	"KP5",
	"KP6",
	"KP7",
	"KP8",
	"KP9",
	"KPADD",
	"KPSUB",
	"KPDIV",
	"KPMUL",
	"KPDEC",
	"KPENTER",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"NUMLOCK",
	"SCROLLLOCK",
	"SPACE",
	"CHAR",
	NULL
};

static void tcod_remap(const char *input_name, const char *output_name) {
	// find input and output in the list of tcod keys, if it's there
	int i;
	struct mapsymbol *sym = malloc(sizeof(*sym));

	if (sym == NULL) return; // out of memory?  seriously?

	// default to treating the names as literal ascii symbols
	sym->in_c = input_name[0];
	sym->out_c = output_name[0];
	sym->in_vk = TCODK_CHAR;
	sym->out_vk = TCODK_CHAR;

	// but try to find them in the list of tcod keys
	for (i = 0; tcodkeys[i] != NULL; i++) {
		if (strcmp(input_name, tcodkeys[i]) == 0) {
			sym->in_vk = i;
			sym->in_c = 0;
			break;
		}
	}

	for (i = 0; tcodkeys[i] != NULL; i++) {
		if (strcmp(output_name, tcodkeys[i]) == 0) {
			sym->out_vk = i;
			sym->out_c = 0;
			break;
		}
	}

	sym->next = keymap;
	keymap = sym;
}

static boolean modifier_held(int modifier) {
	rogueEvent tempEvent;

	getModifiers(&tempEvent);
	if (modifier == 0) return tempEvent.shiftKey;
	if (modifier == 1) return tempEvent.controlKey;

	return 0;
}

static void notify_event(short eventId, int data1, int data2, const char *str1, const char *str2) {
  //Unused
}

struct brogueConsole tcodConsole = {
	gameLoop,
	tcod_pauseForMilliseconds,
	tcod_nextKeyOrMouseEvent,
	tcod_plotChar,
	tcod_remap,
	modifier_held,
	notify_event
};

#endif

