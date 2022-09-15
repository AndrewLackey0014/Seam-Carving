#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

 using namespace std;
 
Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;
  
  // Create a one dimensional array on the heap of pointers to Pixels 
  //    that has width elements (i.e. the number of columns)
  Pixel** image1 = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image1[i] = new Pixel[height];
    
    if (image1[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image1[i]; // deleting nullptr is not a problem
    }
    delete [] image1; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image1[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image1;
}

void deleteImage(Pixel** image1, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image1[i]; // delete each individual array placed on the heap
  }
  delete [] image1;
  image1 = nullptr;
}

int* createSeam(int length) {
    int* pointer = new int[length];
    for (int i = 0; i < length; ++i){
      pointer[i] = 0;
    }
    return pointer;
}
void deleteSeam(int* seam) {
    delete[] seam;
}

bool loadImage(string filename, Pixel** image1, int width, int height) {
  cout << "Loading image..." << endl;
  // declare/define and open input file stream
  ifstream ifs (filename);
  
  // check if input stream opened successfully
  if (!ifs.is_open()) {
    cout << "Error: failed to open input file - " << filename << endl;
    return false;
  }
  
  // get type from preamble
  char type[3];
  ifs >> type; // should be P3
  if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
    cout << "Error: type is " << type << " instead of P3" << endl;
    return false;
  }
  
  // get width (w) and height (h) from preamble;
  int w = 0, h = 0;
  ifs >> w >> h;
  if (ifs.fail()){
    cout << "Error: read non-integer value" << endl;
    return false;
  }
  if (w != width || width < 0) { // check that width matches what was passed into the function
    cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;

    return false;
  }
  if (h != height || height <= 0) { // check that height matches what was passed into the function
    cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
    return false;
  }
  
  // get maximum value from preamble
  int colorMax = 0;
  ifs >> colorMax;
  if (ifs.fail()){
    cout << "Error: read non-integer value" << endl;
    return false;
  }
  if (colorMax != 255) {
    cout << "Error: file is not using RGB color values." << endl;
    return false;
  }
  
 
    // get RGB pixel values
    
  while (!ifs.eof()) {
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        if (image1[col][row].r < 0 || image1[col][row].r > 255){
            cout << "Error: invalid color value " << image1[col][row].r << endl;
            return false;
        }
        if (image1[col][row].g < 0 || image1[col][row].g > 255){
            cout << "Error: invalid color value " << image1[col][row].g << endl;
            return false;
        }
        if (image1[col][row].b < 0 || image1[col][row].b > 255){
            cout << "Error: invalid color value " << image1[col][row].b << endl;
            return false;
        }
          
        ifs >> image1[col][row].r;

        ifs >> image1[col][row].g;

        ifs >> image1[col][row].b;

      }
    }
  }
    
  return true;
}



