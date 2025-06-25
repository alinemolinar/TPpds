#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

//definir a matriz dos aliens
#define ALIEN_LINHAS 4
#define ALIEN_COLUNAS 7
//espacamento entre os aliens
#define ALIEN_EX 40  
#define ALIEN_EY 30

//largura da tela
const int SCREEN_W = 960;
//altura da tela
const int SCREEN_H = 540;
//para determinar o intervalo do relógio por frames por segundo
const float FPS = 100; 
//parte embaixo, onde a nave fica
const int GRASS_H = 60;

//NAVE
  const int NAVE_W = 100;
  const int NAVE_H = 50;

  typedef struct Nave {
	float x;
	float vel;
	int dir, esq;
	//ALLEGRO_COLOR cor;
  } Nave;

//ALIEN
  const int ALIEN_W = 50;
  const int ALIEN_H = 25;

  //velocidade do bloco de aliens
  float aliens_dx; 
  float aliens_dy; 

  typedef struct Alien {
	float x, y;
	float x_vel, y_vel;
	//ALLEGRO_COLOR cor;
	int acionado;
  } Alien;

  //BOLA (do tiro)
typedef struct Bola {
    float x, y;
    float vel;
    int acionado;
} Bola;

//desenhar a bola
void draw_bola(Bola bola) {
    if (bola.acionado) {
        al_draw_filled_circle(bola.x, bola.y, 3, al_map_rgb(255, 255, 255));
    }
}

//faz a bola subir 
void update_bola(Bola *bola) {
    if (bola->acionado) {
        bola->y -= bola->vel;
        if (bola->y < 0) {
            bola->acionado = 0;
        }
    }
}

//inicializa a nave
  void initNave(Nave *nave){
	nave->x = SCREEN_W/2;
	nave->vel = 3;
	nave->dir = 0;
	nave->esq = 0;
	//nave->cor = al_map_rgb(65,105,225);
  }

//fazer o cenário
void draw_scenario(int pontuacao, int recorde, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *fundo){
	//al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_bitmap(fundo, 0, 0, 0);

	al_draw_filled_rectangle(0, SCREEN_H - GRASS_H, 
		                     SCREEN_W, SCREEN_H,
                             al_map_rgb(0,0,0));
    
	//exibir a pontuacao e o recorde
    char t_pontuacao[40], t_recorde[40];
	sprintf(t_pontuacao, "Pontuação: %d", pontuacao);
	sprintf(t_recorde, "Recorde: %d", recorde);

	al_draw_text(fonte, al_map_rgb(255, 255, 255), 20, 10, 0, t_pontuacao);
    al_draw_text(fonte, al_map_rgb(255, 0, 0), SCREEN_W - 200, 10, 0, t_recorde);

}

//desenhar a nave
void draw_nave(Nave nave, ALLEGRO_BITMAP *img_nave){
	float y_base = SCREEN_H - GRASS_H;
	/* al_draw_filled_triangle(nave.x, y_base - NAVE_H,
		                    nave.x - NAVE_W/2, y_base,
							nave.x + NAVE_W/2, y_base,
                            nave.cor
	                        );
							*/
    al_draw_bitmap(img_nave, nave.x - NAVE_W / 2, y_base - NAVE_H - 10, 0);
}

//move a nave para a esquerda ou para a direita
void update_nave (Nave *nave){
	if(nave->dir && nave->x + nave->vel <= SCREEN_W){
		nave->x += nave->vel;
	}
	if(nave->esq && nave->x - nave->vel >= 0){
		nave->x -= nave->vel;
	}
}

//inicializar o alien
void initAlien(Alien *alien){
	alien->x = 0;
	alien->y = 0;
	alien->x_vel = 1;
	alien->y_vel = ALIEN_H;
	//alien->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
}

//desenhar o alien
void draw_alien(Alien alien, ALLEGRO_BITMAP *img_alien){
	  if (alien.acionado) {
     /* al_draw_filled_rectangle(alien.x, alien.y, 
                               alien.x + ALIEN_W, alien.y + ALIEN_H,
          				   alien.cor);
       */
	   al_draw_bitmap(img_alien, alien.x, alien.y, 0);
       }
}

//se bater na borda, os aliens descem, e move para a direcao atual
void update_alien(Alien *alien){

	if(alien->x + ALIEN_W + alien->x_vel > SCREEN_W || alien->x + alien->x_vel < 0){
		alien->y +=  alien->y_vel;
		alien->x_vel *= -1;
	}
	alien->x += alien->x_vel;
}

