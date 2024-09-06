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
#define WINDOW_WIDTH 715
#define WINDOW_HEIGHT 920
int fps=60;
#define HAUTEUR_PADDLE 800
#define SPEED_BALL 1

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
//void supprBricks();
int ball_y=750;
int ball_x=357;
int speed_ball_x=-SPEED_BALL;
int speed_ball_y=-SPEED_BALL;
int gdpaddle=300;
int r,v,b,code;
int matrice[NLIGNES-1][NBRIQUE];

//position du carre dessiné dans drawGame()
int x = 0;
int y = 0;

void drawGame(){
    /* Ici je dessine mon jeu
     * exemple position x, y modifiés dans KeyPressed() et utilisés pour
     * pouvoir deplacer la figure à chaque boucle de gameLoop()
     */
    clear();
	changeColor(255, 255, 0);
	drawPaddle();
	drawBricks();
	movementBall();
    actualize();
    //supprBricks();
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
            if (gdpaddle<=WINDOW_WIDTH-TAILLE_PADDLE)
				gdpaddle=gdpaddle+20;
			if(gdpaddle>=WINDOW_WIDTH-TAILLE_PADDLE)
				gdpaddle=WINDOW_WIDTH-TAILLE_PADDLE;
            break;
        }
        case SDLK_ESCAPE:
            freeAndTerminate();
            break;
        case SDLK_o:
			fps=fps*2;
			break;
		case SDLK_l:
			if (fps>60)
			{
				fps=fps/2;
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
                    KeyPressed(event.key.keysym.sym);
                    break;
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
    printf("Fin du programme\n");
    freeAndTerminate();
}

void drawBricks()
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
			longueur=longueur+LBRIQUE+SPACE;
		}
		hauteur=hauteur+HBRIQUE+SPACE;
		longueur=0;
	}
}

void initDrawBricks()
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

int couleur()
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
	drawRect(gdpaddle, HAUTEUR_PADDLE, TAILLE_PADDLE, 20);
}

void movementBall() //colisions avec les bords, le paddle et les briques (+ gere leurs disparitions)
{
	int i=0;
	int j=0;
	
	if (ball_x<=0+TAILLE_BALL)							//rebondi sur les bords
		speed_ball_x=-speed_ball_x;
	if (ball_x>=WINDOW_WIDTH-TAILLE_BALL)	// ""
		speed_ball_x=-speed_ball_x;
	if (ball_y<=0+TAILLE_BALL)							//""
		speed_ball_y=-speed_ball_y;
	if (ball_y>=WINDOW_HEIGHT-TAILLE_BALL)//rebondi sur les bords
		speed_ball_y=-speed_ball_y;
		
	if (ball_y==HAUTEUR_PADDLE-TAILLE_BALL || ball_y==HAUTEUR_PADDLE+30) //colision avec paddle
	{
		if (gdpaddle<=ball_x && ball_x<(gdpaddle+TAILLE_PADDLE))
			speed_ball_y=-speed_ball_y;
	}
	if (ball_x==gdpaddle || ball_x==gdpaddle+TAILLE_PADDLE)
	{
		if (ball_y>=HAUTEUR_PADDLE && ball_y<=HAUTEUR_PADDLE+30)
		{
			speed_ball_x=-speed_ball_x;
		}
	}
	
	int hauteur=HBRIQUE+SPACE;
	int longueur=0;
	
	for (j=0;j<=NLIGNES;j++)
	{
		for (i=0;i<=NBRIQUE;i++)
		{
			if (ball_y-TAILLE_BALL==hauteur+HBRIQUE  || ball_y+TAILLE_BALL== hauteur)
			{
				if (ball_x>=longueur && ball_x<=longueur+LBRIQUE+SPACE && matrice[j][i]>0)
				{
					speed_ball_y=-speed_ball_y;
					matrice[j][i]=0;
				}
			}
			longueur=longueur+LBRIQUE+SPACE;
		}
		hauteur=hauteur+HBRIQUE+SPACE;
		longueur=0;
	}
	
	/*int hauteur2=HBRIQUE+SPACE;
	int longueur2=0;
	i=0;
	j=0;
	
	for (i=0;i<=NBRIQUE;i++)
	{
		for (j=1;j<=NLIGNES;j++)
		{
			if (ball_x+TAILLE_BALL == longueur2 || ball_x-TAILLE_BALL == longueur2 + LBRIQUE+SPACE)
			{
				//printf("%d %d\n",hauteur2,longueur2);
				if (ball_y+TAILLE_BALL >= hauteur2 && ball_y-TAILLE_BALL <= hauteur2+HBRIQUE && matrice[j][i]>0) 
				{
					printf("%d %d %d %d %d\n",j,i,matrice[j][i],ball_x-TAILLE_BALL,ball_y);
					speed_ball_x=-speed_ball_x;
					matrice[j][i]=0;
				}
			}
			hauteur2=hauteur2+HBRIQUE+SPACE;
		}
		longueur2=longueur2+LBRIQUE+SPACE;
		hauteur2=0;
	}*/
	/*for (j=0;j<=NLIGNES;j++)
		if (ball_y<=((HBRIQUE+SPACE)*(j+2)) && ball_y>=((HBRIQUE+SPACE)*(j+1)))
			for (i=0;i<=NBRIQUE;i++)
				if (ball_x>=i*(LBRIQUE+SPACE) && ball_x<=(i+1)*(LBRIQUE+SPACE) && matrice[j][i]>0)
				{
					speed_ball_y=-speed_ball_y;
					matrice[j][i]=0;
				}*/
	for (i=0;i<=NBRIQUE;i++)
		if (ball_x+TAILLE_BALL>=i*(LBRIQUE+SPACE) && ball_x-TAILLE_BALL<=(i+1)*(LBRIQUE+SPACE))
			for (j=0;j<=NLIGNES;j++)
				if (ball_y<=((HBRIQUE+SPACE)*(j+2)) && ball_y>=((HBRIQUE+SPACE)*(j+1)) && matrice[j][i]>0)
				{
					printf("ok %d %d\n",i,j);
					speed_ball_x=-speed_ball_x;
					matrice[j][i]=0;
				}
	ball_x=ball_x+speed_ball_x;
	ball_y=ball_y+speed_ball_y;
	changeColor(255,255,255);
	drawCircle(ball_x,ball_y,TAILLE_BALL);
}
