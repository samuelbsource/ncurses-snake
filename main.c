/*
 * Copyright (c) 2017 Samuel Boczek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *********************************************************************************/

#define _GNU_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

//|//
//|// <= Space station
//|//

Snake snake;
Apple apple;
Board board;

void (*mode)(void) = mode_welcome;

int last_arrow_key = -1;
int current_arrow_key = -1;

#define BOTTOM_PADDING 1
#define APPLE_SYMBOL ACS_STERLING
#define SNAKE_SYMBOL ACS_DIAMOND
#define BORDER_SYMBOL ACS_BLOCK
#define GAME_SPEED 100000 // In microseconds
#define GAMEOVER_TEXT1 "Game Over!"
#define GAMEOVER_TEXT2 "Press <ENTER> to start over"
#define GAMESTART_TEXT "Press any arrow key to begin"

// Helper functions
int is_arrow_key(int key) {
  switch (key) {
    case KEY_UP:
    case KEY_DOWN:
    case KEY_LEFT:
    case KEY_RIGHT:
      return 1;
    default:
      return 0;
  }
}
void draw_game() {
  
  clear(); // Clear whatever is on the board now
  
  // Draw custom border around the board
  for (size_t x = 0; x < board.width; ++x) {
    for (size_t y = 0; y < board.height; ++y) {
      if (x == 0 || y == 0 || x == board.width-1 || y == board.height-1) {
        move(y, x);
        addch(BORDER_SYMBOL);
      }
    }
  }
  
  // Draw apple
  move(apple.y, apple.x);
  addch(APPLE_SYMBOL);
  
  // Draw snake
  for (size_t i = 0; i < snake.size; ++i) {
    move(snake.tail[i].y, snake.tail[i].x);
    addch(SNAKE_SYMBOL);
  }
  
  // Draw the snake size
  move(board.height + (BOTTOM_PADDING / 2), 1);
  printw("Snake's size: %d", snake.size);
  
  refresh(); // This will apply all the changes to the screen
  
}
void spawn_apple() {
  apple.y = (random() % board.height-2) + 2; // Spawn the apple somewhere inside the board
  apple.x = (random() % board.width-2) + 2; // Spawn the apple somewhere inside the board
}
void init_snake() {
  snake.tail[0].x = board.width / 2; // Initialize snake's head to the center of the screen
  snake.tail[0].y = board.height / 2; // Initialize snake's head to the center of the screen
  snake.size = 1; // Set the size of the snake
}
void grow_snake() {
  snake.tail[snake.size].x = snake.tail[snake.size-1].x; // Copy the position of older part of snake
  snake.tail[snake.size].y = snake.tail[snake.size-1].y; // Copy the position of older part of snake
  snake.size += 1; // Set the new size of the snake
}
void update_snake() {
  for (int i = snake.size-1; i >= 0; --i) {
    snake.tail[i].x += snake.tail[i].v.x; // Add the velocity to the location
    snake.tail[i].y += snake.tail[i].v.y;
    if (i != 0) {
      snake.tail[i].v.x = snake.tail[i-1].v.x; // Copy the older snake's part velocity
      snake.tail[i].v.y = snake.tail[i-1].v.y;
    }
  }
  last_arrow_key = current_arrow_key; // Update the last_arrow_key
}
void check_collisions() {
  
  // Check if snake hit the wall
  if (
    snake.tail[0].x == 0 || 
    snake.tail[0].y == 0 || 
    snake.tail[0].x == board.width-1 || 
    snake.tail[0].y == board.height-1
  ) {
    mode = mode_gameover; // Game Over
  }
  
  // Check if snake ate itself
  for (size_t i = 1; i < snake.size; i++) {
    if (snake.tail[0].x == snake.tail[i].x && snake.tail[0].y == snake.tail[i].y) {
      mode = mode_gameover; // Game Over
    }
  }
  
  // Check if snake ate apple
  if (snake.tail[0].x == apple.x && snake.tail[0].y == apple.y) {
    grow_snake(); // Self explanatory I think
    spawn_apple(); // Respawn the apple at new location
  }
}
void handle_keys(int key) {
  
  if (is_arrow_key(key)) {
    if (
      (last_arrow_key == KEY_UP && key == KEY_DOWN) ||
      (last_arrow_key == KEY_DOWN && key == KEY_UP) ||
      (last_arrow_key == KEY_RIGHT && key == KEY_LEFT) ||
      (last_arrow_key == KEY_LEFT && key == KEY_RIGHT)
    ) {
      return; // Don't let the user invert snake's direction
    }
    
    current_arrow_key = key; // Remember the arrow key
  }
  
  // React to arrow keys
  switch (key) {
    case KEY_UP: {
      snake.tail[0].v.x = 0; // Change snake's velocity in x direction
      snake.tail[0].v.y = -1; // Change snake's velocity in y direction
      break;
    }
    case KEY_DOWN: {
      snake.tail[0].v.x = 0;
      snake.tail[0].v.y = 1;
      break;
    }
    case KEY_RIGHT: {
      snake.tail[0].v.x = 1;
      snake.tail[0].v.y = 0;
      break;
    }
    case KEY_LEFT: {
      snake.tail[0].v.x = -1;
      snake.tail[0].v.y = 0;
      break;
    }
  }
  
}

// Game modes
void mode_playing() {
  update_snake(); // Update the snake
  check_collisions(); // Check for collisions in game
  draw_game(); // Draw the result
}
void mode_gameover() {
  draw_game(); // Draw the game screen
  move((board.height / 2) - 2, (board.width / 2) - (strlen(GAMEOVER_TEXT1) / 2));
  printw("%s", GAMEOVER_TEXT1); // Print the text at the center of the screen - 2
  move((board.height / 2) - 1, (board.width / 2) - (strlen(GAMEOVER_TEXT2) / 2));
  printw("%s", GAMEOVER_TEXT2); // Print the text at the center of the screen - 1
  refresh(); // Update changes
  while (getch() != '\n') {} // Read the keys
  init_snake(); // Reset snake to it's initial state
  mode = mode_welcome;
}
void mode_welcome() {
  draw_game(); // Draw the game screen
  move((board.height / 2) - 3, (board.width / 2) - (strlen(GAMESTART_TEXT) / 2));
  printw("%s", GAMESTART_TEXT); // Print the text at the center of the screen - 3 height
  refresh(); // Update changes
  while (1) { // Read the keys
    int key = -1;
    handle_keys(key = getch());
    if (is_arrow_key(key)) {
      break;
    }
  }
  mode = mode_playing; // Change the mode
}

// This function fires every GAME_SPEED microseconds
void update() {
  mode();
}

int main() {
  
  // Engine parts
  initscr(); // Initialize ncurses library
  cbreak(); // When reading keyboard input don't wait for <enter>
  noecho(); // Don't print keyboard input to the screen
  keypad(stdscr, 1); // Translate escaped sequences into single characters
  curs_set(0); // Hide the cursor
  getmaxyx(stdscr, board.height, board.width); // Read maximum board height and width
  board.height -= BOTTOM_PADDING; // Substract the padding from board height
  
  // Game parts
  init_snake(); // Initialize snake
  spawn_apple(); // Spawn the apple
  draw_game(); // Draw the game for the first time
  
  signal(SIGALRM, update); // Make update function handler for ALARM signal
  ualarm(GAME_SPEED, GAME_SPEED); // Call ALARM signal every GAME_SPEED microseconds
  
  while (1) {
    handle_keys(getch()); // Read keys indefinatelly
  }
  
  endwin(); // Disable ncurses library and restore terminal to it's normal mode
  
  return 0;
}






