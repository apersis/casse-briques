// gcc -o main main.c function.c -lSDL2
// ./main

#include <SDL2/SDL.h>
#include <unistd.h>
#include "function.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LBRIQUE 50 // longueur d'une brique
#define HBRIQUE 40 // hauteur d'une brique
#define NBRIQUE 13 // comien de briques par lignes
#define NLIGNES 5 // combien de colonnes de briques
#define SPACE 5 // espace entre chaque briques
#define TAILLE_PADDLE 100
#define TAILLE_BALL 10
#define FPS 60
#define WINDOW_WIDTH 715
#define WINDOW_HEIGHT 920
#define HAUTEUR_PADDLE 800
#define SPEED_BALL 1
#define VIE 3

void drawBricks();
void drawPaddle();
void movementPaddle();
void game();
void initDrawBricks();
int couleur();
int rouge();
int vert();
int bleu();
void movementBall();
int finJeu();
void overlay();
int gdpaddle=300;
int ball_y=780;
int ball_x=350;
double speed_ball_x=0;
double speed_ball_y=0;
int r,v,b,code;
int matrice[NLIGNES-1][NBRIQUE];
int fps=FPS;
int jeu=0;
int vie=VIE;
int tour=0;
int taille_paddle=TAILLE_PADDLE;
int debut=0;

struct paddle
{
	int pos_x;
	int pos_y;
	int longueur;
	int hauteur;
	int couleur;
};
struct brique
{
	int pos_x;
	int pos_y;
	int longueur;
	int hauteur;
	int couleur;
	int vie;
	int bonus;
};
struct ball
{
	int pos_x;
	int pos_y;
	int taille;
	int couleur;
	int vitesse;
};

//position du carre dessiné dans drawGame()
int x = 0;
int y = 0;

