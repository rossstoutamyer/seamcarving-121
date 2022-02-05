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
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

int* createSeam(int length) {
	int *seam = new int[length];
	
  for (int i = 0; i < length; ++i) {
  	seam[i] = 0;
  }
  
	return seam;
}

void deleteSeam(int* seam) {
	delete[] seam;
}

bool loadImage(string filename, Pixel** image, int width, int height) {
	ifstream ifs(filename);
	
	if (!ifs.is_open()) {
		cout << "Error: failed to open input file - " << filename << endl;
		return false;
	}
	
	char type[3];
	ifs >> type;
	
	if (toupper(type[0]) != 'P' || type[1] != '3') {
		cout << "Error: type is " << type[0] << type[1] << " instead of P3" << endl;
		return false;
	}
	
	int w = 0;
	int h = 0;
	
	ifs >> w >> h;
	
	if (!ifs.good()) {
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	
	if (width != w) {
		cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;
		return false;
	}
	
	if (height != h) {
		cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
		return false;
	}
	
	int maxColor = 0;
	ifs >> maxColor;
	int neededColors = height * width * 3;
	int numColors = 0;
	
	char colorValue[3];
	while (!ifs.eof()) {
		ifs >> colorValue;
		
		if (colorValue[0] != '\n') {
			++numColors;
		}
		if (ifs.fail()) {
			--numColors;
		}
	}
	
	if (numColors > neededColors) {
		cout << "Error: too many color values" << endl;
		return false;
	} else if (numColors < neededColors) {
		cout << "Error: not enough color values" << endl;
		return false;
	}
	
	ifs.clear();
	ifs.close();
	
	ifstream ifst(filename);
	ifst >> type;
	ifst >> w >> h;
	ifst >> maxColor;
	numColors = 0;
	
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			ifst >> image[j][i].r;
			if (ifst.fail()) {
				cout << "Error: read non-integer value" << endl;
				return false;
			} else {
				numColors++;
			}
			ifst >> image[j][i].g;
			if (ifst.fail()) {
				cout << "Error: read non-integer value" << endl;
				return false;
			} else {
				numColors++;
			}
			ifst >> image[j][i].b;
			if (ifst.fail()) {
				cout << "Error: read non-integer value" << endl;
				return false;
			} else {
				numColors++;
		 	}
			if (image[j][i].r > maxColor || image[j][i].r < 0) {
				cout << "Error: invalid color value " << image[j][i].r << endl;
				return false;
			}
			if (image[j][i].g > maxColor || image[j][i].g < 0) {
				cout << "Error: invalid color value " << image[j][i].g << endl;
				return false;
			}
			if (image[j][i].b > maxColor || image[j][i].b < 0) {
				cout << "Error: invalid color value " << image[j][i].b << endl;
				return false;
			}
			if (numColors == neededColors) {
				break;
			}
		}
	}
	
	return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
	ofstream ofs(filename);
  
  if (!ofs.is_open()) {
  	cout << "Error: failed to open output file - " << filename << endl;
	return false;
  }
  
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << "255" << endl;
  
  for (int i = 0; i < height; ++i) {
  	for (int j = 0; j < width; ++j) {
    	ofs << image[j][i].r << " "
      	    << image[j][i].g << " "
            << image[j][i].b << " ";
	}
  }
	
  return true;
}

