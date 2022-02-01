#include <iostream>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include "bmplib.h"

using namespace std;

//============================Add function prototypes here======================
void dummy(unsigned char[][SIZE][RGB], unsigned char[][SIZE][RGB]);
void convolve(unsigned char[][SIZE][RGB], unsigned char[][SIZE][RGB], 
	      int, double[][11]);
void sobel(unsigned char[][SIZE][RGB], unsigned char[][SIZE][RGB]);
void gaussian(double[][11], int, double);
void gaussian_filter(unsigned char[][SIZE][RGB], unsigned char[][SIZE][RGB],
                    int, double);
void unsharp(unsigned char[][SIZE][RGB], unsigned char[][SIZE][RGB],
                    int, double, double);
//============================Do not change code in main()======================

#ifndef AUTOTEST

int main(int argc, char* argv[])
{
   //First check argc
  if(argc < 3)
    {
      //we need at least ./filter <input file> <filter name> to continue
      cout << "usage: ./filter <input file> <filter name> <filter parameters>";
      cout << " <output file name>" << endl;
      return -1;
    }
   //then check to see if we can open the input file
   unsigned char input[SIZE][SIZE][RGB];
   unsigned char output[SIZE][SIZE][RGB];
   char* outfile;
   int N;
   double sigma, alpha;
   //double kernel[11][11];

   // read file contents into input array
   int status = readRGBBMP(argv[1], input); 
   if(status != 0)
   {
      cout << "unable to open " << argv[1] << " for input." << endl;
      return -1;
   }
   //Input file is good, now look at next argument
   if( strcmp("sobel", argv[2]) == 0)
   {
     sobel(output, input);
     outfile = argv[3];
   }
   else if( strcmp("blur", argv[2]) == 0)
   {
     if(argc < 6)
       {
	 cout << "not enough arguments for blur." << endl;
	 return -1;
       }
     N = atoi(argv[3]);
     sigma = atof(argv[4]);
     outfile = argv[5];
     gaussian_filter(output, input, N, sigma);
   }
   else if( strcmp("unsharp", argv[2]) == 0)
   {
     if(argc < 7)
       {
	 cout << "not enough arguments for unsharp." << endl;
	 return -1;
       }
     N = atoi(argv[3]);
     sigma = atof(argv[4]);
     alpha = atof(argv[5]);
     outfile = argv[6];
     unsharp(output, input, N, sigma, alpha);

   }
   else if( strcmp("dummy", argv[2]) == 0)
   {
     //do dummy
     dummy(output, input);
     outfile = argv[3];
   }
   else
   {
      cout << "unknown filter type." << endl;
      return -1;
   }

   if(writeRGBBMP(outfile, output) != 0)
   {
      cout << "error writing file " << argv[3] << endl;
   }

}   

#endif 

//=========================End Do not change code in main()=====================


// Creates an identity kernel (dummy kernel) that will simply
// copy input to output image and applies it via convolve()
//
// ** This function is complete and need not be changed.
// Use this as an example of how to create a kernel array, fill it in
// appropriately and then use it in a call to convolve.
void dummy(unsigned char out[][SIZE][RGB], unsigned char in[][SIZE][RGB])
{
   double k[11][11];
   for (int i = 0; i < 3; i++)
   {
      for(int j = 0; j < 3; j++)
      {
         k[i][j] = 0;
      }
   }
   k[1][1] = 1;
   convolve(out, in, 3, k);
}


// Convolves an input image with an NxN kernel to produce the output kernel
// You will need to complete this function by following the 
//  instructions in the comments
void convolve(unsigned char out[][SIZE][RGB], unsigned char in[][SIZE][RGB], 
	      int N, double kernel[][11])
{
 
   int padded[SIZE+10][SIZE+10][RGB];  // Use for input image with appropriate 
                                       // padding
   int temp[SIZE][SIZE][RGB];          // Use for the unclamped output pixel 
                                       // values then copy from temp to out, 
                                       // applying clamping 

   //first set all of padded to 0 (black)
  int halfPad = N/2; 
  for (int r = 0; r < RGB; r++)
   {
      for (int i =0; i<(SIZE+10);i++)
      {
        for(int j = 0; j<(SIZE+10);j++)
        {
          padded[i][j][r] = 0;
        }
      }
   }


   //now copy input into padding to appropriate locations

  for (int r = 0; r< RGB; r++)
  {
    for (int i = 0; i < SIZE; i++)
    {
      for (int j = 0; j < SIZE; j++)
      {
        padded[i+halfPad][j+halfPad][r] = (int)in[i][j][r];
      }
    }
  }

   //initialize temp pixels to 0 (black)

  for (int r = 0; r < RGB; r++)
   {
      for (int i =0; i<SIZE;i++)
      {
        for(int j = 0; j<SIZE;j++)
        {
          temp[i][j][r] = 0;
        }
      }
   }


  //now perform convolve (using convolution equation on each pixel of the 
  // actual image) placing the results in temp (i.e. unclamped result)
  //Here we give you the structure of the convolve for-loops, you need
  //to figure out the loop limits
  for(int y= halfPad;y< (SIZE+halfPad);y++)
  { 
    for(int x= halfPad;x< (SIZE+halfPad);x++)
    { 
      for(int k=0;k<RGB;k++)
      { 
        for(int i= -halfPad; i<= halfPad; i++)
        { 
          for(int j= -halfPad; j<= halfPad; j++)
          { 
            temp[y-halfPad][x-halfPad][k] += padded[y+i][x+j][k]*kernel[i+halfPad][j+halfPad];
          }
        }
      }
    }
  }

   //now clamp and copy to output
   // You may need to cast to avoid warnings from the compiler:
   // (i.e. out[i][j][k] = (unsigned char) temp[i][j][k];)

  for (int r = 0; r < RGB; r++)
  {
    for (int y=0; y < SIZE; y++)
    {
       for (int x = 0; x < SIZE; x++)
       {
         if (temp[y][x][r] > 255)
         {
           temp[y][x][r] = 255;
         }
         if (temp[y][x][r] < 0)
         {
           temp[y][x][r] = 0;
         }
         out[y][x][r] = (unsigned char)temp[y][x][r];
       }
    }
  }



}

