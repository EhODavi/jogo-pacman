#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>

using namespace std;

const float FPS = 6;
const int SCREEN_W = 500;
const int SCREEN_H = 550;

enum MYKEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

//matriz definindo mapa do jogo: 1 representa paredes, 0 representa corredor
char MAPA[26][26] = {
	"1111111111111111111111111",
	"1000000001111111000000001",
	"1011111101111111011111101",
	"1011111100000000011111101",
	"1000000001111111000000001",
	"1110111101111111011110111",
	"1000111100001000011110001",
	"1010111111101011111110101",
	"1010000111001001110000101",
	"1011110111011101110111101",
	"1011110100000000010111101",
	"1000110001110111000110001",
	"1110111101110111011110111",
	"1000000001110111000000001",
	"1011111101110111011111101",
	"1011100000000000000011101",
	"1000001111110111111000001",
	"1111101110000000111011111",
	"1111101110111110111011111",
	"1000000000000000000000001",
	"1011101111110111111011101",
	"1000001111110111111000001",
	"1011100000010100000011101",
	"1000001111000001111000001",
	"1111111111111111111111111",
};

ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_EVENT_QUEUE* event_queue = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* wall = NULL;
ALLEGRO_BITMAP* pill = NULL;
ALLEGRO_BITMAP* ghost = NULL;
ALLEGRO_BITMAP* pacman = NULL;
int i = 15, j = 12; //posicao inicial do Pacman na matriz
int q = 20; //tamanho de cada celula no mapa
int posy = i * q;
int posx = j * q;

bool key[4] = {false, false, false, false};
bool redraw = true;
bool sair = false;
int pontos = 0;

void carregarMapa() {
	for(int i = 0; i < 26; i++) {
		for(int j = 0; j < 26; j++) {
			if(MAPA[i][j] == '1') {
				al_draw_bitmap(wall, j * q, i * q, 0);
			} else if(MAPA[i][j] == '0') {
				if(posx == j * q && posy == i * q) {
					MAPA[i][j] = 'a';
					if(posx != 12 * 20 || posy != 15 * 20) {
						pontos += 10;
					}
					
				} else {
					al_draw_bitmap(pill, j * q, i * q, 0);
				}
			}
			if((i == 1 && j == 1) || (i == 1 && j == 23) || (i == 23 && j == 1) || (i == 23 && j == 23)) {
				al_draw_bitmap(ghost, j * q, i * q, 0);
			}
		}
	}
	al_draw_textf(fonte, al_map_rgb(255, 255, 255), 5, 5, ALLEGRO_ALIGN_LEFT, "PONTOS: %d", pontos);
}

int inicializa() {
	if(!al_init()) {
		cout << "Falha ao carregar Allegro" << endl;
		return 0;
	}

	if(!al_install_keyboard()) {
		cout << "Falha ao inicializar o teclado" << endl;
		return 0;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		cout << "Falha ao inicializar o temporizador" << endl;
		return 0;
	}

	if(!al_init_image_addon()) {
		cout << "Falha ao iniciar al_init_image_addon!" << endl;
		return 0;
	}

	if (!al_init_font_addon()) {
		cout << "Falha ao iniciar al_init_font_addon!" << endl;
		return 0;
	}

	if (!al_init_ttf_addon()) {
		cout << "Falha ao inicializar add-on allegro_ttf";
		return 0;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		cout << "Falha ao inicializar a tela" << endl;
		al_destroy_timer(timer);
		return 0;
	}

	fonte = al_load_font("arial.ttf", 12, 0);
	if (!fonte) {
		cout << "Falha ao carregar a fonte!" << endl;
		al_destroy_display(display);
		return 0;
	}

	// mapa = al_load_bitmap("map.bmp");
	// if(!mapa)
	// {
	//     cout << "Falha ao carregar o mapa!" << endl;
	//     al_destroy_display(display);
	//     return 0;
	// }
	// al_draw_bitmap(mapa,0,0,0);

	pacman = al_load_bitmap("pacmanright.tga");
	if(!pacman) {
		cout << "Falha ao carregar o pacman!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(pacman, posx, posy, 0);

	event_queue = al_create_event_queue();
	if(!event_queue) {
		cout << "Falha ao criar a fila de eventos" << endl;
		al_destroy_display(display);
		al_destroy_timer(timer);
		return 0;
	}

	//cria as paredes do mapa e as pilulas e os fantasmas

	wall = al_create_bitmap(q, q);
	pill = al_load_bitmap("pill.tga");
	ghost = al_load_bitmap("ghost.tga");
	if(!wall || !pill) {
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	al_set_target_bitmap(wall);
	al_clear_to_color(al_map_rgb(0, 0, 255));
	al_set_target_bitmap(al_get_backbuffer(display));

	//****************************************************************//

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_flip_display();
	al_start_timer(timer);

	return 1;
}

int main(int argc, char** argv)
{
	if(!inicializa()) {
		return -1;
	}

	while(!sair && pontos != 2530) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			if(key[KEY_UP] && MAPA[i - 1][j] != '1') {
				i--;
				posy = i * q;
				pacman = al_load_bitmap("pacmanup.tga");
			}

			if(key[KEY_DOWN] && MAPA[i + 1][j] != '1') {
				i++;
				posy = i * q;
				pacman = al_load_bitmap("pacmandown.tga");
			}

			if(key[KEY_LEFT] && MAPA[i][j - 1] != '1') {
				j--;
				posx = j * q;
				pacman = al_load_bitmap("pacmanleft.tga");
			}

			if(key[KEY_RIGHT] && MAPA[i][j + 1] != '1') {
				j++;
				posx = j * q;
				pacman = al_load_bitmap("pacmanright.tga");
			}

			redraw = true;
		} else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		} else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch(ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = true;
					break;

				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = true;
					break;

				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = true;
					break;

				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = true;
					break;
			}
		} else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = false;
					break;

				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = false;
					break;

				case ALLEGRO_KEY_LEFT:
					key[KEY_LEFT] = false;
					break;

				case ALLEGRO_KEY_RIGHT:
					key[KEY_RIGHT] = false;
					break;

				case ALLEGRO_KEY_ESCAPE:
					sair = true;
					break;
			}
		}

		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));
			carregarMapa();
			// al_draw_bitmap(mapa,0,0,0);
			al_draw_bitmap(pacman, posx, posy, 0);
			al_flip_display();
		}
	}

	if(pontos == 2530) {
		fonte = al_load_font("arial.ttf", 48, 0);
		al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W / 2, 215, ALLEGRO_ALIGN_CENTRE, "VOCE VENCEU!");
		al_flip_display();
		al_rest(5.0);
	} 

	// al_destroy_bitmap(mapa);
	al_destroy_bitmap(wall);
	al_destroy_bitmap(pacman);
	al_destroy_bitmap(pill);
	al_destroy_bitmap(ghost);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}