int energy(Pixel** image, int x, int y, int width, int height) { 
	
  int energyx = 0;
  int energyy = 0;
  int totalEnergy = 0;
  
  if (x == 0) {
  	energyx += pow((image[1][y].r - image[width - 1][y].r),2);
    energyx += pow((image[1][y].g - image[width - 1][y].g),2);
    energyx += pow((image[1][y].b - image[width - 1][y].b),2);
  } else if (x == width - 1) {
  	energyx += pow((image[0][y].r - image[width - 2][y].r),2);
    energyx += pow((image[0][y].g - image[width - 2][y].g),2);
    energyx += pow((image[0][y].b - image[width - 2][y].b),2);
  } else {
  	energyx += pow((image[x + 1][y].r - image[x - 1][y].r),2);
    energyx += pow((image[x + 1][y].g - image[x - 1][y].g),2);
    energyx += pow((image[x + 1][y].b - image[x - 1][y].b),2);
  }
  
  if (y == 0) {
  	energyy += pow((image[x][1].r - image[x][height - 1].r),2);
    energyy += pow((image[x][1].g - image[x][height - 1].g),2);
    energyy += pow((image[x][1].b - image[x][height - 1].b),2);
  } else if (y == height - 1) {
  	energyy += pow((image[x][0].r - image[x][height - 2].r),2);
    energyy += pow((image[x][0].g - image[x][height - 2].g),2);
    energyy += pow((image[x][0].b - image[x][height - 2].b),2);
  } else {
  	energyy += pow((image[x][y + 1].r - image[x][y - 1].r),2);
    energyy += pow((image[x][y + 1].g - image[x][y - 1].g),2);
    energyy += pow((image[x][y + 1].b - image[x][y - 1].b),2);
  }
  
  totalEnergy = energyx + energyy;
  
  return totalEnergy;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
	seam[0] = start_col;
	int totalEnergy = energy (image, start_col, 0, width, height);
	int leftenergy = 0;
	int rightenergy = 0;
	int midenergy = 0;
	
	for (int i = 1; i < height; ++i) {
		leftenergy = 0;
		rightenergy = 0;
		midenergy = 0;
		
		if (start_col == 0) {
			rightenergy = energy(image, start_col + 1, i, width, height);
			midenergy = energy(image, start_col, i, width, height);
			
			if (midenergy <= rightenergy) {
				totalEnergy += midenergy;
				seam[i] = start_col;
			} else {
				totalEnergy += rightenergy;
				seam[i] = start_col + 1;
				start_col += 1;
			}
		} else if (start_col == width - 1) {
			leftenergy = energy(image, start_col - 1, i, width, height);
			midenergy = energy(image, start_col, i, width, height);
			
			if (midenergy <= leftenergy)  {
				totalEnergy += midenergy;
				seam[i] = start_col;
			} else {
				totalEnergy += leftenergy;
				seam[i] = start_col - 1;
				start_col -= 1;
			}
		} else {
			leftenergy = energy(image, start_col - 1, i, width, height);
			midenergy = energy(image, start_col, i, width, height);
			rightenergy = energy(image, start_col + 1, i, width, height);
			
			if (midenergy <= leftenergy && midenergy <= rightenergy) {
				totalEnergy += midenergy;
				seam[i] = start_col;
			} else if (rightenergy < midenergy && rightenergy <= leftenergy) {
				totalEnergy += rightenergy;
				seam[i] = start_col + 1;
				start_col += 1;
			} else {
				totalEnergy += leftenergy;
				seam[i] = start_col - 1;
				start_col -= 1;
			}
		}
	}
	
  return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
	seam[0] = start_row;
	int totalEnergy = energy (image, 0, start_row, width, height);
	int upenergy = 0;
	int downenergy = 0;
	int midenergy = 0;
	
	for (int i = 1; i < width; ++i) {
		upenergy = 0;
		downenergy = 0;
		midenergy = 0;
		
		if (start_row == 0) {
			downenergy = energy(image, i, start_row + 1, width, height);
			midenergy = energy(image, i, start_row, width, height);
			
			if (midenergy <= downenergy) {
				totalEnergy += midenergy;
				seam[i] = start_row;
			} else {
				totalEnergy += downenergy;
				seam[i] = start_row + 1;
				start_row += 1;
			}
		} else if (start_row == height - 1) {
			upenergy = energy(image, i, start_row - 1, width, height);
			midenergy = energy(image, i, start_row, width, height);
			
			if (midenergy <= upenergy)  {
				totalEnergy += midenergy;
				seam[i] = start_row;
			} else {
				totalEnergy += upenergy;
				seam[i] = start_row - 1;
				start_row -= 1;
			}
		} else {
			upenergy = energy(image, i, start_row - 1, width, height);
			midenergy = energy(image, i, start_row, width, height);
			downenergy = energy(image, i, start_row + 1, width, height);
			
			if (midenergy <= upenergy && midenergy <= downenergy) {
				totalEnergy += midenergy;
				seam[i] = start_row;
			} else if (upenergy < midenergy && upenergy <= downenergy) {
				totalEnergy += upenergy;
				seam[i] = start_row - 1;
				start_row -= 1;
			} else {
				totalEnergy += downenergy;
				seam[i] = start_row + 1;
				start_row += 1;
			}
		}
	}
	
  return totalEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
  int* seam = new int[height];
  int* minSeam = new int[height];
  int minEnergy = loadVerticalSeam(image, 0, width, height, seam);
  int tempEnergy = 0;
  
  for (int i = 0; i < height; ++i) {
	  minSeam[i] = seam[i];
  }
  
  for (int j = 1; j < width; ++j) {
	  tempEnergy = loadVerticalSeam(image, j, width, height, seam);
	  
	  if (tempEnergy < minEnergy) {
		  minEnergy = tempEnergy;
		  for (int k = 0; k < height; ++k) {
			  minSeam[k] = seam[k];
		  }
	  }
  }
  
  delete[] seam;
  
  return minSeam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
  int* seam = new int[width];
  int* minSeam = new int[width];
  int minEnergy = loadHorizontalSeam(image, 0, width, height, seam);
  int tempEnergy = 0;
  
  for (int i = 0; i < width; ++i) {
	  minSeam[i] = seam[i];
  }
  
  for (int j = 1; j < height; ++j) {
	  tempEnergy = loadHorizontalSeam(image, j, width, height, seam);
	  
	  if (tempEnergy < minEnergy) {
		  minEnergy = tempEnergy;
		  for (int k = 0; k < height; ++k) {
			  minSeam[k] = seam[k];
		  }
	  }
  }
  
  delete[] seam;
  
  return minSeam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
	for (int i = 0; i < height; ++i) {
		for (int j = verticalSeam[i]; j < width - 1; ++j) {
			image[j][i] = image[j + 1][i];
		}
	}
	
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
	for (int i = 0; i < width; ++i) {
		for (int j = horizontalSeam[i]; j < height - 1; ++j) {
			image[i][j] = image[i][j + 1];
		}
	}
}
