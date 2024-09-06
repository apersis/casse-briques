// gcc -o main main.c function.c -lSDL2 -lm
// ./main

#include <SDL2/SDL.h>
#include <unistd.h>
#include "function.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LBRIQUE 50 // longueur d'une brique
#define HBRIQUE 40 // hauteur d'une brique
#define NBRIQUE 13 // comien de briques par lignes
#define NLIGNES 5 // combien de colonnes de briques
#define SPACE 5 // espace entre chaque briques
#define TAILLE_PADDLE 100
#define TAILLE_BALL 10
#define FPS 60
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 920
#define HAUTEUR_PADDLE 800
#define SPEED_BALL 4
#define VIE 5

void drawBricks();
void drawPaddle();
void drawBonus();
void movementPaddle();
void game();
void initDrawBricks();
void initBonus();
int couleur();
int rouge();
int vert();
int bleu();
void movementBall();
int finJeu();
void overlay();
void rebondBord();
void perteVie();
void collisionBriques();
void collisionPaddle();
void initDrawFlocon();
void drawFlocon();

int gdpaddle=310;
double ball_y=780;
double ball_x=350;
double speed_ball_x=0;
double speed_ball_y=0;
double speed_ball=SPEED_BALL;
int r,v,b,code;
int jeu=0;
int vie=VIE;
int taille_paddle=TAILLE_PADDLE;
int debut=0;
int nbonus=0;
int tour=0;
int vie_enleve=0;
double fps=FPS;

struct paddle
{
	int pos_x;
	int pos_y;
	int longueur;
	int hauteur;
	int couleur;
};

struct brick
{
	int pos_x; //gauche
	int pos_x2; //droite
	int pos_y; //haut
	int pos_y2; //bas
	int longueur;
	int hauteur;
	int couleur;
	int vie; // 0 = n'a plus de vie = n'est plus visible
	int bonus; //0 pour aucun
};
struct ball
{
	int pos_x;
	int pos_y;
	int taille;
	int couleur;
	int vitesse;
};
struct bonus_sprite
{
	int pos_x;
	int pos_y;
	int type; // 0 pour pas de bonus, 1 pour ralentis ball, 2 pour aggrandi paddle, 3 pour accelere ball, 4 pour retrecis paddle 
	int etat; //0 pour pas encore apparu, 1 pour en train de tomber, 2 pour touché, 3 pour ratté, 4 pour deja utilisé
	char *texture;
};
struct neige
{
	int pos_x;
	int pos_y;
	int etat; // 0 pour au sol, 1 pour tombe et 2 pour pas encore a l'ecran
	int direction;
};
struct neige flocon[1000];
struct bonus_sprite bonus_level1[NLIGNES*NBRIQUE];
struct brick level1[NLIGNES][NBRIQUE];

//position du carre dessiné dans drawGame()
int x = 0;
int y = 0;

