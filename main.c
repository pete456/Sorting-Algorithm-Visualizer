#include <stdio.h>
#include <SDL2/SDL.h>
#include <inttypes.h>

#define LISTSIZE 10
#define SCRNWIDTH 640
#define SCRNHEIGHT 480
#define DELAY 100 //Delay in milliseconds

struct color {
  int r, g, b;
};

struct item {
  int value;
  SDL_Rect rect;
  struct color color;
  SDL_Texture *tex;
};

void setColors(int value, struct color *color, int max)
{
  color->r=value<=255?value:255;
  color->g=value<=255?value:255;
  color->b=255*value/max;
}

int max(int size, int valuelist[])
{
  int max=0;
  for(int i=0; i<size; ++i) {
    if(valuelist[i] > max)
      max=valuelist[i];
  }
  return max;
}

int calcBarHeight(int frameheight, int max, int value)
{
  return frameheight*value/max;
}

//Sets up a list of struct items
int createItems(int size, int valuelist[], struct item itemlist[], SDL_Rect *frame, SDL_Renderer *ren)
{
  //Returns if there are to many items 
  if(frame->w < size)
    return -1;

  int width=frame->w/size;
  int largestvalue=max(size,valuelist);
  
  for(int i=0; i < size; ++i) {
    //Sets x,y,w,h, and colors
    itemlist[i].value = valuelist[i];
    itemlist[i].rect.h=calcBarHeight(frame->h, largestvalue, valuelist[i]);
    itemlist[i].rect.w=width;
    itemlist[i].rect.x=i*width;
    itemlist[i].rect.y=frame->y-itemlist[i].rect.h;
    setColors(itemlist[i].value, &(itemlist[i].color), largestvalue);

    //Sets texure
    SDL_Surface *surf = SDL_CreateRGBSurface(0,width,itemlist[i].value*10,32,0,0,0,0);
    if(SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format,itemlist[i].color.r,itemlist[i].color.g,itemlist[i].color.b)) < 0) {
      printf(SDL_GetError());
      return -1;
    }
    itemlist[i].tex=SDL_CreateTextureFromSurface(ren,surf);
    SDL_FreeSurface(surf);
  }
  return 0;
}

int main(int argc, char *argv[]){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win = SDL_CreateWindow("Insertion Sort Visualization", 100, 100, SCRNWIDTH, SCRNHEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *ren = SDL_CreateRenderer(win,-1,0);

  SDL_SetRenderDrawColor(ren,255,0,0,255);
  
  int running = 1;
  SDL_Event event;

  //Sets up values
  int unorderedlist[]={8,2,7,1,10,13,4,10,15,20};
  struct item itemlist[LISTSIZE];
  SDL_Rect graphframe={.h=300,.w=SCRNWIDTH,.x=0,.y=SCRNHEIGHT};
  
  createItems(LISTSIZE, unorderedlist, itemlist, &graphframe,ren);

  //Index of comparing items
  int currentitem_i=LISTSIZE-1;
  int compareitem_i=currentitem_i-1;

  //Clock data
  Uint64 start, end;
  double elapsed=0;
  int delay=DELAY;

  while(running) {
    start=SDL_GetPerformanceCounter();
    
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
	running = 0;
      }
    }
    
    SDL_RenderClear(ren);
    
    if(elapsed >= delay) {
      elapsed=0;
      if(compareitem_i < 0) {
	currentitem_i--;
	compareitem_i=currentitem_i-1;	
      }
      if(currentitem_i >= 0) {
	if(compareitem_i >= 0) {
	  if(itemlist[currentitem_i].value < itemlist[compareitem_i].value) {
	  //Swap x values
	    int tempx=itemlist[compareitem_i].rect.x;
	    itemlist[compareitem_i].rect.x=itemlist[currentitem_i].rect.x;
	    itemlist[currentitem_i].rect.x=tempx;
	    
	    //Swap position in array
	    struct item temp=itemlist[compareitem_i];
	    itemlist[compareitem_i]=itemlist[currentitem_i];
	    itemlist[currentitem_i]=temp;
	  }
	}
      }
      compareitem_i--;
    }
    for(struct item *p = &itemlist[0]; p < &itemlist[LISTSIZE]; ++p) {
      SDL_RenderCopy(ren,p->tex,NULL,&(p->rect));
    }
    SDL_RenderPresent(ren);

    end=SDL_GetPerformanceCounter();
    //printf("Start: %"PRIu64", End: %"PRIu64", elapsed: %lf\n",start,end,elapsed);
    elapsed+=(double)(end-start)*1000/SDL_GetPerformanceFrequency();
  }

  //Clean up
  for(struct item *p=&itemlist[0]; p < &itemlist[LISTSIZE]; ++p) {
    SDL_DestroyTexture(p->tex);
  }
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