//para quando o alien colidir com o solo (jogo termina)
int colisao_alien_solo(Alien alien){
	if(alien.y + ALIEN_H > SCREEN_H - GRASS_H)
	return 1;
return 0;
}

//pro movimento dos aliens
void update_aliens(Alien aliens[ALIEN_LINHAS][ALIEN_COLUNAS]) {
    //para nao bater na borda
    int descer = 0;
    for (int i = 0; i < ALIEN_LINHAS; i++) {
        for (int j = 0; j < ALIEN_COLUNAS; j++) {
            Alien *a = &aliens[i][j];
            if ((a->x + ALIEN_W + aliens_dx > SCREEN_W) || (a->x + aliens_dx < 0)) {
                descer = 1;
                break;
            }
        }
        if (descer) break;
    }

    // se bater na borda, inverter direção e descer
    if (descer) {
        aliens_dx *= -1;
        for (int i = 0; i < ALIEN_LINHAS; i++) {
            for (int j = 0; j < ALIEN_COLUNAS; j++) {
                aliens[i][j].y += aliens_dy;
            }
        }
    }

    // mover todos os aliens na direção atual
    for (int i = 0; i < ALIEN_LINHAS; i++) {
        for (int j = 0; j < ALIEN_COLUNAS; j++) {
            aliens[i][j].x += aliens_dx;
        }
    }
}

//verificar a colisao da bola(tiro) com o alien
int colisao_bola_alien(Bola *bola, Alien *alien) {
    if (!alien->acionado || !bola->acionado) return 0;

    if (bola->x >= alien->x && bola->x <= alien->x + ALIEN_W &&
        bola->y >= alien->y && bola->y <= alien->y + ALIEN_H) {
        alien->acionado = 0;
        bola->acionado = 0;
        return 1;
    }
    return 0;
}

//colisao entre o alien e a nave(= terminar jogo)
int colisao_alien_nave(Alien alien, Nave nave) {
    float nave_y_base = SCREEN_H - GRASS_H/2;
    float nave_top = nave_y_base - NAVE_H;

    //considerando a nave um retangulo (para facilitar)
    float nave_esq = nave.x - NAVE_W / 2;
    float nave_dir = nave.x + NAVE_W / 2;

    if (alien.acionado &&
        alien.y + ALIEN_H >= nave_top &&
        alien.y <= nave_y_base &&
        alien.x + ALIEN_W >= nave_esq &&
        alien.x <= nave_dir) {
        return 1;
    }
    return 0;
}