void drawGame(){
    /* Ici je dessine mon jeu
     * exemple position x, y modifiés dans KeyPressed() et utilisés pour
     * pouvoir deplacer la figure à chaque boucle de gameLoop()
     */
    clear();
	//drawFlocon();
    if(speed_ball_x==0)
		ball_x=gdpaddle+taille_paddle/2;
	if (debut==0)
		sprite(0,330,"Start.bmp");		
	changeColor(255, 255, 0);
	drawPaddle();
	drawBricks();
	drawBonus();
	movementBall();
	overlay();
	finJeu();
    actualize();
    usleep(1000000 / fps); // 60 images par seconde | 1000000 = 1 seconde
}
void KeyPressed(SDL_Keycode touche){
    /** @brief event.key.keysym.sym renvoi la touche appuyé
     *
     */
    switch (touche) {
        // Voir doc SDL_Keycode pour plus de touches https://wiki.libsdl.org/SDL_Keycode
        // ici exemple ou appuyer sur les touches Q | D modifie x (position du carré dans drawGame())
        case SDLK_q:
            //touche q appuyé
            //printf("Q\n");
            if (gdpaddle>=0)
				gdpaddle=gdpaddle-20;
			if(gdpaddle<0)
				gdpaddle=0;
            break;
        case SDLK_d:
		{
            //touche d appuyé
            //printf("D\n");
            if (gdpaddle<=WINDOW_WIDTH-taille_paddle)
				gdpaddle=gdpaddle+20;
			if(gdpaddle>=WINDOW_WIDTH-taille_paddle)
				gdpaddle=WINDOW_WIDTH-taille_paddle;
            break;
        }
        case SDLK_ESCAPE:
            freeAndTerminate();
            break;
        case SDLK_o: //double la vitesse de la balle
			speed_ball_x=speed_ball_x*2;
			speed_ball_y=speed_ball_y*2;
			speed_ball=speed_ball*2;
			break;
		case SDLK_l: // diminue de moitier la vitesse de la balle
			if (speed_ball_x>0.5)
			{
				speed_ball_x=speed_ball_x*0.5;
				speed_ball_y=speed_ball_y*0.5;
				speed_ball=speed_ball*0.5;
			}
			break;
		case SDLK_SPACE:
		{
			speed_ball_x=-SPEED_BALL/(2/sqrt(2));
			speed_ball_y=-SPEED_BALL/(2/sqrt(2));
			debut=1;
			break;
		}
		case SDLK_p: //aggrandis le paddle
			if (taille_paddle<=WINDOW_WIDTH-20)
			{
				taille_paddle=taille_paddle+10;
				gdpaddle=gdpaddle-5;
			}
			if (taille_paddle+gdpaddle>=WINDOW_WIDTH)
				gdpaddle=gdpaddle-10;
			if (gdpaddle<=0)
				gdpaddle=gdpaddle+10;
			break;
		case SDLK_m: //retrecis le paddle
			if (taille_paddle>10)
			{
				taille_paddle=taille_paddle-10;
				gdpaddle=gdpaddle+5;
			}
			break;
		case SDLK_v: //met le jeu a 0 fps
			fps=0.0001;
			printf("ok");
			break;
		case SDLK_b: //met le jeu a 1 fps
			fps=1;
			break;
		case SDLK_n: //met le jeu a 60 fps
			fps=60;
			break;
        default:
            break;
    }
}
void joyButtonPressed(){
}

void gameLoop() {
    int programLaunched = 1;
    while (programLaunched == 1) {
        // Boucle pour garder le programme ouvert
        // lorsque programLaunched est different de 1
        // on sort de la boucle, donc de la fonction
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // boucle pour la gestion d'évenement
            switch (event.type) {
                /** event.type renvoi le type d'evenement qui se passe
                 * (si on appuie sur une touche, clique de souris...)
                 * en fonction du type d'évènement on à alors
                 * différentes information accessibles
                 * voir doc pour plus d'event https://wiki.libsdl.org/SDL_EventType
                 */
                case SDL_QUIT:
                    // quand on clique sur fermer la fénêtre en haut à droite
                    programLaunched = 0;
                    break;
                case SDL_MOUSEBUTTONUP:
                    /* clique de la souris
                     * event.motion.y | event.motion.x pour les positions de la souris
                     */
                    printf("position de la souris x : %d , y : %d\n", event.motion.x, event.motion.y);
                    break;
                case SDL_KEYDOWN:
				{
					//lastKeyPressed(&event);
                    KeyPressed(event.key.keysym.sym);
                    break;
				}
                case SDL_JOYBUTTONDOWN:
                    break;
                default:
                    break;
            }
        }
        /* coder ici pour que le code s'execute après chaque évenement
         * exemple dessiner un carré avec position int x, int y qu'on peut
         * deplacer lorsqu'on appuie sur une touche
         */
        drawGame();
        if (vie==0)
        {
			programLaunched=0;
			jeu=3;
		}
        if (finJeu()==0)
        {
			programLaunched=0;
			jeu=2;
		}
    }
}

