#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "Bitmap_Helper.h"

/*
Author  - Aadil Chasmawala ,netID -aac10066
CPE Assignment_2 
*/

using namespace std;
//Function prototypes : 
double** create2DGrid(int n_rows,int n_columns);
void display_grid(double** arr, int rows, int columns);
void setBoundaryConditions(double** arr,int n_rows,int n_columns);
void stimulateGrid(double** arr, int n_rows, int n_columns ,int& stim_row, int& stim_col);
void simulateThermalDissipation(double** arr , double threshold, int n_rows,int n_columns,int stim_row,int stim_col);
uint8_t** convert_to_Uint(double** arr, int n_rows, int n_columns);
void deallocate(double** arr,int n_rows);
void deallocate(uint8_t** arr,int n_rows);
int main() {
	int n_rows(0), n_columns(0);
	int stim_row(1), stim_col(1);
	int selection;
	bool option1_selected(false);
	bool option2_selected = false;
	double** arr = nullptr;
	while (true) {
		//Display menu
		cout << "---------------------------------------------" << endl;
		cout << "THERMAL DISTRIBUTION SIMULATION" << endl;
		cout << "---------------------------------------------" << endl;
		cout << "1) Create the dynamic 2D grid . " << endl;
		cout << "2) Set initial side temperature values for the grid. " << endl;
		cout << "3) Stimulate the grid at a specific point." << endl;
		cout << "4) Calculate the thermal distribution based on the specific stimulation and initial conditions(and save the results in an image file)." << endl;
		cout << "5) Print the thermal grid on the screen " << endl;
		cout << "6) Exit the program. " << endl;
		cout << "Enter your selection: " << endl;
		cin >> selection;
		switch (selection) {
		case 1: {
			option1_selected = true;
			//Deallocate the previous grid whenever a new one is created
			deallocate(arr, n_rows);// this was deallocate(arr,0) which doesnt work.
			bool value_valid = true;
			//Ask and validate for user input
	       //Should be atleast 4X4 Grid
			do{
			    value_valid =true;
				cout << "Enter the number of rows(maximum value is 500 , minimum is 4):" << endl;
				cin >> n_rows;
				if (n_rows < 4 || n_rows > 500) {
					cout << "Value of rows is out of bounds /negative , please enter a valid value:" << endl;
					value_valid = false;
				}


				cout << "Enter the number of columns(maximum value is 500 , minimum is 4):" << endl;
				cin >> n_columns;
				if (n_columns < 4 || n_columns > 500) {
					cout << "Value of columns is out of bounds /negative , please enter a valid value:" << endl;
					value_valid = false;
				}
				
			} while (value_valid == false);
			arr = create2DGrid(n_rows, n_columns);
			cout << "Grid has been created" << endl;
			break;
		}
		case 2: {
			//check if option_1 has been selected
			if (option1_selected) {
				option2_selected = true;
				setBoundaryConditions(arr, n_rows, n_columns);
				break;
			}
			else {
				cout << "Selection option 1 before option 2" << endl;
				break;
			}}
		case 3: {
			//Check if option_2 has been selected
			if (option2_selected) {
				stimulateGrid(arr, n_rows, n_columns, stim_row, stim_col);
				break;
			}
			else {
				cout << "Select option 1 and 2 before stimulating the plate" << endl;
				break;
			}
		}
		case 4: {
			//Ask user for threshold value and run simulation
			bool threshold_valid;
            double threshold;
			//Validate threshold value
			do {
				threshold_valid = true;
				cout << "Enter the threshold value:" << endl;
				cin >> threshold;
				if (threshold < 0) {
					cout << "threshold value is not valid, should be positive , try again :" << endl;
					threshold_valid = false;
				}
			} while (threshold_valid == false);

			//run simulation
			simulateThermalDissipation(arr, threshold, n_rows, n_columns, stim_row, stim_col);
			break;
		}
		case 5: {
			display_grid(arr, n_rows, n_columns);
			break;
		}
		case 6: {
			cout << "You have chosen to exit the program" << endl;
			//deallocation
			deallocate(arr, n_rows);
			return 0;
		}
		default: {
			cout << "Enter a valid input, please retry." << endl; }
		}
	}
}

	

void display_grid(double** arr, int rows, int columns){
	//Iterate through the grid
	for (int i = 0; i < rows; i++) {
		cout << '|';
		for (int j = 0; j < columns; j++) {
			cout << setw(5)<< arr[i][j] << '|';
		}
		cout << "\n";
	}
	cout << "\n";
}

double** create2DGrid(int n_rows, int n_columns) {
	//Setup the Grid
	double** arr = new double* [n_rows];
	for (int i = 0; i < n_rows; i++) {
		arr[i] = new double[n_columns];
	}

	//Initialize all values to 0
	for (int i = 0; i < n_rows; i++) {
		for (int j = 0; j < n_columns; j++) {
			arr[i][j] = 0;
		}
	}
	return arr;
}