int main (){
  //tela
  ALLEGRO_DISPLAY *display = NULL;
  //fila de eventos
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  //relógio
  ALLEGRO_TIMER *timer = NULL;
  
  //iniciar a pontuacao e o recorde
  int pontuacao = 0;
  int recorde = 0;

  //manter o recorde entre as execucoes
  FILE *arq = fopen("recorde.txt", "r");
  if (arq != NULL) {
    fscanf(arq, "%d", &recorde);
    fclose(arq);
  }

  //inicializa o allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

    //criar a tela 
    display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	//carregar a imagem de fundo
	ALLEGRO_BITMAP *fundo = al_load_bitmap("espaco.png");
    if (!fundo) {
    fprintf(stderr, "failed to load background image!\n");
    return -1;
    }

	//imagem do alien
	ALLEGRO_BITMAP *img_alien = al_load_bitmap("alien.png");

	//imagem da nave
    ALLEGRO_BITMAP *img_nave = al_load_bitmap("nave.png");

	//inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	

    //cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

    //cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

    //instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

	
	
	//carrega o arquivo fonteda fonte Arial e define que sera usado o tamanho 15 (segundo parametro)
    ALLEGRO_FONT *fonte = al_load_font("fonte.ttf", 15, 0);   
	if(fonte == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}


    //registra na fila os eventos de tela 
	al_register_event_source(event_queue, al_get_display_event_source(display));
    //registra na fila os eventos de teclado 
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse 
	al_register_event_source(event_queue, al_get_mouse_event_source());  
    //registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

    Nave nave;
	initNave(&nave);

	Bola bola;
	bola.acionado = 0;
    bola.vel = 6;

	Alien aliens[ALIEN_LINHAS][ALIEN_COLUNAS];
	aliens_dx = 1; 
    aliens_dy = ALIEN_H; 
	
	//inicializar a matriz de aliens
	for (int i = 0; i < ALIEN_LINHAS; i++){
		for (int j = 0; j < ALIEN_COLUNAS; j++){
		aliens[i][j].x = j * (ALIEN_W + ALIEN_EX);
        aliens[i][j].y = i * (ALIEN_H + ALIEN_EY);
        aliens[i][j].x_vel = 1;
        aliens[i][j].y_vel = ALIEN_H;
      //aliens[i][j].cor = al_map_rgb(rand() % 256, rand() % 256, rand() % 256);
		aliens[i][j].acionado = 1;
		}
	}

	

    int playing = 1;

    //inicia o temporizador
	al_start_timer(timer);

    while(playing){

    ALLEGRO_EVENT ev;
	//espera por um evento e o armazena na variavel de evento ev
	al_wait_for_event(event_queue, &ev);

    //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
	if(ev.type == ALLEGRO_EVENT_TIMER) {

		
        //aparecer o cenário
		draw_scenario(pontuacao, recorde, fonte, fundo);
        
		//atualiza o x da nave com a velocidade dela
		update_nave(&nave);
        
		//atualiza o movimento dos aliens
		update_aliens(aliens); 

		//atualiza a bola(tiro)
		update_bola(&bola);

		//desenhar a nave
		draw_nave(nave, img_nave);

        //desenhar a bola(tiro)
		draw_bola(bola);


        //movimento dos aliens (fim de jogo caso haja colisao 
		// com o solo ou com a nave)
        int colisao = 0;
        for (int i = 0; i < ALIEN_LINHAS; i++) {
            for (int j = 0; j < ALIEN_COLUNAS; j++) {
            draw_alien(aliens[i][j], img_alien);
			if (colisao_alien_solo(aliens[i][j]) || colisao_alien_nave(aliens[i][j], nave)) {
            colisao = 1;
            } 
            }
        }

		playing = !colisao;

		for (int i = 0; i < ALIEN_LINHAS; i++) {
            for (int j = 0; j < ALIEN_COLUNAS; j++) {
            if (colisao_bola_alien(&bola, &aliens[i][j])){
				pontuacao = pontuacao + 10;}
				if(pontuacao > recorde){
					recorde = pontuacao;
				}
            }
        }

		

		//atualiza a tela (quando houver algo para mostrar)
		al_flip_display();
			
		if(al_get_timer_count(timer)%(int)FPS == 0)
			printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
	}

    //se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		playing = 0;
	}
    
    //se o tipo de evento for um pressionar de uma tecla
	else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		//imprime qual tecla foi
	    printf("\ncodigo tecla: %d", ev.keyboard.keycode);
        // movimentar a nave a partir de teclas
		switch(ev.keyboard.keycode) {

			case ALLEGRO_KEY_A:
			nave.esq = 1;
			break;

			case ALLEGRO_KEY_D:
			nave.dir = 1;
			break;

			case ALLEGRO_KEY_SPACE:
            if (!bola.acionado) {
            bola.acionado = 1;
            bola.x = nave.x;
            bola.y = SCREEN_H - GRASS_H/2 - NAVE_H;
            }
    break;
		}
         
	}

	 //se o tipo de evento for soltar uma tecla
	else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
		//imprime qual tecla foi
	    printf("\ncodigo tecla: %d", ev.keyboard.keycode);
        // movimentar a nave a partir de teclas
		switch(ev.keyboard.keycode) {

			case ALLEGRO_KEY_A:
			nave.esq = 0;
			break;

			case ALLEGRO_KEY_D:
			nave.dir = 0;
			break;
		}
         
	}

    
    }

	al_clear_to_color(al_map_rgb(0, 0, 0));
	//exibir a pontuacao e recorde na tela
	char msg0[80], msg1[80], msg2[80];
	sprintf(msg0, "O JOGO ACABOU!");
    sprintf(msg1, "Pontuação: %d", pontuacao);
    sprintf(msg2, "Recorde: %d", recorde);
	al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 60, ALLEGRO_ALIGN_CENTER, msg0);
	al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 20, ALLEGRO_ALIGN_CENTER, msg1);
    al_draw_text(fonte, al_map_rgb(255, 196, 0), SCREEN_W / 2, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTER, msg2);

	al_flip_display();
	//ficar um tempo na tela
	al_rest(6.0);


	al_destroy_font(fonte);
    al_destroy_bitmap(img_alien);
    al_destroy_bitmap(img_nave);
	al_destroy_bitmap(fundo);
    al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	arq = fopen("recorde.txt", "w");
    if (arq != NULL) {
    fprintf(arq, "%d", recorde);
    fclose(arq);
    }

    return 0;
}