int main(){
    /** @description 3 fonctions dans le main qui permettent de créer l'application et la maintenir ouverte :
     *  init(...) : initialiser la SDL/ fenêtre
     *  gameLoop() : boucle de jeu dans laquelle l'application reste ouverte
     *  freeAndTerminate() : quitte le programme proprement
     */
    init(WINDOW_WIDTH, WINDOW_HEIGHT);
    srand(time(NULL));
    changeColor(255, 255, 255);
    drawCircle(ball_x, ball_y, TAILLE_BALL);
    initDrawBricks();
	//initDrawFlocon();
    initBonus();
    drawPaddle();
    gameLoop();
    clear();
    int i;
    if (jeu==2)
    {
		for (i=0;i<999;i++)
		{
			sprite(0,0,"Felicitation.bmp");
			actualize();
		}
		printf("FÉLICITATION, VOUS AVEZ GAGNÉ\n");
	}
	if (jeu==3)
	{
		for (i=0;i<999;i++)
		{
			sprite(0,0,"GameOver.bmp");
			actualize();
		}
		printf("GAME OVER, VOUS N'AVEZ PLUS DE VIES\n");
    }
    if (jeu==0)
		printf("Fin du programme\n");
	freeAndTerminate();
}
/*void initDrawFlocon()
{
	int i;
	for (i=0;i<1000;i++)
	{
		flocon[i].pos_x=rand()%720;
		flocon[i].pos_y=0;
		flocon[i].etat=2;
		flocon[i].direction=(rand()%60)+240;
	}
}/*

/*void drawFlocon()
{
	int i;
	changeColor(255,255,255);
	int radian=0;
	for (i=0;i<1000;i++)
	{
		if (flocon[i].etat==1)
		{
			radian=flocon[i].direction*180/M_PI
			flocon[i].pos_x=
		}
		if (flocon[i].etat<2)
			drawCircle(flocon[i].pos_x,flocon[i].pos_y,1)
	}
}*/

void drawBricks()//dessine les briques avec couleur
{
	int i=0;
	int j=0;
	for (j=0;j<NLIGNES;j++)
	{
		for (i=0;i<NBRIQUE;i++)
		{
			if (level1[j][i].vie>0)
			{
				changeColor(rouge(level1[j][i].couleur),vert(level1[j][i].couleur),bleu(level1[j][i].couleur));
				drawRect(level1[j][i].pos_x,level1[j][i].pos_y,level1[j][i].longueur,level1[j][i].hauteur);
				if (level1[j][i].vie==1)
					sprite(level1[j][i].pos_x,level1[j][i].pos_y,"1.bmp");
				if (level1[j][i].vie==2)
					sprite(level1[j][i].pos_x,level1[j][i].pos_y,"2.bmp");
				if (level1[j][i].vie==3)
					sprite(level1[j][i].pos_x,level1[j][i].pos_y,"3.bmp");
				if (level1[j][i].vie==4)
					sprite(level1[j][i].pos_x,level1[j][i].pos_y,"4.bmp");
				if (level1[j][i].vie==5)
					sprite(level1[j][i].pos_x,level1[j][i].pos_y,"5.bmp");
			}
		}
	}
}

void initDrawBricks() //intialise le tableau de brique
{
	int i=0;
	int j=0;
	for (j=0;j<NLIGNES;j++)
	{
		int codec=couleur();
		for (i=0;i<NBRIQUE;i++)
		{
			level1[j][i].longueur=LBRIQUE;
			level1[j][i].hauteur=HBRIQUE;
			level1[j][i].couleur=codec;
			level1[j][i].vie=NLIGNES-j;
			level1[j][i].pos_x=(SPACE*(i+1))+(LBRIQUE*i);
			level1[j][i].pos_x2=(SPACE*(i+1))+(LBRIQUE*(i+1));
			level1[j][i].pos_y=(HBRIQUE*(j+1))+(SPACE*(j+2));
			level1[j][i].pos_y2=(HBRIQUE*(j+2))+(SPACE*(j+2));
			if (rand()%2==1)
				level1[j][i].bonus=1;
			else
				level1[j][i].bonus=0;
		}
	}
}

void initBonus()//initialise le tableau des bonus
{
	int i;
	for (i=0;i<65;i++)
	{
		bonus_level1[i].pos_x=0;
		bonus_level1[i].pos_y=0;
		bonus_level1[i].type=(rand()%10)-5;
		bonus_level1[i].etat=0;
		if (bonus_level1[i].type==1 || bonus_level1[i].type==2)
			bonus_level1[i].texture="bonus.bmp";
		if (bonus_level1[i].type==3 || bonus_level1[i].type==4)
			bonus_level1[i].texture="malus.bmp";
	}
}

int couleur()//gere et renvoit le code d'une couleur aleatoire
{
	r=(rand()%200)+56;
	v=(rand()%200)+56;
	b=(rand()%200)+56;
	code=r+v*256+b*256*256; //code couleur rgb
	return code;
}
int rouge(int codeCouleur)
{
	return codeCouleur&255;
}
int vert(int codeCouleur)
{
	codeCouleur=codeCouleur>>8;
	return codeCouleur&255;
}
int bleu(int codeCouleur)
{
	codeCouleur=codeCouleur>>16;
	return codeCouleur&255;
}