// You will need to complete this function by following the 
//  instructions in the comments
void sobel(unsigned char out[][SIZE][RGB], unsigned char in[][SIZE][RGB])
{
   double k[11][11];
   double s_h1[3][3] = { {-1, 0, 1}, 
                         {-2, 0, 2}, 
                         {-1, 0, 1} };
   double s_h2[3][3] = { {1, 0, -1}, 
                         {2, 0, -2}, 
                         {1, 0, -1} };
   
   unsigned char h1_sobel[SIZE][SIZE][RGB]; //hold intemediate images
   unsigned char h2_sobel[SIZE][SIZE][RGB]; 

   for (int i = 0; i < 11; i++)
   {
      for(int j=0; j < 11; j++)
      {
         k[i][j] = 0;
      }
   }


   // Copy in 1st 3x3 horizontal sobel kernel (i.e. copy s_h1 into k)

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      k[i][j] = s_h1[i][j];
    }
  }


   // Call convolve to apply horizontal sobel kernel with result in h1_sobel

  convolve(h1_sobel, in, 3, k);


   // Copy in 2nd 3x3 horizontal sobel kernel (i.e. copy s_h2 into k)

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      k[i][j] = s_h2[i][j];
    }
  }


   // Call convolve to apply horizontal sobel kernel with result in h2_sobel

  convolve(h2_sobel, in, 3, k);


   // Add the two results (applying clamping) to produce the final output 
  for (int r = 0; r < RGB; r++)
  {
    for (int y=0; y < SIZE; y++)
    {
       for (int x = 0; x < SIZE; x++)
       {
         int temp = 0;
         temp = h1_sobel[y][x][r] + h2_sobel[y][x][r];
         if (temp > 255)
         {
           temp = 255;
         }
         if (temp < 0)
         {
           temp = 0;
         }
         out[y][x][r] = (unsigned char)(temp);
       }
    }
  }
  

}


// Add the rest of your functions here (i.e. gaussian, gaussian_filter, unsharp)
void gaussian(double k[][11], int N, double sigma)
{
  int pad = N/2;
  double sum = 0;
  for (int y = -(pad); y <= pad; y++)
  {
    for (int x = -pad; x <= pad; x++)
    {
      k[y+pad][x+pad] = exp(-((pow(x,2) + pow(y,2))/(2*pow(sigma,2))));
      sum+= k[y+pad][x+pad];
    }
  }
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      double frac = k[i][j]/sum;
      k[i][j] = frac;
    }
  }
  
}


void gaussian_filter(unsigned char out[][SIZE][RGB], unsigned char in[][SIZE][RGB],
                    int N, double sigma)
{
  double k[11][11];
  gaussian(k, N, sigma);
  convolve(out, in, N, k);
}

void unsharp(unsigned char out[][SIZE][RGB], unsigned char in[][SIZE][RGB],
                    int N, double sigma, double alpha)
{
  unsigned char blur[SIZE][SIZE][RGB];
  double k[11][11];
  gaussian(k, N, sigma);
  convolve(blur, in, N, k);
  for (int r = 0; r < RGB; r++)
  {
    for (int i = 0; i < SIZE; i++)
    {
      for (int j= 0; j < SIZE; j++)
      {
        int dif = (int)(in[i][j][r] - blur[i][j][r]);
        int fin = (int)(in[i][j][r] + (alpha*dif));
        if (fin > 255)
        {
          fin = 255;
        }
        if (fin < 0)
        {
          fin = 0;
        }
        out[i][j][r] = (unsigned char)fin;
      }
    }
  }
  
}