bool outputImage(string filename, Pixel** image1, int width, int height) {
  cout << "Outputting image..." << endl;
  // declare/define and open output file stream
  ofstream ofs (filename);
  
  // check if output stream opened successfully
  if (!ofs.is_open()) {
    cout << "Error: failed to open output file - " << filename << endl;
    return false;
  }
  
  // output preamble
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;
  
  // output pixels
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      ofs << image1[col][row].r << " ";
      ofs << image1[col][row].g << " ";
      ofs << image1[col][row].b << " ";
    }
    ofs << endl;
  }
  return true;
}


 
int energy(Pixel** image1, int x, int y, int width, int height) {
    int energy;
    int x_r;
    int x_g;
    int x_b;
    int y_r;
    int y_g;
    int y_b;
    if (width < 3){
        x_r = 0;
        x_g = 0;
        x_b = 0;
    }
    else if (x == (width-1)){
        x_r = abs(image1[0][y].r - image1[x-1][y].r);
        x_g = abs(image1[0][y].g - image1[x-1][y].g);
        x_b = abs(image1[0][y].b - image1[x-1][y].b);
    }
    else if (x == 0){
        x_r = abs(image1[x+1][y].r - image1[width-1][y].r);
        x_g = abs(image1[x+1][y].g - image1[width-1][y].g);
        x_b = abs(image1[x+1][y].b - image1[width-1][y].b);
    }else{
        x_r = abs(image1[x+1][y].r - image1[x-1][y].r);
        x_g = abs(image1[x+1][y].g - image1[x-1][y].g);
        x_b = abs(image1[x+1][y].b - image1[x-1][y].b);
    }
    if (height < 3){
        y_r = 0;
        y_g = 0;
        y_b = 0;
    }
    else if (y == (height-1)){
        y_r = abs(image1[x][0].r - image1[x][y-1].r);
        y_g = abs(image1[x][0].g - image1[x][y-1].g);
        y_b = abs(image1[x][0].b - image1[x][y-1].b);
    }
    else if (y == 0){
        y_r = abs(image1[x][y+1].r - image1[x][height-1].r);
        y_g = abs(image1[x][y+1].g - image1[x][height-1].g);
        y_b = abs(image1[x][y+1].b - image1[x][height-1].b);
    }else{
        y_r = abs(image1[x][y+1].r - image1[x][y-1].r);
        y_g = abs(image1[x][y+1].g - image1[x][y-1].g);
        y_b = abs(image1[x][y+1].b - image1[x][y-1].b);
    }
    energy = pow(x_r, 2) + pow(x_b, 2) + pow(x_g, 2) + pow(y_r, 2) + pow(y_b, 2) + pow(y_g, 2);
    return energy;
}


 
int loadVerticalSeam (Pixel ** image1, int start_col, int width, int height, int *seam){

int energy_total = 0;
int left_energy;
int down_energy;
int right_energy;
 
for (int row = 0; row <= height-1; ++row){
    if (row == 0){ // Initializing the seam
        seam[row] = start_col;
        energy_total += energy(image1, start_col, row, width, height);
        continue;
    }
    
    left_energy = 100000;
    down_energy = energy(image1, start_col, row, width, height);
    right_energy = 200000;
    
    if (width == 1){ // down is the only option
        energy_total += down_energy;
        seam[row] = start_col;
        continue;
    }
    if (width < 3 && start_col != 0){
        right_energy = energy(image1, (start_col - 1), row, width, height);
    }
    else if (width < 3 && start_col == 0){
        left_energy = energy(image1, (start_col + 1), row, width, height);
    }
    else if (start_col == 0 && width >= 3){
        left_energy = energy(image1, (start_col + 1), row, width, height);
    }
    else if (start_col == (width-1) && width >= 3){
        right_energy = energy(image1, (start_col - 1), row, width, height);
    }
    else if ((start_col != 0 && start_col != (width-1)) || width >= 3){
        left_energy = energy(image1, (start_col + 1), row, width, height);
        right_energy = energy(image1, (start_col - 1), row, width, height);
    }
    // down_energy is the lowest
    if (down_energy <= left_energy && down_energy <= right_energy ){
        energy_total += down_energy;
        seam[row] = start_col;
        continue;
    }
    // left_energy is the lowest
    else if (left_energy <= down_energy && left_energy <= right_energy){
        energy_total += left_energy;
        seam[row] = (start_col + 1);
        start_col += 1;
        continue;
    // right_energy is the lowest
    }else if (right_energy <= down_energy && right_energy <= left_energy){
        energy_total += right_energy;
        seam[row] = (start_col - 1);
        start_col -= 1;
        continue;
    }
    
 
}
  
return energy_total;

}



int loadHorizontalSeam (Pixel ** image1, int start_row, int width, int height, int *seam){
  
    return 0;

}


 
int *findMinVerticalSeam (Pixel ** image1, int width, int height){
    int Seam[height];
    int * pointer = createSeam(height);
    int candidate_seam[height];
    int min_energy;
    int candidate_energy;
    for (int col = 0; col < width; ++col){
        if (col == 0){
          min_energy = loadVerticalSeam(image1, col, width, height, Seam);
          for (int i = 0; i < height; ++i){
              pointer[i] = Seam[i];
          }
          continue;
        }
        candidate_energy = loadVerticalSeam(image1, col, width, height, candidate_seam);
        if (candidate_energy < min_energy){
            min_energy = candidate_energy;
            deleteSeam(pointer);
            pointer = createSeam(height);
            for (int i = 0; i < height; ++i){
                Seam[i] = candidate_seam[i];
                pointer[i] = Seam[i];

            }
        }
    }
    return pointer;
}



int *findMinHorizontalSeam (Pixel ** image1, int width, int height){
  
    return nullptr;

}


 
void removeVerticalSeam(Pixel** image1, int width, int height, int* verticalSeam) {
  for (int row = 0; row < height; ++row){
        if (verticalSeam[row] != width - 1){
            for (int col = verticalSeam[row]; col < width - 1; ++col){
                image1[col][row] = image1[col + 1][row];
            }
        }
  }
}


void removeHorizontalSeam(Pixel** image1, int width, int height, int* horizontalSeam) {
}