void drawPaddle()
{
	changeColor(255,255,255);
	drawRect(gdpaddle, HAUTEUR_PADDLE, taille_paddle, 20);
}

void movementBall() //collisions avec les bords, le paddle, les briques (+ gere leurs disparitions) et la perte d'une vie
{
	int i=0;
	int j=0;
	double degre,radian=0;
	int rebond_x=0;
	int rebond_y=0;
	
	rebondBord();
	perteVie();
	collisionPaddle();	
	collisionBriques();

	ball_x=ball_x+speed_ball_x;
	ball_y=ball_y+speed_ball_y;
	changeColor(129,250,250);
	drawCircle(ball_x,ball_y,TAILLE_BALL);
}

void rebondBord() // rebond sur les bords de l'ecran
{
	if (ball_x<=0+TAILLE_BALL)				//rebondi sur le bord gauche de l'ecran
	{
		speed_ball_x=-speed_ball_x;
	}
	if (ball_x>=WINDOW_WIDTH-TAILLE_BALL)	// "" droite ""
	{
		speed_ball_x=-speed_ball_x;
	}
	if (ball_y<=0+TAILLE_BALL)				//"" haut ""
		speed_ball_y=-speed_ball_y;
	if (ball_y>=WINDOW_HEIGHT-TAILLE_BALL)	//rebondi sur le bord bas ""
		speed_ball_y=-speed_ball_y;
}

void perteVie()
{
	if (ball_y>=850)//perte d'une vie
	{
		vie=vie-1;
		speed_ball_x=0;
		speed_ball_y=0;
		speed_ball=SPEED_BALL;
		ball_x=gdpaddle+(taille_paddle/2);
		ball_y=780;
	}
}

void collisionPaddle()
{
	double degre,radian=0;
	if (ball_y>=HAUTEUR_PADDLE-TAILLE_BALL && ball_y<=HAUTEUR_PADDLE+30) //collision avec paddle haut bas
	{
		if (gdpaddle<=ball_x && ball_x<(gdpaddle+taille_paddle))
		{
			degre=(((ball_x-gdpaddle)/taille_paddle)*120)+30;
			radian=(degre*M_PI)/180;
			speed_ball_y=-sin( radian )*speed_ball;
			speed_ball_x=-cos( radian )*speed_ball;
			speed_ball=speed_ball*1.07;
		}
	}
	if (ball_x==gdpaddle-TAILLE_BALL || ball_x==gdpaddle+taille_paddle+TAILLE_BALL)//collision avec paddle gauche droite
	{
		if (ball_y>=HAUTEUR_PADDLE && ball_y<=HAUTEUR_PADDLE+30)
		{
			speed_ball_x=-speed_ball_x;
		}
	}
}

void collisionBriques()
{
	int i=0;
	int j=0;
	int rebond_x=0;
	int rebond_y=0;
	for (j=0;j<NLIGNES;j++)//collision avec brique
	{
		for (i=0;i<NBRIQUE;i++)
		{
			vie_enleve=0;
			if (ball_y-TAILLE_BALL<level1[j][i].pos_y2  && ball_y+TAILLE_BALL> level1[j][i].pos_y)//collision avec brique haut bas
			{
				if (ball_x>level1[j][i].pos_x && ball_x<level1[j][i].pos_x2 && level1[j][i].vie>0)
				{
					if (rebond_y==0)
					{
						speed_ball_y=-speed_ball_y;
						//printf("vie 1%d\n",vie_enleve);
						if (vie_enleve==0)
						{
							level1[j][i].vie=level1[j][i].vie-1;
							vie_enleve=vie_enleve+1;
							rebond_y=rebond_y+1;
						}
					}
					if (level1[j][i].vie==0)
					{
						bonus_level1[nbonus].pos_x=level1[j][i].pos_x;
						bonus_level1[nbonus].pos_y=level1[j][i].pos_y;
						bonus_level1[nbonus].etat=1;
						nbonus=nbonus+1;
					}
				}
			}
			if (ball_x+TAILLE_BALL> level1[j][i].pos_x && ball_x-TAILLE_BALL<level1[j][i].pos_x2) //collision avec brique gauche droite
			{
				if (ball_y<level1[j][i].pos_y2 && ball_y>level1[j][i].pos_y && level1[j][i].vie>0)
				{
					//printf("vie %d, ballx %f, bally %f, j i %d %d, posx %d, posx2 %d, posy %d, posy2 %d, rebond_x %d, rebond_y %d\n",vie_enleve, ball_x, ball_y, j, i, level1[j][i].pos_x, level1[j][i].pos_x2, level1[j][i].pos_y, level1[j][i].pos_y2,rebond_x,rebond_y);
					if (rebond_x==0)
					{
						speed_ball_x=-speed_ball_x;
						if (vie_enleve==0)
						{
							level1[j][i].vie=level1[j][i].vie-1;
							vie_enleve=vie_enleve+1;
							rebond_x=rebond_x+1;
						}
					}
					if (level1[j][i].vie==0)
					{
						bonus_level1[nbonus].pos_x=level1[j][i].pos_x;
						bonus_level1[nbonus].pos_y=level1[j][i].pos_y;
						bonus_level1[nbonus].etat=1;
						nbonus=nbonus+1;
					}
				}
			}
		}
	}
}

