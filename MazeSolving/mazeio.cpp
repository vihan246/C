/* 
mazeio.cpp
*/

#include <iostream>
#include <fstream>
#include "mazeio.h"

using namespace std;

/*************************************************
 * read_maze:
 * Read the maze from the given filename into a 
 *  2D dynamically  allocated array.
 * 
 * Return the pointer to that array.
 * Return NULL (a special address) if there is a problem, 
 * opening the file or reading in the integer sizes.
 *
 * The first argument is the filename of the maze input.
 *  You should use an ifstream to open and read from this
 *  file.
 *
 * We also pass in two pointers to integers. These are
 * output parameters (i.e. declared by the caller and 
 * passed-by-reference for this function to fill in).
 * Fill the integers pointed to by these arguments
 * with the number of rows and columns 
 * read (the first two input values).
 *
 *************************************************/
char** read_maze(char* filename, int* rows, int* cols) 
{
	
   // *** You complete ****
	ifstream in;
	in.open(filename);
  int r =0;
  int c = 0;
  in >> r;
  in >> c;
  *rows = r;
  *cols = c;
  char** tempMaze = new char*[r];
  for (int i =0; i < r; i++)
  {
    tempMaze[i] = new char[c];
  }
  
  for (int y = 0; y < r; y++)
  {
    for (int x = 0; x < c; x++)
    {
      in >> tempMaze[y][x];
      if ((tempMaze[y][x] != '.')&&(tempMaze[y][x] != 'S')&&
          (tempMaze[y][x] != 'F')&&(tempMaze[y][x] != '#'))
      {
        return NULL;
      }
    }
  }
  
  return tempMaze;
	
}

/*************************************************
 * Print the maze contents to the screen in the
 * same format as the input (rows and columns, then
 * the maze character grid).
 *************************************************/
void print_maze(char** maze, int rows, int cols) 
{
	
   // *** You complete ****

  cout << rows << " " << cols << endl;
  for (int y = 0; y < rows; y++)
  {
    for (int x = 0; x < cols; x++)
    {
      cout << maze[y][x];
    }
    cout << endl;
  }
}

