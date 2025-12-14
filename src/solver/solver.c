#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "../pretreatment/blocks_treatment.h"
#include "solver.h"

#define MAX_LIGNES 100
#define MAX_COLS 100

/**
 * @brief Solves a word search puzzle by finding a word in the grid
 * 
 * Reads the grid from a file, searches for the specified word in all 8 directions
 * (horizontal, vertical, and diagonal), and returns the pixel coordinates of the
 * word's start and end positions.
 * 
 * @param grid Path to the grid file
 * @param word Word to search for in the grid
 * @param grid_blocks Array of Rect structures representing character positions
 * @param results Array to store results [x_start, y_start, x_end, y_end]
 */
void solve_word(char* grid, char* word, struct Rect* grid_blocks,
                int* results)
{
    char grille[MAX_LIGNES][MAX_COLS];
    int nb_lignes = 0;
    int nb_colonnes = 0;
    char ligne[MAX_COLS];

    FILE *fichier = fopen(grid, "r");
    if (fichier == NULL)
    {
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier))
    {
        int longueur = strlen(ligne);
        if (ligne[longueur - 1] == '\n' || ligne[longueur - 1] == '\r')
        {
            ligne[longueur - 1] = '\0';
        }

        strcpy(grille[nb_lignes], ligne);

        for (int i = 0; grille[nb_lignes][i] != '\0'; i++)
        {
            grille[nb_lignes][i] = toupper(grille[nb_lignes][i]);
        }

        if ((int)strlen(ligne) > nb_colonnes)
        {
            nb_colonnes = strlen(ligne);
        }

        nb_lignes++;
    }
    fclose(fichier);

    char mot[100];
    strcpy(mot, word);
    for (int i = 0; mot[i] != '\0'; i++)
    {
        mot[i] = toupper(mot[i]);
    }
    int taille_mot = strlen(mot);

    int trouve = 0;
    int x_debut = -1, y_debut = -1;
    int x_fin = -1, y_fin = -1;

    for (int y = 0; y < nb_lignes; y++)
    {
        for (int x = 0; x < nb_colonnes; x++)
        {
            if (grille[y][x] == mot[0])
            {
                int ok = 1;
                for (int k = 1; k < taille_mot; k++)
                {
                    if (x + k >= nb_colonnes || grille[y][x + k] != mot[k])
                    {
                        ok = 0;
                        break;
                    }
                }
                if (ok == 1)
                {
                    trouve = 1;
                    x_debut = x;
                    y_debut = y;
                    x_fin = x + taille_mot - 1;
                    y_fin = y;
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (x - k < 0 || grille[y][x - k] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x - taille_mot + 1;
                        y_fin = y;
                    }
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (y + k >= nb_lignes ||
                            grille[y + k][x] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x;
                        y_fin = y + taille_mot - 1;
                    }
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (y - k < 0 || grille[y - k][x] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x;
                        y_fin = y - taille_mot + 1;
                    }
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (x + k >= nb_colonnes || y + k >= nb_lignes ||
                            grille[y + k][x + k] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x + taille_mot - 1;
                        y_fin = y + taille_mot - 1;
                    }
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (x - k < 0 || y - k < 0 ||
                            grille[y - k][x - k] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x - taille_mot + 1;
                        y_fin = y - taille_mot + 1;
                    }
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (x - k < 0 || y + k >= nb_lignes ||
                            grille[y + k][x - k] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x - taille_mot + 1;
                        y_fin = y + taille_mot - 1;
                    }
                }

                if (!trouve)
                {
                    ok = 1;
                    for (int k = 1; k < taille_mot; k++)
                    {
                        if (x + k >= nb_colonnes || y - k < 0 ||
                            grille[y - k][x + k] != mot[k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok == 1)
                    {
                        trouve = 1;
                        x_debut = x;
                        y_debut = y;
                        x_fin = x + taille_mot - 1;
                        y_fin = y - taille_mot + 1;
                    }
                }
            }

            if (trouve)
            {
                break;
            }
        }
        if (trouve)
        {
            break;
        }
    }

    if (trouve)
    {
        struct Rect begin = grid_blocks[nb_colonnes*y_debut+x_debut];
        struct Rect end = grid_blocks[nb_colonnes*y_fin+x_fin];

        results[0] = (begin.x+(begin.x+begin.w-1))/2;
        results[1] = (begin.y+(begin.y+begin.h-1))/2;

        results[2] = (end.x+(end.x+end.w-1))/2;
        results[3] = (end.y+(end.y+end.h-1))/2;
    }
    else
    {
        results[0] = 0;
        results[1] = 0;
        results[2] = 0;
        results[3] = 0;
    }
}

/**
 * @brief Searches for a word in the grid in all directions
 * 
 * @param grid 2D character grid
 * @param word Word to search for
 * @param rows Number of rows in the grid
 * @param cols Number of columns in the grid
 * @return int 1 if found, 0 otherwise
 */
