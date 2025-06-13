#include <raylib.h>
#include <iostream>
#define sw 800
#define sh 500
#define st "Ping Pong"
int playerscore = 0;
int cpuscore = 0;
int matchpoint = 5;
enum state
{
  start,
  gameplay,
  end
};
enum movement
{
  left,
  right
} ballmovement;
class Ball
{
public:
  int ballsize = 15;
  float ballx = sw / 2, bally = sh / 2;
  float speedx = 5, speedy = 5;
  void DrawBall()
  {
    DrawCircle(ballx, bally, ballsize, WHITE);
    DrawCircle(sw/2,sh/2,50,Fade(WHITE,0.3));
    DrawCircleLines(sw/2,sh/2,50,WHITE);
  }
  void BallMotion()
  {
    ballx += speedx;
    bally += speedy;
    if (ballx > (sw - (ballsize / 2)))
      speedx *= -1;
    if (ballx < (ballsize / 2))
      speedx *= -1;
    if (bally > (sh - (ballsize / 2)))
      speedy *= -1;
    if (bally < (ballsize / 2))
      speedy *= -1;
  }
} ball;
class Player : public Ball
{
public:
  int width = 15, height = 80;
  float playerx = 0, playery = sh / 2 - (height / 2);
  float playerspeed = 5;
  void DrawPlayer()
  {
    DrawRectangle(playerx, playery, width, height, WHITE);
  }
  void PlayerMovement()
  {
    if (IsKeyDown(KEY_I))
    {
      if (playery > 0)
        playery -= playerspeed;
    }
    if (IsKeyDown(KEY_K))
    {
      if (playery < (sh - height))
        playery += playerspeed;
    }
  }

} player;
class Cpu : public Ball
{
public:
  int width = 15, height = 80;
  float cpux = sw - width, cpuy = sh / 2 - (height / 2);
  float cpuspeed = 3.5;
  int cpuscore = 0;
  void DrawCpu()
  {
    DrawRectangle(cpux, cpuy, width, height, WHITE);
  }
  void CpuMovement(int x, int y)
  {
    if (y > cpuy && x > sw / 2)
    {
      if (cpuy < (sh - height))
        cpuy += cpuspeed;
    }
    if (y < cpuy && x > sw / 2)
    {
      if (cpuy > 0)
        cpuy -= cpuspeed;
    }
  }
} cpu;
bool refree(Player p, Cpu c, Ball b)
{
  if (CheckCollisionCircleRec(Vector2{b.ballx, b.bally}, b.ballsize, Rectangle{player.playerx, player.playery, (float)player.width, (float)player.height}) != true)
  {
    if (b.ballx < p.width)
    {
      cpuscore = cpuscore + 1;
      return true;
    }
    if (b.ballx > (sw - c.width))
    {
      playerscore++;
      return true;
    }
  }
  return false;
}
void reposition(Player &p, Cpu &c, Ball &b)
{
  p.playerx = 0;
  p.playery = sh / 2 - (p.height / 2);
  c.cpux = sw - c.width;
  c.cpuy = sh / 2 - (c.height / 2);
  b.ballx = sw / 2;
  b.bally = sh / 2;
}
int main()
{
  InitWindow(sw, sh, st);
  SetTargetFPS(60);
  Vector2 mouse = {0, 0};
  state currentstate = start;
  std::string winner;
  int n = 1,c,p;
  int framecount = 0;
  bool exitreq = false;
  bool exit = false;
  bool pause = false;
  bool serve = false;
  bool timer = true;
  while (!exit)
  {
    framecount++;
    mouse = GetMousePosition();
    // updation
    if (currentstate == start)
    {
      if (IsKeyPressed(KEY_P) && !exitreq)
      {
        currentstate = gameplay;
        framecount = 0;
      }
      if (IsKeyPressed(KEY_Q))
        exitreq = !exitreq;
      if (exitreq)
      {
        if (IsKeyPressed(KEY_Y))
          exit = !exit;
        if (IsKeyPressed(KEY_N))
          exitreq = !exitreq;
      }
    }
    if (currentstate == gameplay)
    {
      if (IsKeyPressed(KEY_ESCAPE))
        pause = !pause;
      if (pause)
      {
        if (IsKeyPressed(KEY_R))
          pause = !pause;
        if (IsKeyPressed(KEY_E))
        {
          currentstate = start;
          pause = !pause;
          reposition(player, cpu, ball);
          playerscore = 0;
          cpuscore = 0;
          n = 1;
          timer = true;
          serve = false;
          framecount = 0;
        }
      }
      if (!pause)
      {
        if (timer)
        {
          if (framecount > 100 && n < 4)
          {
            framecount = 0;
            n++;
          }
          if (n == 4)
          {
            serve = true;
            timer = false;
          }
        }
        // object movements
        if (serve)
          ball.BallMotion();
        player.PlayerMovement();
        cpu.CpuMovement(ball.ballx, ball.bally);
        // collision
        if (CheckCollisionCircleRec(Vector2{ball.ballx, ball.bally}, ball.ballsize, Rectangle{player.playerx, player.playery, (float)player.width, (float)player.height}))
        {
          p = GetRandomValue(0,1);
          switch(p)
          {
            case 0:
            {
              ball.speedx = 5;
              ball.speedy = 5;
              break;
            }
            case 1:
            {
              ball.speedx = 5;
              ball.speedy = -5;
              break;
            }
          }
          
        }
        if (CheckCollisionCircleRec(Vector2{ball.ballx, ball.bally}, ball.ballsize, Rectangle{cpu.cpux, cpu.cpuy, (float)cpu.width, (float)cpu.height}))
        {
          c = GetRandomValue(0,1);
          switch(c)
          {
            case 0:
            {
              ball.speedx = -5;
              ball.speedy = 5;
              break;
            }
            case 1:
            {
              ball.speedx = -5;
              ball.speedy = -5;
              break;
            }
          }
        }
        if (refree(player, cpu, ball))
        {
          reposition(player, cpu, ball);
          serve = !serve;
          timer = !timer;
          n = 1;
          framecount = 0;
        }
        if (playerscore == matchpoint)
        {
          serve = false;
          winner = "Player";
          currentstate = end;
        }
        if (cpuscore == matchpoint)
        {
          serve = false;
          winner = "Cpu";
          currentstate = end;
        }
      }
    }
    if (currentstate == end)
    {
      n = 1;
      timer = true;
      framecount = 0;
      reposition(player, cpu, ball);
      if (IsKeyPressed(KEY_R))
      {
        currentstate = gameplay;
        playerscore = 0;
        cpuscore = 0;
      }
      if (IsKeyPressed(KEY_E))
      {
        currentstate = start;
        playerscore = 0;
        cpuscore = 0;
      }
    }
    BeginDrawing();
    ClearBackground(Fade(BLUE, 0.3));
    if (currentstate == start)
    { // main menu
      DrawText("Ping Pong", 50, 20, 50, WHITE);
      DrawLineEx(Vector2{0, 80}, Vector2{600, 80}, 5, WHITE);
      DrawText("[P]Play", 600, 250, 30, WHITE);
      DrawLineEx(Vector2{600, 300}, Vector2{sw, 300}, 5, WHITE);
      DrawText("[Q]Quit", 600, 350, 30, WHITE);
      DrawLineEx(Vector2{600, 400}, Vector2{sw, 400}, 5, RED);
      DrawText("CONTROLS :",10,350,30,WHITE);
      DrawLineEx(Vector2{0,380},Vector2{200,380},5,RED);
      DrawText("[I] Key to Move UP",10,400,25,WHITE);
      DrawText("[K] Key to Move DOWN",10,450,25,WHITE);
      // exit request
      if (exitreq)
      {
        DrawRectangle(0, 0, sw, sh, Fade(WHITE, 0.5));
        DrawRectangle(0, sh / 2 - 100, sw, 200, WHITE);
        DrawText("Do You Want to Exit..?", 130, 220, 30, BLACK);
        DrawText("[Y]Yes", 500, 270, 25, RED);
        DrawText("[N]No", 600, 270, 25, BLACK);
      }
    }
    if (currentstate == gameplay)
    { // player and cpu drawing
      DrawLine(sw / 2, 0, sw / 2, sh, WHITE);
      ball.DrawBall();
      player.DrawPlayer();
      cpu.DrawCpu();
      DrawText("Player",160,10,30,WHITE);
      DrawText("Cpu",570,10,30,WHITE);
      DrawText("[ESC] Key to PAUSE",10,480,20,WHITE);
      DrawText(TextFormat("%i", cpuscore), 600, 50, 30, WHITE);
      DrawText(TextFormat("%i", playerscore), 200, 50, 30, WHITE);
      if (timer)
        DrawText(TextFormat("%i", n), 390, 300, 70, RED);
      if (pause)
      {
        DrawRectangle(0, 0, sw, sh, Fade(WHITE, 0.6));
        DrawRectangle(0, sh / 2 - 100, sw, 200, Fade(BLUE, 0.9));
        DrawText("[R]Resume", 300, 190, 30, WHITE);
        DrawText("[E]Exit", 300, 250, 30, WHITE);
      }
    }
    if (currentstate == end)
    {
      DrawRectangle(0, 0, sw / 2, sh, BLUE);
      DrawText("PLAYER", 90, 50, 40, WHITE);
      DrawLineEx(Vector2{0, 100}, Vector2{270, 100}, 5, WHITE);
      DrawRectangle(sw / 2, 0, sw / 2, sh, RED);
      DrawText("CPU", 580, 50, 40, WHITE);
      DrawLineEx(Vector2{560, 100}, Vector2{sw, 100}, 5, WHITE);
      DrawLineEx(Vector2{sw / 2,0}, Vector2{sw / 2,sh}, 5, WHITE);
      if (winner == "Player")
      {
        DrawText("WINNER", 130, 150, 30, WHITE);
        DrawText("NEXT TIME WINNER", 450, 150, 30, WHITE);
      }
      if (winner == "Cpu")
      {
        DrawText("WINNER", 540, 150, 30, WHITE);
        DrawText("NEXT TIME WINNER", 50, 150, 30, WHITE);
      }
      DrawTriangle(Vector2{390,200},Vector2{390,250},Vector2{440,200},BLUE);
      DrawTriangle(Vector2{360,280},Vector2{410,280},Vector2{410,230},RED);
      DrawText(TextFormat("Score : %i", playerscore), 115, 200, 30, WHITE);
      DrawText(TextFormat("Score : %i", cpuscore), 530, 200, 30, WHITE);
      DrawText("[R]Retry", 130, 400, 30, WHITE);
      DrawText("[E]Exit", 560, 400, 30, WHITE);
    }
    EndDrawing();
  }
  return 0;
}