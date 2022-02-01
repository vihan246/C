/* 
maze.cpp
*/

#include <iostream>
#include "mazeio.h"
#include "queue.h"

using namespace std;

// Prototype for maze_search, which you will fill in below.
int maze_search(char**, int, int);

// main function to read, solve maze, and print result
int main(int argc, char* argv[]) {
   int rows, cols, result;
   char** mymaze=NULL;
   
   if(argc < 2)
   {
       cout << "Please provide a maze input file" << endl;
       return 1;
   }
  
   mymaze = read_maze(argv[1], &rows, &cols); // <---TASK: COMPLETE THIS
     
     
   if (mymaze == NULL) {
      cout << "Error, input format incorrect." << endl;
      return 1;
   }
	 //================================
   // When working on Checkpoint 3, you will call maze_search here.
   // But for Checkpoint 1, just assume we found the path.
  
   result = maze_search(mymaze, rows, cols); // <--- TASK: CHANGE THIS
	
	
   // examine value returned by maze_search and print appropriate output
   if (result == 1) { // path found!
      print_maze(mymaze, rows, cols);
   }
   else if (result == 0) { // no path :(
      cout << "No path could be found!" << endl;
   }
   else { // result == -1
      cout << "Invalid maze." << endl;
   }

	 //================================
   // ADD CODE BELOW
   // to delete all memory that read_maze allocated

  for (int y =0; y < rows; y++)
  {
    delete[] mymaze[y];
  }
  delete[] mymaze;

  
  
  
   return 0;
}

/**************************************************
 * Attempt to find shortest path and return:
 *  1 if successful
 *  0 if no path exists
 * -1 if invalid maze (not exactly one S and one F)
 *
 * If path is found fill it in with '*' characters
 *  but don't overwrite the 'S' and 'F' cells
 * NOTE: don't forget to deallocate memory in here too!
 *************************************************/
int maze_search(char** maze, int rows, int cols) 
{
  // *** You complete ****
  bool** visited = new bool*[rows];
  Location** predecessor = new Location*[rows];
  for (int i = 0 ;i < rows; i++)
  {
    visited[i] = new bool[cols];
    predecessor[i] = new Location[cols];
  }
  
  int sNum = 0;
  int fNum = 0;
  int total = rows*cols;
  Queue q(total);
  Location finish;
  Location start;
  for (int y = 0; y < rows; y++)
  {
    for (int x = 0; x < cols; x++)
    {
      if (maze[y][x] == 'S')
      {
        start.row = y;
        start.col = x;
        sNum++;
      }
      else if (maze[y][x] =='F')
      {
        fNum++;
        finish.row = y;
        finish.col = x; 
      }
    }
  }
  
  q.add_to_back(start);
  if ((sNum != 1)||(fNum != 1))
  {
    return -1;
  }
  
  for (int y = 0; y < rows; y++)
  {
    for (int x = 0; x < cols; x++)
    {
      visited[y][x] = false;
    }
  }
  
  
  while (!(q.is_empty()))
  {
    Location loc = q.remove_from_front();
    if (loc.row > 0)
    {
      if((maze[loc.row-1][loc.col]=='.' || maze[loc.row-1][loc.col]=='F') && !visited[loc.row-1][loc.col])
      {
         Location neighbor;
        neighbor.row = loc.row-1;
        neighbor.col = loc.col;
        visited[neighbor.row][neighbor.col] = true;
        q.add_to_back(neighbor);
        predecessor[neighbor.row][neighbor.col].row = loc.row;
        predecessor[neighbor.row][neighbor.col].col = loc.col;
      }
    }
    if (loc.col > 0)
    {
      if((maze[loc.row][loc.col-1]=='.' || maze[loc.row][loc.col-1]=='F') && !visited[loc.row][loc.col-1])
      {
         Location neighbor;
        neighbor.row = loc.row;
        neighbor.col = loc.col-1;
        q.add_to_back(neighbor);
        visited[neighbor.row][neighbor.col]= true;
        predecessor[neighbor.row][neighbor.col].row = loc.row;
        predecessor[neighbor.row][neighbor.col].col = loc.col;
      }
    }
    
    if (loc.row < (rows-1))
    {
      if((maze[loc.row+1][loc.col]=='.' || maze[loc.row+1][loc.col]=='F') && !visited[loc.row+1][loc.col])
      {
         Location neighbor;
        neighbor.row = loc.row+1;
        neighbor.col = loc.col;
        visited[neighbor.row][neighbor.col] =true;
        q.add_to_back(neighbor);
        predecessor[neighbor.row][neighbor.col].row = loc.row;
        predecessor[neighbor.row][neighbor.col].col = loc.col;
      }
    }   
  
    if (loc.col < (cols-1))
    {
      if((maze[loc.row][loc.col+1]=='.'||maze[loc.row][loc.col+1]=='F') && !visited[loc.row][loc.col+1])
        {
         Location neighbor;
          neighbor.row=loc.row;
          neighbor.col=loc.col+1;
          q.add_to_back(neighbor);
          visited[neighbor.row][neighbor.col]=true;
          predecessor[neighbor.row][neighbor.col].row = loc.row;
          predecessor[neighbor.row][neighbor.col].col = loc.col;
      }
    }
  }

  if (!(visited[finish.row][finish.col]))
  {
    return 0;
  }
    Location current; 
    current.row = finish.row;
    current.col = finish.col;
    //cout << "finish "<<current.row<< " " << current.col << endl; 
    while ((current.row != start.row) || (current.col != start.col))
    {
      if (visited[current.row][current.col] == true)
      {
        Location previous;
        previous.row = predecessor[current.row][current.col].row;
        previous.col = predecessor[current.row][current.col].col;
        current.row = previous.row;
        current.col = previous.col;
        if (visited[current.row][current.col])
        {
          maze[current.row][current.col] = '*';
        }
      }
    }
  
  for (int i = 0; i < rows; i++)
    {
      delete[] visited[i];
      delete[] predecessor[i];
    }
    delete[] visited;
    delete[] predecessor;
  return 1;
// DELETE this stub, it's just for Checkpoint 1 to compile.
}