void drawGame(){
    /* Ici je dessine mon jeu
     * exemple position x, y modifiés dans KeyPressed() et utilisés pour
     * pouvoir deplacer la figure à chaque boucle de gameLoop()
     */
    clear();
    if(speed_ball_x!=0)
    {
		tour=tour+1;
		if (tour%1000==0)
			fps=fps+10;
	}
	else
		ball_x=gdpaddle+taille_paddle/2;
	if (debut==0)
		sprite(0,330,"Start.bmp");		
	changeColor(255, 255, 0);
	drawPaddle();
	drawBricks();
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
        case SDLK_o: //double les fps (la vitesse de la balle)
			fps=fps*2;
			break;
		case SDLK_l: // diminue de moitier les fps (la vitesse de la balle)
			if (fps>60)
			{
				fps=fps/2;
			}
			break;
		case SDLK_SPACE:
		{
			speed_ball_x=-SPEED_BALL;
			speed_ball_y=-SPEED_BALL;
			debut=1;
			break;
		}
		case SDLK_p: //aggrandis le paddle
			if (taille_paddle<WINDOW_WIDTH-20)
			{
				taille_paddle=taille_paddle+10;
				gdpaddle=gdpaddle-5;
			}
			if (taille_paddle+gdpaddle>WINDOW_WIDTH)
				gdpaddle=gdpaddle-10;
			if (gdpaddle<0)
				gdpaddle=gdpaddle+10;
			break;
		case SDLK_m: //retrecis le paddle
			if (taille_paddle>10)
			{
				taille_paddle=taille_paddle-10;
				gdpaddle=gdpaddle+5;
			}
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

void drawBricks()//dessine les briques avec couleur
{
	int i=0;
	int j=0;
	int hauteur=HBRIQUE+SPACE;
	int longueur=0;
	for (j=0;j<NLIGNES;j++)
	{
		for (i=0;i<NBRIQUE;i++)
		{
			changeColor(rouge(matrice[j][i]),vert(matrice[j][i]),bleu(matrice[j][i]));
			drawRect(longueur,hauteur,LBRIQUE,HBRIQUE);
			if (matrice[j][i]>0)
			{
				if ((matrice[j][i]%5)+1==1)
					sprite(longueur,hauteur,"1.bmp");
				if ((matrice[j][i]%5)+1==2)
					sprite(longueur,hauteur,"2.bmp");
				if ((matrice[j][i]%5)+1==3)
					sprite(longueur,hauteur,"3.bmp");
				if ((matrice[j][i]%5)+1==4)
					sprite(longueur,hauteur,"4.bmp");
				if ((matrice[j][i]%5)+1==5)
					sprite(longueur,hauteur,"5.bmp");
			}
			longueur=longueur+LBRIQUE+SPACE;
		}
		hauteur=hauteur+HBRIQUE+SPACE;
		longueur=0;
	}
}

void initDrawBricks() //dessine les briques et initialise la matrice avec couleur
{
	int i=0;
	int j=0;
	int hauteur=HBRIQUE+SPACE;
	int longueur=0;
	for (j=0;j<NLIGNES;j++)
	{
		int codec=couleur();
		changeColor(rouge(codec),vert(codec),bleu(codec));
		for (i=0;i<NBRIQUE;i++)
		{
			matrice[j][i]=codec;
			drawRect(longueur,hauteur,LBRIQUE,HBRIQUE);
			longueur=longueur+LBRIQUE+SPACE;
		}
		hauteur=hauteur+HBRIQUE+SPACE;
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
	
	if (ball_x<=0+TAILLE_BALL)							//rebondi sur le bord gauche de l'ecran
		speed_ball_x=-speed_ball_x;
	if (ball_x>=WINDOW_WIDTH-TAILLE_BALL)	// "" droite ""
		speed_ball_x=-speed_ball_x;
	if (ball_y<=0+TAILLE_BALL)							//"" haut ""
		speed_ball_y=-speed_ball_y;
	if (ball_y>=WINDOW_HEIGHT-TAILLE_BALL)//rebondi sur le bord bas ""
		speed_ball_y=-speed_ball_y;
		
	if (ball_y>=850)//perte d'une vie
	{
		vie=vie-1;
		speed_ball_x=0;
		speed_ball_y=0;
		ball_x=gdpaddle+(taille_paddle/2);
		ball_y=780;
		fps=60;
	}
		
	if (ball_y>=HAUTEUR_PADDLE-TAILLE_BALL && ball_y<=HAUTEUR_PADDLE+30) //collision avec paddle haut bas
	{
		if (gdpaddle<=ball_x && ball_x<(gdpaddle+taille_paddle))
			speed_ball_y=-speed_ball_y;
	}
	if (ball_x==gdpaddle-TAILLE_BALL || ball_x==gdpaddle+taille_paddle+TAILLE_BALL)//collision avec paddle gauche droite
	{
		if (ball_y>=HAUTEUR_PADDLE && ball_y<=HAUTEUR_PADDLE+30)
		{
			speed_ball_x=-speed_ball_x;
		}
	}
	
	int hauteur=HBRIQUE+SPACE;
	int longueur=0;
	
	for (j=0;j<=NLIGNES;j++)//collision avec brique haut bas
	{
		for (i=0;i<=NBRIQUE;i++)
		{
			if (ball_y-TAILLE_BALL==hauteur+HBRIQUE  || ball_y+TAILLE_BALL== hauteur)
			{
				if (ball_x>=longueur && ball_x<=longueur+LBRIQUE+SPACE && matrice[j][i]>0)
				{
					speed_ball_y=-speed_ball_y;
					if (((matrice[j][i]%5)+1)>1)
						matrice[j][i]=matrice[j][i]-1;
					else
						matrice[j][i]=0;
				}
			}
			longueur=longueur+LBRIQUE+SPACE;
		}
		hauteur=hauteur+HBRIQUE+SPACE;
		longueur=0;
	}
	
	for (i=0;i<=NBRIQUE;i++)//collision avec brique gauche droite
		if (ball_x+TAILLE_BALL>=i*(LBRIQUE+SPACE) && ball_x-TAILLE_BALL<=(i+1)*(LBRIQUE+SPACE))
			for (j=0;j<=NLIGNES;j++)
				if (ball_y<=((HBRIQUE+SPACE)*(j+2)) && ball_y>=((HBRIQUE+SPACE)*(j+1)) && matrice[j][i]>0)
				{
					speed_ball_x=-speed_ball_x;
					if (((matrice[j][i]%5)+1)>1)
						matrice[j][i]=matrice[j][i]-1;
					else
						matrice[j][i]=0;
				}
				
	ball_x=ball_x+speed_ball_x;
	ball_y=ball_y+speed_ball_y;
	changeColor(255,255,255);
	drawCircle(ball_x,ball_y,TAILLE_BALL);
}

int finJeu()//verifie si il reste ou non des briques
{
	int i,j;
	int condition=0;
	for (j=0;j<=NLIGNES;j++)
	{
		for (i=0;i<=NBRIQUE;i++)
		{
			if (matrice[j][i]>0)
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
	drawLine(0,880,100,880);
	drawLine(100,880,100,920);
	changeColor(255,0,0);
	drawLine(0,860,WINDOW_WIDTH,860);
	changeColor(255,255,255);
	for (i=0;i<vie;i++)
		drawCircle((30*i)+20,900,TAILLE_BALL);
}
