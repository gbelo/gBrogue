# gBrogue (v1.18.02.11)
gBrogue: A community-driven fork of Brogue

gBrogue is based on the roguelike game Brogue. It's mostly designed to build upon Brogue's particular advantages over other roguelikes. Brogue provides you with a lot of information, but not overwhelmingly so; is easy to navigate, use, and understand; and presents more varied and interesting level designs than many other roguelikes. This modest fork strives to crank these to 11 while also adding some twists and turns, with increments guided by community feedback.

It's currently based upon Brogue v1.7.4. gBrogue v1.17.10.24 marked the first version with a real "v1.0" tag, meaning that the design philosophy and overall approach is sound enough to build upon.

___

Changes from Brogue –
General feel / Overview:
 * Items have generally been streamlined, reduced in total number but enhanced in versatility. Many gameplay functions, such as throwing and applying/using items, are changed with that in mind.
 * You're given options. You can choose one of a few different special items on the very first floor to get the game going. You can choose to explore and find hidden areas to find more items. Potions, especially, are multipurpose. You can choose to allow your starting pet to accompany you and to take care of it, and/or you can free captured monsters along the way.
 * There's a focus on usability as well -- playing into what's already a major strength of Brogue -- in the UI, keyboard commands, and visual cues.
 * Features for new players include a page on the help screen with tips and tricks and "stealth range" visible by default.
 
Some of the following needs to be amended for the latest version, but it's generally correct.

