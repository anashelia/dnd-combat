DICE COMBAT SIMULATOR
=====================

A terminal-based combat simulator written in C that pits pairs of
creatures against each other in one-on-one battles, using a
Dungeons & Dragons-style d20 combat system with pseudo-random dice
rolls.


OVERVIEW
--------

The program reads a scenario file describing armor types, attack
types, and creatures. It then pairs the creatures off (1st vs 2nd,
3rd vs 4th, ...) and simulates a fight for each pair. For every
battle, the winner is printed as a single character (the first
letter of the winner's id), so the output is a compact summary
line, e.g. `HG` means the first creature won the first fight and
the third creature won the second fight.


WHAT IT DOES
------------

  * Parses a scenario file containing:
      - A list of ARMOR types (id and armor class).
      - A list of ATTACK types (id, dice count, dice type, damage
        bonus).
      - A list of CREATURES (id, hp, attack bonus, armor id,
        attack type id).

  * Links each creature to its armor and attack type by id.
    If an id is not found, a warning is printed.

  * Pairs creatures: (0 vs 1), (2 vs 3), (4 vs 5), ...

  * Simulates each fight, turn by turn:
      - The attacker rolls 1d20 + attack bonus.
      - If the result is >= the defender's armor class, the attack
        hits.
      - On a hit, damage is rolled as:
            (dice_count)d(dice_type) + damage_bonus
        and subtracted from the defender's HP.
      - The roles then swap (attacker becomes defender, and vice
        versa) and the fight continues until one creature's HP
        drops to 0 or below.

  * Prints the winner of each fight as the first letter of its id,
    followed by a newline at the end.


FILES
-----

  main.c
      The main program. Contains:
        - Data structures: Armor, AttackType, Creature.
        - Parsers: parse_armor, parse_attack_type, parse_creature.
        - fight(): the turn-based combat loop.
        - main(): file I/O, memory management, and pairing logic.

  dice.h
      A small header-only dice library. Contains:
        - DiceType enum (D4, D6, D8, D10, D12, D20, D100).
        - roll(DiceType dice, int count, int bonus): rolls `count`
          dice of the given type, adds `bonus`, and returns the
          total. Uses an xorshift pseudo-random generator seeded
          with a fixed constant, so runs are reproducible.

  input1.txt, input2.txt, input3.txt
      Example scenario files of varying size.

  sample.txt
      A small, hand-written scenario file useful for testing.


SCENARIO FILE FORMAT
--------------------

The file is read in four sections:

  1. Number of armor types, followed by that many armor entries:
         <armor_id> <armor_class>

  2. Number of attack types, followed by that many attack entries:
         <attack_id> <dice_count> <dice_type> <damage_bonus>
     where <dice_type> is one of: D4 D6 D8 D10 D12 D20 D100

  3. Number of creatures, followed by that many creature entries:
         <creature_id> <hp> <attack_bonus> <armor_id> <attack_id>

  4. (No further sections.)

Example (from sample.txt):

     5
     none       10
     leather    11
     studded    12
     chainmail  16
     plate      18
     3
     dagger     1 D4  2
     longsword  1 D8  3
     firebolt   1 D10 3
     4
     Hero     30 6 plate    longsword
     Goblin   12 4 none     dagger
     Mage     16 5 none     firebolt
     Orc      20 5 leather  longsword

With this input, the program runs two fights:
     Hero   vs Goblin
     Mage   vs Orc
and prints a two-character result line.


BUILDING
--------

Only main.c needs to be compiled (dice.h is header-only):

    gcc main.c -o combat

On Windows with MSVC:

    cl main.c


RUNNING
-------

    .\combat.exe sample.txt

Examples:

    .\combat.exe input1.txt
    .\combat.exe input2.txt
    .\combat.exe input3.txt

The program prints one character per fight, e.g.:
    HG

Meaning: the first creature won fight 1, and the third creature
won fight 2.


NOTES
-----

  * The dice roll generator in dice.h uses a fixed initial seed, so
    the same scenario file always produces the same results. To get
    different outcomes, change the seed value in dice.h.

  * If an unknown armor or attack type id appears in the creature
    list, the program prints a warning but continues; the affected
    fields are left uninitialized, which is undefined behavior. Use
    valid ids to avoid surprises.

  * If the number of creatures is odd, the final unpaired creature
    is simply ignored (the pairing loop requires i + 1).

  * Memory allocated for armors, attack types, and creatures is
    freed before exit.


LICENSE
-------

This program is provided as-is for educational and demonstration
purposes. Use and modify it freely.