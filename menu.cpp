#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#define barra_alt 13
#define altura_define 555
#define barra_larg 195
#define FPS 60.0
#define LARGURA_TELA 820
#define ALTURA_TELA 580
#define vel_barra 10
#define total_threads 3
#define AUMENTO_VELOCIDADE_BOLA 0.3
typedef pair<int,int> ii;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_BITMAP *imagem = NULL;
ALLEGRO_BITMAP *bloco_img = NULL;
ALLEGRO_BITMAP *barra_img = NULL;
ALLEGRO_BITMAP *bola_img = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

struct Ball{
    int x,y;
    double vel;
    int sent_x;
    int sent_y;
};
struct Block{
    bool flag;
    int x,y;
};
struct Barra{
    int x_in,x_out;
    int y;
};

vector<Block> campo;

Ball bola;
Barra barra;

int score;

void init(){
    al_init();
    al_init_image_addon();
    al_install_keyboard();
    timer = al_create_timer(1.0/FPS);
    if(!timer){
        fprintf(stderr,"Falha ao criar timer!\n");
        al_destroy_timer(timer);
        exit(-1);
    }
}

void destroy(){

    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
}

int flip_game(){

    al_draw_bitmap(imagem,0,0,0);
    al_draw_bitmap(barra_img,barra.x_in,barra.y,0);
    al_draw_bitmap(bola_img,bola.x,bola.y,0);
    
    for(int i=0;i<campo.size();i++){
        if(campo[i].flag){
            al_draw_bitmap(bloco_img,campo[i].x,campo[i].y,0);
        }
    }
    al_flip_display();

    return 0;
}

void registro_eventos(){

    fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos,al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos,al_get_keyboard_event_source());
    al_start_timer(timer);
}

bool busca(){
    bola.vel+=0.3;
    if(barra.x_in - 18 <= bola.x && barra.x_out >= bola.x + 18) return true;
    return false;
}

bool visit_all(){
    int num_id;
    int xa,xb,ya,yb;
    bool crash;
    int up_x= 0;
    int down_x =0;
    int up_y = 0;
    int down_y = 0;

    /*#pragma omp parallel num_threads(total_threads) private(num_id,xa,xb,ya,yb,crash)
    {
        crash = false;
        num_id = omp_get_thread_num();
        //fprintf(stderr,"thread %d\n",num_id);
        fprintf(stderr,"posx -> %d, posy -> %d\n",bola.x,bola.y);
        for(int i=num_id;i<112;i+=(112/total_threads)){
            if(campo[i].flag){
                xa = campo[i].x;
                xb = campo[i].x + 45;
                ya = campo[i].y;
                yb = campo[i].y + 30;

                if(bola.x >= xa && bola.x <= xb && bola.y >= ya && bola.y <= yb) crash = true;
                else if(bola.x + 33 >= xa && bola.x + 33 <= xb && bola.y >= ya && bola.y <= yb) crash = true;
                else if(bola.x + 33 >= xa && bola.x + 33 <= xb && bola.y + 33 >= ya && bola.y + 33 <= yb) crash = true;
                else if(bola.x >= xa && bola.x <= xb && bola.y + 33 >= ya && bola.y + 33 <= yb) crash = true;

                if(crash){
                    
                    #pragma omp atomic
                    score++;

                    campo[i].flag = false;
                    if(bola.sent_x == 1){ // esquerda
                        if(bola.x+33 == xa){
                            #pragma omp atomic
                            down_x++;
                        }
                    }
                    else{ //direita
                        if(bola.x == xb){
                            #pragma omp atomic
                            up_x++;
                        }
                    }

                    if(bola.sent_y == 1){ // baixo
                        if(bola.y + 33 == ya){ 
                            #pragma omp atomic
                            down_y++; 
                        }
                    }
                    else{ // cima
                        if(bola.y == yb){
                            #pragma omp atomic
                            up_y++;
                        }
                    }
                }
            }
        }
    }*/

    for(int i=0;i<112;i++){
        bool crash = false;
        if(campo[i].flag){
            xa = campo[i].x;
            xb = campo[i].x + 45;
            ya = campo[i].y;
            yb = campo[i].y + 30;

            if(bola.x >= xa && bola.x <= xb && bola.y >= ya && bola.y <= yb) crash = true;
            else if(bola.x + 33 >= xa && bola.x + 33 <= xb && bola.y >= ya && bola.y <= yb) crash = true;
            else if(bola.x + 33 >= xa && bola.x + 33 <= xb && bola.y + 33 >= ya && bola.y + 33 <= yb) crash = true;
            else if(bola.x >= xa && bola.x <= xb && bola.y + 33 >= ya && bola.y + 33 <= yb) crash = true;

            if(crash){
                
                ///#pragma omp atomic
                score++;

                campo[i].flag = false;
                if(bola.sent_x == 1){ // esquerda
                    if(bola.x+33 == xa){
                        ///#pragma omp atomic
                        down_x++;
                    }
                }
                else{ //direita
                    if(bola.x == xb){
                        //// #pragma omp atomic
                        up_x++;
                    }
                }

                if(bola.sent_y == 1){ // baixo
                    if(bola.y + 33 == ya){ 
                        /// #pragma omp atomic
                        down_y++; 
                    }
                }
                else{ // cima
                    if(bola.y == yb){
                        /// #pragma omp atomic
                        up_y++;
                    }
                }
            }
        }
    }

    if(up_y) bola.sent_y = 1;
    if(up_x) bola.sent_x = 1;
    if(down_x) bola.sent_x = -1;
    if(down_y) bola.sent_y = -1;

    bool piso = false;

    if(bola.x <= 0) bola.sent_x = 1;
    else if(bola.x + 33 >= LARGURA_TELA) bola.sent_x = -1;

    if(bola.y <= 0) bola.sent_y = 1;
    else if(bola.y >= (altura_define-33)){ bola.sent_y = -1; piso = true;}
    bool ret = true;
    if(piso){
       
        bola.vel += AUMENTO_VELOCIDADE_BOLA;
        if((bola.x >= barra.x_in && bola.x <= barra.x_out) || (bola.x + 33 >= barra.x_in && bola.x + 33 <= barra.x_out)){
            bola.sent_y = -1;

            if(bola.x <= barra.x_in + 40) bola.sent_x = -1;
            else if(bola.x >= barra.x_out - 40) bola.sent_x = 1;
            ret = true;
        }
        else ret = false;
    }

    return ret;
}
int cont = 0;
bool check_blocks(){

    int total = bola.vel;
    bool ret = true;
    bool aux;

    while(total > 0){
        aux = visit_all();
        if(!aux) ret = false;
        if(bola.sent_x < 0) bola.x--;
        else if(bola.sent_x > 0) bola.x++;
        if(bola.sent_y < 0) bola.y--;
        else if(bola.sent_y > 0) bola.y++;
        total--;
    }

    return ret;
}