Tweaks to existing items:
 * All wands have been removed. Staffs of entrancement, healing, haste, and protection are out. Some of these have had their functionality given to other items, such as scrolls or potions (e.g. you can throw a potion at a monster to speed it up or heal it). Some functionalities, such as domination and entrancement, are entirely unavailable.
 * Non-throwing items don't break when thrown at a creature. This is part of the emphasis on throwing. However, if we don't have enough strength to throw a particular weapon (whether it's a throwing weapon or otherwise), it will always miss its target.
 * Potions of darkness, descent, lichen, and hallucination have been removed. Darkness and hallucination take other forms..
 * Potion of life and strength are combined into a potion of empowerment, which can be consumed or thrown at an ally -- your choice.
 * Ring of light and ring of awareness are combined into a ring of perception.
 * Scroll of protect armor and protect weapon are out. The scroll of protect equipment will protect both.
 * Throwing a potion of telepathy at a monster will give you a one-way telepathic bond with it, allowing you to track its position permanently. Shattering it on the ground will give you a limited "magic map" around the point of impact.

New items:
 * The new staff of force acts like a weapon of force, but from a distance and with a greater variety of angles.
 * Potion of chaos is in. When drunk, you hallucinate. When thrown at a monster, you might polymorph it. When shattered on the ground, it releases a cloud of confusion.
 * Potion of respiration, which gives you some literal breathing room by making you temporarily immune to gases when drunk or dispelling gases when thrown.
 * The ring of propulsion boosts throw distance; the ring of speed increases speed; and the ring of enchantment affects the enchantment of the ring on your other hand.
 * Scroll of summon familiar summons an allied monster of the same depth that you're on
 * The rare scroll of great identify will identify all items in your pack.
 * The new armor, the cloak, is weak but often generated runic. Also, you start with one of these. Also, yours isn't special.
 * Telepathy and negation charms are out; fear and discord are in. Fear is lifted directly from UnBrogue (with credit, of course).
 * Kunai, club, chain whip, epee, and nunchaku are added as weaker or stronger versions (i.e. different "levels") of the dagger, mace and hammer, whip, rapier, and flail, respectively -- just like the axe and war axe, and sword and broadsword, are in vanilla.
 * Tranquilizer darts and poison darts have been added as throwing weapons.
 * Weapon runics slowing and confusion are out; poison and double-edge are in. New armor runics include force, shadows, frailty, and teleportation. The armor of shadows darkens you if you're up against a solid wall. The rest do what they say on the tin.

Item handling:
 * The frequency at which scrolls of enchantment are generated is slightly reduced.
 * Probabilities of different items spawning has been adjusted, biasing more toward consumables (such as potions and scrolls).
 * Potions of speed, invisibility, levitation, and healing can be thrown at creatures (to grant them the effects said on the tin). Namely, allies.
 * You're told if a weapon/armor you come across is runic, but not what the runic is -- or whether it's good or bad.
 * The amount of gold generated in piles is significantly increased. This change was made because, if you head back to the surface (even prematurely), you'll sell all of your items in the epilogue -- so gathering up all of the items on depths 1-2 and heading back up won't get you very far on the high score sheet. Of course, you can't compare any high scores from vanilla and from gBrogue.
 * Staff of tunneling has been buffed.
Gameplay:
 * There's a guaranteed library on the first floor, intended to help the early game along and possibly start a "build."
 * In accordance with throwing potions and the new throwing weapons, throwing distance has changed dramatically. It's generally shorter earlier on, but can be aided by additional strength and rings of propulsion.
 * Throwing weapons are in their own category. They can't be wielded or enchanted. A minor change, but in keeping with our theme.
 * Many adjustments to the relative frequencies of libraries, altars, and other special features. For example, the "throwing item over a chasm at switch" puzzle is out, many secret rooms are hidden behind secret doors
 * More reward rooms are strewn about the place, but many are hidden and require exploration.
 * So.. food/nutrition has been made slightly more forgiving to aid in exploration.
 * There's no longer a flat 25% chance for an unaware monster to notice you. There's a 30% chance if the monster is wandering, 20% if it's sleeping.
 * The player can leave the dungeon at any time by climbing back to the entrance. This is sort of a win, but a very flimsy win. It's astronomically better (for both your pride and your score) to grab the Amulet, or even to die at deeper levels, than to go this route.
 * Special levels have been added: a "Big Room" at D6, "Narrow Room" at D13, and.. ... well, a D39. The first of these is a large room where you are easily spotted but have a chance to grab a lot of items if you want to take some risks. The second pretty much tests your ability to get through a short level with exactly one route through it. The third is.. a D39.
 * Ogre armory is a new dungeon section that may appear, similar to goblin warrens. They're packed with ogres, but you can brave the danger and nab 1-2 weapons or sets of armor. One will be runic.
 * No pits on the amulet level (D26 to D27). There's only one way down!
 * Lightning reflects once (and only once) off of most surfaces. No big deal, but pretty cool once in a while.
 * Confusion traps have been replaced with steam traps.

New monsters and creatures:
 * You start with a pet dog. It has roughly the strength of a monkey, and you are immediately telepathically bonded to it -- but you're free to leave it behind or let it get killed.
 * The fellow adventurer is a "pseudo-ally," who is labeled "Peaceful" and does help you out and tries to kill monsters itself, but does not necessarily follow you around.
 * Various adjustments to monster stats. For example, will-o-the-wisps move faster; vampire bats move slower; rats can come in pretty big packs.
 * Additions include the goblin driver, goblin thief, glass golem, rope golem, lemmer, ink eel, magma eel, quickling, hellhound, and.. surprises. These surprises are mostly in the deeper levels, in the "post-game" after the Amulet level.
 * Also, a sinister presence in D39, just waiting for you to try for a mastery.

Changes to monster behavior:
 * The effective depth for new hordes/monsters spawning is an average of the current depth and the player's latest depth. This doesn't affect the descent, but it makes the ascent more interesting.
 * Some monsters can blind (imbue "darkness" status on) the player.
 * New mutations: jelly, floating, glass, everburning, berserk, and acidic.
 * Empowered monsters are healed for 50% rather than 100%.
 * Monsters are woken up if something is thrown at them. Enough said.
 * Those legendary allies? At least they're still legendary.
 * Changed around what skills could be learned by allies. Allies can now learn to swim, for example, but they can't learn how to make other allies invisible.

Usability enhancements:
 * Aggro/stealth range displays by default. (This is mainly so that new players can dive right in and know what stealth actually is.)
 * There's now a second page to the help screen, with some tips for new players.
 * In the UI, the portions of the names of items are colored so that they stand out. For example, "strength" in "potion of strength" will be teal. (.. or it would be if potions of strength were in the game.)
 * Messages are given when a monster begins to hunt you, falls asleep, begins to wander, and starts to flee. Monsters will flash subtly when they notice you.
 * Whenever the player enters a new depth, a slight flash helps indicate his position on the screen. Teleported monsters also flash just a bit.
 * Saved games aren't deleted upon load; but they are deleted upon death. This makes crashes less annoying. You know, just in case they occur.
 * Whenever an item screen is shown -- whether in the inventory menu, apply menu, etc. -- the number of item slots that remain in your pack is shown.
 * Most everything can be done via the 'a'pply command. Applying throwing weapons will allow you to throw them. Applying unequipped weapons, armor, and rings will equip them. Applying equipped weapons, armor, and rings will unequip them. Some of these entries might be grayed out in the menus, but that's by design.
 * HP and max HP of creatures is displayed in their bars. Hallucination makes these values freak out.
 * Some items, such as throwing items, stack in vanilla. This is made more apparent that, say, your 15 darts occupy the same slot (and are one single item for inventory purposes) by changing the phrasing to "A bundle of 15 darts."
 * Turrets have sparkly colors to make their existence more apparent (when you are supposed to "know" they exist anyway).
 * The multiplier for the color for dark and lit squares have been exaggerated, making it easier to tell what's dark or light or neither.
 * "This is a scroll of identify/enchanting" prompt is only given the one time.
 * Redid placement of items on the Discoveries screen. Of course.
 * Replaced the "View seed" (~) command with a "View session information" of sorts. It shows the seeds, but it also shows you which conducts/feats you've violated, which you have earned (pending ascension), and which are still in the running for you.

Extremely minor changes:
 * The title and main menu screen are different. Obviously.
 * Level messages for Narrow level, Big level, Nether level, and ... D39.
 * Adjusted staff charge durations.
 * Messages generated by monsters stepping on confusion traps no longer require you to acknowledge them. Moot point since confusion traps are out anyway.
 * Added "unbreathing"/"respirated" status.
 * Eating takes a turn or two, during which you're in "eating" status. It's much like paralyzation. But it's hardly visible or noticed at all, since eating is quick anyway.
 * Adjusted monsters in monster classes, mostly to account for new monsters.

Things that only "deep-divers" would ever care about:
 * There's an incomplete, dummied-out feature allowing you to to call monsters anything (e.g. "Your Ogre #1"...)
 * A unique ID number is generated for every monster. Doesn't do anything yet, but could feed into the above.
 * A staff of domination exists, but it's also dummied out in the code. A charm of identify exists but is dummied out.
 * There's a halfway completed functionality for dipping items into potions. Decided against it, but, hey, it's there.
 * Streamlined how the code manages deciding whether an item is "bad" and what icon is displayed for it in menus (i.e. good or bad magic).
 * The magnitude of stealth bonus provided by rings is broken out and made variable, rather than being dependent directly on enchantment level.
 * Force weapon hit routine is generalized to account for, say, staff of force.
 * Added a routine, much like the passableArcCount routine, that basically "executive summarizes" the situation of the player wrt walls. Used by armor of shadows.
 * And a lot more, hopefully documented well.

___
 
Changes since last version of gBrogue –
 * Fixed "pretty much always crashing after D30 or so" bug. Yes, it was a new monster that caused the problem. Replaced the hellhound with what might be a more interesting monster.
 * Replaced the "View seed" (~) command with a "View session information" of sorts. It shows the seeds, but it also shows you which conducts/feats you've violated, which you have earned (pending ascension), and which are still in the running for you.
 * Rebalanced throwing items. Throwing distance is now back up to snuff. Ring of propulsion is also made a somewhat useful item, in that it's pretty powerful at low enchants and worth keeping around for situational purposes.
 * Changed around what skills could be learned by allies. Allies can now learn to swim, for example, but they can't learn how to make other allies invisible.
 * Added nymphs. Inconsequential monster for veterans, possibly minorly interesting for newcomers.

To do:
 * Fix warnings in code.
 