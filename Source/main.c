#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>

int randomNumber(int nr_min, int nr_max)
{
    static int temp = 0;
    static bool initialized = false;
    if(!initialized)
    {
        initialized = true;
        srand(time(NULL));
    }
    temp = rand() % nr_max + nr_min;
    temp -= (temp%10);
    return temp;
}

struct node {
    SDL_Rect konum;
    bool isNew;
    struct node *next;
};

struct node* head =NULL;
struct node* iterator = NULL;
SDL_Renderer* renderer;
char rotation;
bool collision = false;
bool eat = false;
int random1,random2,count;

void ekle(){
    struct node* eklenecek = (struct node *)malloc(sizeof(struct node));
    iterator = head;
    while(iterator->next!=NULL){
        iterator = iterator->next;
    }
    iterator->next = eklenecek;
    eklenecek->isNew = true;
    eklenecek->konum.x = iterator->konum.x;
    eklenecek->konum.y = iterator->konum.y;
    eklenecek->konum.w = 8;
    eklenecek->konum.h = 8;
    eklenecek->next = NULL;
}

/*
void yazdir(){
    iterator = head;
    while(iterator != NULL){
        printf("posx:%d  posy:%d =>",iterator->konum.x,iterator->konum.y);
        iterator = iterator->next;
    }
    printf("\n");
}
*/

void guncelle(struct node* head, struct node* prev){
    struct node* local_iterator;
    local_iterator= head;
    if(local_iterator->next!=NULL){
        guncelle(local_iterator->next,local_iterator);
    }
    if(local_iterator->isNew==true){
       local_iterator->konum.x = prev->konum.x;
       local_iterator->konum.y = prev->konum.y;
    }
    else{
        local_iterator->isNew = false;
    }

}

void checkEat()
{
    if(head->konum.x == random1 && head->konum.y == random2){
        eat = true;
    }
}

void checkCollision()
{
        iterator=head->next;
        while(iterator!=NULL)
        {
            if(head->konum.x ==iterator->konum.x && head->konum.y ==iterator->konum.y )
            {
                collision = true;
                printf("\nGAME OVER\nSCORE: %d",count);
                break;
            }
            else
            {
                iterator = iterator->next;
            }
        }
}

void move(){

    guncelle(head,head);

    switch(rotation)
    {
        case 'u':
            if(head->konum.y != 0){
                head ->konum.y -= 10;
            }
            else{
                head->konum.y = 400;
            }
            break;
        case 'd':
            if(head->konum.y != 400){
                head ->konum.y += 10;
            }
            else{
                head->konum.y = 0;
            }
            break;
        case 'l':
            if(head->konum.x != 0){
                head ->konum.x -= 10;
            }
            else{
                head->konum.x = 400;
            }
            break;
        case 'r':
            if(head->konum.x != 400){
                head ->konum.x += 10;
            }
            else{
                head->konum.x = 0;
            }
            break;
        default:
            // do something
            break;
    }
    checkCollision();
}

int main(int argc, char** argv){
    int delayms = 100;
    int mindelay = 40;
    count = 0;
    random1 = randomNumber(0,400);
    random2 = randomNumber(0,400);

    SDL_Rect startposition = {200,200,8,8};
    SDL_Rect eatposition = {random1,random2,8,8};
    head = (struct node *)malloc(sizeof(struct node));
    head->isNew = false;
    head->konum = startposition;
    head->next= NULL;

    rotation = 'p';

    //sdl init
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);
    Mix_Chunk *apple = Mix_LoadWAV("beep.wav");

    //pencere olustur

    SDL_Window* window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,400,400,SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    //renderer olustur

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //game loop

    bool running = true;
    SDL_Event event;
    while(running){

        SDL_Delay(delayms);

        //event listener

         while (SDL_PollEvent(&event)){

            //quit event

            if(event.type == SDL_QUIT){
                running = false;
            }

            //keyboard event

            if (event.type == SDL_KEYDOWN){  //buffer ile keydown okuma çözülecek.

                switch(event.key.keysym.sym){

                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_UP:
                    if(rotation!='d'){
                        rotation = 'u';
                    }
                    break;
                case SDLK_DOWN:
                    if(rotation!='u'){
                        rotation = 'd';
                    }
                    break;
                case SDLK_LEFT:
                    if(rotation!='r'){
                        rotation = 'l';
                    }
                    break;
                case SDLK_RIGHT:
                    if(rotation!='l'){
                        rotation = 'r';
                    }
                    break;
                case SDLK_x:
                    ekle();
                    break;
                case SDLK_p:
                    //pause eklenecek
                    break;
                default:
                    //do something
                    break;
                }
            }
         }
         checkEat();
         move();

         if(collision==true){
            running = false;
         }

         if(eat==true){
            Mix_PlayChannel(-1,apple,0);
            ekle();
            count++;
            eat=false;
            random1 = randomNumber(0,400);
            random2 = randomNumber(0,400);
            eatposition.x = random1;
            eatposition.y = random2;
            if(delayms>mindelay){
                delayms -= 2;
            }
         }

        // Clear buffer
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
	    SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255,0,0,255);
        SDL_RenderFillRect(renderer,&eatposition);
        //  Draw islemleri
        SDL_SetRenderDrawColor(renderer, 240,240,240,255);
        SDL_RenderFillRect(renderer,&head->konum);
        iterator = head->next;
        while(iterator != NULL){
            SDL_RenderFillRect(renderer, &iterator->konum);
            iterator = iterator->next;
        }

	    // Göster
	    SDL_RenderPresent(renderer);

    }

    //temizle

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_FreeChunk(apple);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;

}
