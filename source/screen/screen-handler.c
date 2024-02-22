#include "../screen.h"

extern SDL_Rect board_rect(int screenWidth, int screenHeight);

extern void screen_board_base_textures_destroy();

extern void screen_board_base_textures_load(SDL_Renderer* renderer);

extern void screen_board_textures_destroy(ScreenBoardTextures* boardTextures);

extern bool squares_texture_create(SDL_Texture** texture, SDL_Renderer* renderer, int width, int height);


bool window_create(SDL_Window** window, int width, int height, const char title[])
{
  if((*window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)) == NULL)
  {
    error_print("SDL_CreateWindow: %s", SDL_GetError());

    return false;
  }
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

  // This forces the window to be at least this dimension
  // SDL_SetWindowMinimumSize(*window, 400, 400);

  return true;
}

void window_destroy(SDL_Window** window)
{
  SDL_DestroyWindow(*window);
  *window = NULL;
}

bool renderer_create(SDL_Renderer** renderer, SDL_Window* window)
{
  if((*renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
  {
    error_print("SDL_CreateRenderer: %s", SDL_GetError());

    return false;
  }
  return true;
}

void renderer_destroy(SDL_Renderer** renderer)
{
  SDL_DestroyRenderer(*renderer);
  *renderer = NULL;
}

void screen_board_meta_init(ScreenBoardMeta* screenBoardMeta)
{
  screenBoardMeta->grabbedSquare = SQUARE_NONE;
  screenBoardMeta->markedSquare = SQUARE_NONE;
  screenBoardMeta->holdingPiece = PIECE_NONE;
  screenBoardMeta->rightHoldingSquare = SQUARE_NONE;
  screenBoardMeta->markedSquaresBoard = 0ULL;

  memset(screenBoardMeta->arrows, 0, sizeof(screenBoardMeta->arrows));

  info_print("Initialized screen board meta");
}

// This function should create a complete screen, so it can be used directly
bool screen_create(Screen* screen, int width, int height, const char title[])
{
  if(!sdl_drivers_init())
  {
    sdl_drivers_quit();

    error_print("Failed to initialize SDL drivers");
    
    return 1;
  }

  if(!window_create(&screen->window, width, height, title))
  {
    return false;
  }
  if(!renderer_create(&screen->renderer, screen->window))
  {
    return false;
  }

  screen->width = width;
  screen->height = height;

  // Loading base textures for the board
  screen_board_base_textures_load(screen->renderer);

  background_base_textures_load(screen->renderer);


  info_print("Created Screen");

  return true;
}

bool screen_board_create(ScreenBoard* board, Screen screen)
{
  board->rect = board_rect(screen.width, screen.height);

  board->textures = (ScreenBoardTextures) {0};

  screen_board_meta_init(&board->meta);

  squares_texture_create(&board->textures.squares, screen.renderer, board->rect.w, board->rect.h);

  return true;
}

void screen_board_destroy(ScreenBoard* board)
{
  screen_board_textures_destroy(&board->textures);
}

bool screen_menu_board_create(ScreenMenuBoard* menu, Screen screen)
{
  background_texture_create(&menu->menu.background, screen.renderer, screen.width, screen.height);

  screen_board_create(&menu->board, screen);

  return true;
}

void screen_menu_board_destroy(ScreenMenuBoard* menu)
{
  texture_destroy(&menu->menu.background);

  screen_board_destroy(&menu->board);
}

void screen_destroy(Screen* screen)
{
  screen_board_base_textures_destroy();

  background_base_textures_destroy();


  renderer_destroy(&screen->renderer);

  window_destroy(&screen->window);

  sdl_drivers_quit();

  info_print("Destroyed Screen");
}

void sdl_drivers_quit()
{
  Mix_CloseAudio();

  // SDL_WasInit
  SDL_Quit();

  IMG_Quit();

  TTF_Quit();

  Mix_Quit();

  info_print("Quitted SDL Drivers");
}

bool sdl_drivers_init()
{
  if(SDL_Init(SDL_INIT_VIDEO) != 0) // | SDL_INIT_AUDIO
  {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
  
    return false;
  }
  if(IMG_Init(IMG_INIT_PNG) == 0)
  {
    fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
  
    return false;
  }
  if(TTF_Init() == -1)
  {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
  
    return false;
  }
  if(Mix_Init(0) != 0)
  {
    fprintf(stderr, "TTF_Init: %s\n", Mix_GetError());
  
    return false;
  }
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
  {
    fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
    
    return false;
  }

  info_print("Initiated SDL Drivers");

  return true;
}