int finJeu()//verifie si il reste ou non des briques
{
	int i,j;
	int condition=0;
	for (j=0;j<NLIGNES;j++)
	{
		for (i=0;i<NBRIQUE;i++)
		{
			if (level1[j][i].vie>0)
			{
				condition=1;
			}
		}
	}
	return condition;
}

void overlay()
{
	int i;
	changeColor(255,158,66);
	drawLine(0,880,(VIE*30)+10,880);
	drawLine(VIE*30+10,880,VIE*30+10,920);
	changeColor(255,0,0);
	drawLine(0,860,WINDOW_WIDTH,860);
	changeColor(129,250,250);
	for (i=0;i<vie;i++)
		drawCircle((30*i)+20,900,TAILLE_BALL);
}

void drawBonus() // dessine les bonus et gere leur colision
{
	int i;
	for (i=0;i<NLIGNES*NBRIQUE;i++)
	{
		if (bonus_level1[i].type>0 && bonus_level1[i].etat==1) // quand le bonus est en train de descendre
		{
			sprite(bonus_level1[i].pos_x,bonus_level1[i].pos_y,bonus_level1[i].texture);
			bonus_level1[i].pos_y=bonus_level1[i].pos_y+5;
			if (bonus_level1[i].pos_y+30>=HAUTEUR_PADDLE && bonus_level1[i].pos_y<=HAUTEUR_PADDLE+30) //collision avec paddle haut bas
			{
				if (gdpaddle<=bonus_level1[i].pos_x+50 && bonus_level1[i].pos_x<(gdpaddle+taille_paddle))
					bonus_level1[i].etat=2;
			}
			if(bonus_level1[i].pos_y+30>860)
				bonus_level1[i].etat=3;
		}
		if (bonus_level1[i].etat==2) // pour activer le bonus une fois qu'il a touché le paddle
		{
			if (bonus_level1[i].type==1)//bonus 1, ralentis la balle
			{
				if (speed_ball>5)
				{
					speed_ball_x=speed_ball_x/2;
					speed_ball_y=speed_ball_y/2;
					speed_ball=speed_ball/2;
				}
				bonus_level1[i].etat=4;
			}
			if (bonus_level1[i].type==2)//bonus 2, aggrandi le paddle
			{
				if (taille_paddle<=WINDOW_WIDTH-20)
				{
					taille_paddle=taille_paddle+10;
					gdpaddle=gdpaddle-5;
				}
				if (taille_paddle+gdpaddle>=WINDOW_WIDTH)
					gdpaddle=gdpaddle-10;
				if (gdpaddle<=0)
					gdpaddle=gdpaddle+10;
				bonus_level1[i].etat=4;
			}
			if (bonus_level1[i].type==3)//malus 3, accelere la balle
			{
				speed_ball_x=speed_ball_x*2;
				speed_ball_y=speed_ball_y*2;
				speed_ball=speed_ball*2;
				bonus_level1[i].etat=4;
			}
			if	(bonus_level1[i].type==4)//malus 4, retrecis le paddle
			{
				if (taille_paddle>10)
				{
					taille_paddle=taille_paddle-10;
					gdpaddle=gdpaddle+5;
				}
				bonus_level1[i].etat=4;
			}
			//malus paddle glissant, bonus balle multiplication, bonus ball qui casse tout
		}
	}
}
