#include <stdio.h>
#include <ncurses.h>

#include "main.h"
#include "pid.h"

#define WIDTH 60
#define HEIGHT 15

int startx = 0;
int starty = 0;
WINDOW *menu_win, *info_win;

char *choices[] = {
	"	Utilizar referência do potenciômetro",
	"	Inserir temperatura referência",
	"	Sair",
};
int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);
void clear_window(WINDOW *window);
void new_temperature_reference();

void show_infos(float TA, float TI, float TE, float intensity, int potentiometer)
{
	clear_window(info_win);
	info_win = newwin(HEIGHT, WIDTH, 0, WIDTH);
	box(info_win, 0, 0);
	if (potentiometer)
	{
		mvwprintw(info_win, 4, 2, "Temperatura referência inserida do potenciômetro");
	}
	else
	{
		mvwprintw(info_win, 4, 2, "Temperatura referência inserida por linha de comando  ");
	}

	if (intensity > 0)
	{
		wattron(info_win, COLOR_PAIR(2));
		mvwprintw(info_win, 6, 15, "Ventoinha");
		wattroff(info_win, COLOR_PAIR(2));
		wattron(info_win, COLOR_PAIR(1));
		mvwprintw(info_win, 6, 30, "Resistor");
		wattroff(info_win, COLOR_PAIR(1));
	}
	else if (intensity <= -40)
	{
		wattron(info_win, COLOR_PAIR(1));
		mvwprintw(info_win, 6, 15, "Ventoinha");
		wattroff(info_win, COLOR_PAIR(1));
		wattron(info_win, COLOR_PAIR(2));
		mvwprintw(info_win, 6, 30, "Resistor");
		wattroff(info_win, COLOR_PAIR(2));
	}
	else
	{
		wattron(info_win, COLOR_PAIR(2));
		mvwprintw(info_win, 6, 15, "Ventoinha");
		wattroff(info_win, COLOR_PAIR(2));
		wattron(info_win, COLOR_PAIR(2));
		mvwprintw(info_win, 6, 30, "Resistor");
		wattroff(info_win, COLOR_PAIR(2));
	}

	mvwprintw(info_win, 9, 2, "TA: %4.2f", TA);
	mvwprintw(info_win, 10, 2, "TI: %4.2f TE: %4.2f", TI, TE);
	wrefresh(info_win);
}

void dashboard()
{
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak(); /* Line buffering disabled. pass on everything */
	curs_set(0);
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	menu_win = newwin(HEIGHT, WIDTH, 0, 0);
	keypad(menu_win, TRUE);
	refresh();
	do
	{
		print_menu(menu_win, highlight);
		c = wgetch(menu_win);
		switch (c)
		{
		case KEY_UP:
			if (highlight == 1)
				highlight = n_choices;
			else
				--highlight;
			break;
		case KEY_DOWN:
			if (highlight == n_choices)
				highlight = 1;
			else
				++highlight;
			break;
		case 10:
			choice = highlight;
			if (choice == 1)
			{
				set_reference_input(1, 0.0);
			}
			if (choice == 2)
			{
				clear_window(menu_win);
				menu_win = newwin(HEIGHT, WIDTH, 0, 0);
				new_temperature_reference();
				keypad(menu_win, TRUE);
				refresh();
			}
			else if (choice == 3)
			{
				finish();
			}
			break;
		default:
			refresh();
			break;
		}
	} while (1);
}

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for (i = 0; i < n_choices; ++i)
	{
		if (highlight == i + 1) /* High light the present choice */
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

void clear_window(WINDOW *window)
{
	wclear(window);
	wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(window);
	delwin(window);
}

void new_temperature_reference()
{
	WINDOW *local_window;
	float reference;
	int max_height, max_width;
	int _startx, _starty;

	getmaxyx(stdscr, max_height, max_width);

	_starty = (LINES - max_height) / 2;
	_startx = (COLS - max_width) / 2;
	refresh();

	local_window = newwin(HEIGHT, WIDTH, _starty, _startx);

	wattron(local_window, COLOR_PAIR(1));
	mvwprintw(local_window, _starty + 1, _startx + 1, "Insira a temperatura de referência:");
	wattroff(local_window, COLOR_PAIR(1));

	echo();
	wmove(local_window, _starty + 1, _startx + 38);
	wscanw(local_window, "%f", &reference);

	set_reference_input(0, reference);
}