bool flip_pos_bola(){

    return check_blocks();
}

void restart_game(){

    barra.x_in = 340;
    barra.x_out = 340+barra_larg;
    barra.y = altura_define;
    bola.x = 400;
    bola.y = 350;
    bola.vel = 3;
    bola.sent_x = 1;
    bola.sent_y = 1;

    campo.clear();
    Block aux; // criando blocos
    for(int i=0;i<14;i++){
        for(int j=0;j<8;j++){
            aux.x = 17+57*i;
            aux.y = 17+42*j;
            aux.flag = true;
            campo.push_back(aux);
        }
    }
}

int main(){

    init();
    
    /* carregando as imagens */
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    imagem = al_load_bitmap("image/background.jpg");
    barra_img = al_load_bitmap("image/barra.jpg");
    bola_img = al_load_bitmap("image/bola.png");
    bloco_img = al_load_bitmap("image/bloco.jpg");

    /* registro fila de eventos */
    //reset(1);

    restart_game();

    registro_eventos();
    flip_game();

    int level = 1;
    int botao;

    bool sair = false;
    int dir; // direcao que a barra esta indo, ajudara para identificar o movimento da bola
    while(!sair){
        //fprintf(stderr,"oi\n");
        int last = -1;
        score = 0;
        restart_game();
        flip_game();
        bool pressl = false;
        bool pressr = false;
        int press_any = false;
        while(!press_any && !sair){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos,&evento);
            
            if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                press_any = true; // start game 
            }
        }
        bool rodando = true;
        while(rodando){ // loop do jogo
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos,&evento);
            int ret= 0;
            if(evento.type == ALLEGRO_EVENT_TIMER){

                if(last != score){
                    fprintf(stderr,"score is %d\n",score);
                    last = score;
                }

                rodando = flip_pos_bola();
                //rodando = check_colision();

                dir = 0;
                if(pressl){
                    dir = 1;
                    if(barra.x_in > 10){
                        barra.x_in-=vel_barra;
                        barra.x_out-=vel_barra;
                    }
                }
                else if(pressr){
                    dir = 2;
                    if(barra.x_out < LARGURA_TELA-10){
                        barra.x_out+=vel_barra;
                        barra.x_in+=vel_barra;
                    }
                }

                if(pressl && pressr) dir = 0;

                ret = flip_game();
                if(ret == -1){
                    sair = true; // end of game
                    rodando = false;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_LEFT:
                        pressl = true;
                    break;
                    case ALLEGRO_KEY_RIGHT:
                        pressr = true;
                    break;
                    case ALLEGRO_KEY_ESCAPE:
                        sair = true;
                        rodando = false;
                    break;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_UP){
                switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_LEFT:
                        pressl = false;
                    break;
                    case ALLEGRO_KEY_RIGHT:
                        pressr = false;
                    break;
                    case ALLEGRO_KEY_ESCAPE:
                        sair = true;
                        rodando = false;
                    break;
                }
            }

            cont++;
        }

        if(!sair){
            imagem = al_load_bitmap("image/perder.jpeg");
            al_draw_bitmap(imagem,0,0,0);
            al_flip_display();
            al_rest(1.0); 
            imagem = al_load_bitmap("image/background.jpg"); // tela volta a ser o background 
        }  
    }

    destroy();

    return 0;
}
