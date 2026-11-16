#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include "dice.h"

typedef struct {
    char id[20];
    int armor_class;
}Armor;

typedef struct {
    char id[20];
    int dice_count;
    DiceType dice_type;
    int damage_bonus;
}AttackType;

typedef struct {
    char id[20];
    int hp;
    int attack_bonus;
    Armor armor;
    AttackType attack_type;
} Creature;

Armor parse_armor(FILE *fp) {
    Armor a; 
    fscanf(fp, "%19s %d", a.id, &a.armor_class);
    return a;
}

AttackType parse_attack_type(FILE *fp){
    AttackType at;
    char dice_str[32];
    fscanf(fp, "%19s %d %7s %d", at.id, &at.dice_count, dice_str, &at.damage_bonus);

    if (strcmp(dice_str, "D4") == 0) {
        at.dice_type = D4;
    }else if (strcmp(dice_str, "D6") == 0) {
        at.dice_type = D6;
    }else if (strcmp(dice_str, "D8") == 0) {
        at.dice_type = D8;
    }else if (strcmp(dice_str, "D10") == 0) {
        at.dice_type = D10;
    }else if (strcmp(dice_str, "D12") == 0) {
        at.dice_type = D12;
    }else if (strcmp(dice_str, "D20") == 0) {
        at.dice_type = D20;
    }else if (strcmp(dice_str, "D100") == 0) {
        at.dice_type = D100;
    }
    return at;
}

Creature parse_creature(FILE *fp, Armor armors[], int armor_count, AttackType attack_types[], int attack_type_count) {
    Creature c;
    char armor_id[31];
    char attack_type_id[32];
    int armor_found;
    int attack_type_found;

    fscanf(fp, "%19s %d %d %19s %19s", c.id, &c.hp, &c.attack_bonus, armor_id, attack_type_id);
    for (int i = 0; i < armor_count; i++) {
        if (strcmp(armor_id, armors[i].id) == 0) {
            c.armor = armors[i];
            armor_found = 1;
            break;
        }
    }
    if (!armor_found) {
        printf("Armor not found for creature\n");
    }


    for (int i = 0; i < attack_type_count; i++) {
        if (strcmp(attack_type_id, attack_types[i].id) == 0) {
            c.attack_type = attack_types[i];
            attack_type_found = 1;
            break;
        }
    }
    if (!attack_type_found) {
        printf("Attack type not found for creature\n");
    }
    return c;
}

Creature *fight(Creature *c1, Creature *c2) {
    Creature *attacker = c1;
    Creature *defender = c2;

    while (1) {
        int attack_roll = roll(D20, 1, attacker->attack_bonus);

        if (attack_roll >= defender->armor.armor_class) {
            int damage = roll(attacker->attack_type.dice_type,
                              attacker->attack_type.dice_count,
                              attacker->attack_type.damage_bonus);

            defender->hp -= damage;

            if (defender->hp <= 0) {
                return attacker;
            }
        }

        Creature *temp = attacker;
        attacker = defender;
        defender = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    int armor_count;
    if (fscanf(fp, "%d", &armor_count) != 1) {
        printf("Errors\n");
        fclose(fp);
        return 1;
    }

    Armor *armors = malloc(sizeof(Armor) * armor_count);
    if (armors == NULL) {
        printf("Memory allocation failed for armors\n");
        fclose(fp);
        return 1;
    }

    for (int i = 0; i < armor_count; i++) {
        armors[i] = parse_armor(fp);
    }


    int attack_type_count;
    if (fscanf(fp, "%d", &attack_type_count) != 1) {
        printf("Failed to read attack type count\n");
        free(armors);
        fclose(fp);
        return 1;
    }

    AttackType *attack_types = malloc(sizeof(AttackType) * attack_type_count);
    if (attack_types == NULL) {
        printf("Memory allocation failed for attack types\n");
        free(armors);
        fclose(fp);
        return 1;
    }

    for (int i = 0; i < attack_type_count; i++) {
        attack_types[i] = parse_attack_type(fp);
    }

    int creature_count;
    if(fscanf(fp, "%d", &creature_count) != 1) {
        printf("Failed to read creature count\n");
        free(attack_types);
        fclose(fp);
        return 1;
    }

    Creature *creatures = malloc(sizeof(Creature) * creature_count);
    if (creatures == NULL) {
        printf("Memory allocation failed for creatures\n");
        free(attack_types);
        free(armors);
        fclose(fp);
        return 1;
    }

    for (int i = 0; i < creature_count; i++) {
        creatures[i] = parse_creature(fp, armors, armor_count, attack_types, attack_type_count);
    }

    fclose(fp);

    for (int i = 0; i + 1 < creature_count; i += 2) {
        Creature *winner = fight(&creatures[i], &creatures[i + 1]);
        printf("%c", winner->id[0]);
    }

    printf("\n");

    free(armors);
    free(attack_types);
    free(creatures);
    return 0;
}