void setBoundaryConditions(double** arr, int n_rows, int n_columns) {
	//Ask user for Initial side temperature values and store in an array SideTemp.
	double SideTemp[4];
	cout << "Enter temperature values for the 4 sides of the grid:" << endl;
	cout << "Enter temperature value for the top of the plate: " << endl;
	cin >> SideTemp[0];
	cout << "Enter temperature value for the bottom of the plate:" << endl;
	cin >> SideTemp[1];
	cout << "Enter temperature value for the right side of the plate:" << endl;
	cin >> SideTemp[2];
	cout << "Enter temperature value for the left side of the plate: " << endl;
	cin >> SideTemp[3];
	//Update the side temperature values in the grid
	//Update top and bottom
	for (int j = 0; j < n_columns; j++) {
		arr[0][j] = SideTemp[0]; //Top row
		arr[n_rows - 1][j] = SideTemp[1]; //Bottom row
	}
	//Update sides
	for (int i = 1; i < n_rows - 1; i++) {
		arr[i][0] = SideTemp[3];//Left column
		arr[i][n_columns - 1] = SideTemp[2];//Right columns
	}
}


void stimulateGrid(double** arr, int n_rows, int n_columns, int& stim_row , int& stim_col ) {
	//Program to stimulate the grid at one point
	int row_num;
	int col_num;
	double stim_val;
	bool valid_input(true); //boolean flag to check whether row and col input is valid
	do {
		valid_input = true;
		cout << "Enter Row number of the stimulation:(from top, row 0 is the topmost row)" << endl;
		cin >> row_num;


		cout << "Enter Column number of the stimulation:(from left, column 0 is the leftmost column)" << endl;
		cin >> col_num;

		cout << "Enter the stimulation value :" << endl;
		cin >> stim_val;

		//Validate row and column numbers 
		if ((row_num == 0) || (col_num == 0) || (row_num == n_rows - 1) || (col_num == n_columns - 1)) {
			cout << "Invalid Input, you cannot simulate at the boundary of the plate" << endl;
			valid_input = false;
			cout << "Try again" << endl;
		}
		if (row_num >= n_rows || row_num < 0) {
			cout << "Row number of stimulation is out of bounds" << endl;
			valid_input = false;
			cout << "Try again" << endl;
		}
		if (col_num >= n_columns || col_num < 0) {
			cout << "Column number of stimulation is out of bounds" << endl;
			valid_input = false;
			cout << "Try again" << endl;
		}
	} while (valid_input == false);
		//Rerun the program before the next line if input is invalid

		arr[row_num][col_num] = stim_val; //sets the value for given input to stimulation
		stim_row = row_num;
		stim_col = col_num;
	
}
void simulateThermalDissipation(double** arr, double threshold, int n_rows, int n_columns ,int stim_row ,int stim_col) {
	//Calculate thermal distribution for each  iteration until the threshold value is reached
	int new_val;
	int count_threshold_reached = 0;
	int arr_interior_size = (n_rows - 2) * (n_columns - 2);

//Code to update the non-side values
	bool equal_reached; //check whether equilibrium has been reached
	do {
		equal_reached = true;
		for (int i = 1; i < n_rows - 1; i++) {
			for (int j = 1; j < n_columns - 1; j++) {
				if (!((i == stim_row) && (j == stim_col))) { //to keep stimulation point unchanged

					new_val = (arr[i + 1][j] + arr[i][j + 1] + arr[i - 1][j] + arr[i][j - 1]) / 4;//calculate new values
					if ((abs(arr[i][j]- new_val)) >= threshold) {
						equal_reached = false; //if any diff (values)is greater than threshold, equilibrium has not been reached .

					}
					arr[i][j] = new_val;

				}
			}
		}
	} while (!equal_reached); //run until equilibrium is reached

	//convert double 2_D array to uint8_t 2_D array
	uint8_t** (arr_n) = convert_to_Uint(arr, n_rows, n_columns);
	const char* filename = "Bitmap_image.bmp";

	//create image
	writeBitmap(filename, arr_n, n_columns, n_rows);

	//Deallocate the Unsigned integer array as it is no longer needed
	deallocate(arr_n, n_rows);
}


uint8_t** convert_to_Uint(double** arr, int n_rows, int n_columns) {

		//Create a Uint_8 array.
		uint8_t** arr_n = nullptr;
		arr_n = new uint8_t * [n_rows];
		for (int i = 0; i < n_rows; i++) {
			arr_n[i] = new uint8_t[n_columns];
		}

		double max_v = arr[0][0];//To find max value 
		double min_v = 0;// min value will be 0

		// To find range of the numbers
		for (int i = 0; i < n_rows; i++) {
			for (int j = 0; j < n_columns; j++) {
				if (arr[i][j] > max_v) {
					max_v = arr[i][j];
				}
			}
		}

		double range = max_v - min_v;
		double old_value;
		double new_value;
		double new_range = 255;
		//Formula for converting one range into another . 
		/*NewRange = (NewMax - NewMin);
		NewValue = (((OldValue - OldMin) * NewRange) / OldRange) + NewMin;*/
		for (int i = 0; i < n_rows; i++) {
			for (int j = 0; j < n_columns; j++) {
				old_value = arr[i][j];
				new_value = ((old_value - min_v) * 255/ range) + min_v;
				arr_n[i][j] = (int)(new_value);

			}
		}

		return arr_n; //return the converted Uint8_t array

	}

void deallocate(double** arr,int n_rows) { //deallocate for 2-D grid of doubles
	//deallocation
	for (int n = 0; n < n_rows; n++) {
		delete[] arr[n];
	}
	delete[] arr;
}

void deallocate(uint8_t** arr, int n_rows) { //deallocate for 2-D grid of uint8_t 
	//deallocation
	for (int n = 0; n < n_rows; n++) {
		delete[] arr[n];
	}
	delete[] arr;
}



