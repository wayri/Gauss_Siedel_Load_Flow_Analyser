/* Gauss_Seidel_Load_Flow_Analyzer.cpp :
@author - Yawar Badri (c) copyright 2019
@version - 2.3/2019
@Title - Gauss Seidel Load Flow analyzer

!NOTE - The program uses vectors to create a matrix at run time thus for
		large matrix sizes memory allocation may go unchecked and might lead
		to unknown behaviour of the program. For efficiency reasons keep the
		matrix size below 100. Since there are multiple multi-dimensional vectors.
		The program implementation of handling complex numbers generally utilizes
		default operator overloading. However, certain code segments are implemented
		as required.

		! - this program has very basic error handling, however a lot of scenarios are
			still not implemented and might thus lead to unknown behaviour which
			might cause harm to your system.

		Some of the known issues are;
		1.	if a alphanumeric input is given where the program asks for a number,
				the program becomes unstable and goes into a uncontrolled loop..
*/


#include <iostream>
#include <string>
#include <vector>
#include <new>
#include <complex>

//DEFINITIONS

// This placeholder defines 
//the number of bus parameters to be taken from the user
#define N_PARAM 2	
//Program defined limit on number of iterations | to avoid stability problems
#define THRESHOLD 2000
//Tolerance for GSLF variables | 
// there are 2 parameters whose tolerances are defined TOL_V and TOL_A, for voltage magnitude and angle respectively
#define TOL_V 0.001
#define	TOL_A 0.001

//ENUMERATIONS
// define bus types to alter the processing in accordance to the type
enum tbus
{
	SLACK = 1,
	LOAD,
	GEN
};

enum pbus_typ
{
	/*
	0 = P = Net Real Power on the Bus
	1 = Q = Net Reactive Power on the Bus
	2 = V = Bus Voltage
	3 = d = Bus Voltage angle
	*/

	//P, Q, V, d
	// S = P + jQ	both bus power types
	S = 0,
	// V = |V| /_V bus voltage and angle
	V
};

//PROTOTYPES
void pinit();
void bam_gen(std::vector<std::vector<std::complex<double>>>& bam);
void handler_bam_size();
void bus_type(std::vector<tbus>& t_bus);
void bus_param(std::vector<std::vector<std::complex<double>>>& pbus);
void print_bm(std::vector<std::vector<std::complex<double>>>& bam);
void bus_voltage(std::vector<tbus>& t_bus, std::vector<std::vector<std::complex<double>>>& pbus, std::vector<std::vector<std::complex<double>>>& bam);
void help();

//#GLOBALS
std::string str;		//input buffer variable hold
bool index = true;		//control bam loop

//DO NOT ALTER++++++++++
int size_b = 2;			//declaration of basic default value for the BAM size value
//++++++++++++++++++++++


//CLASSES
class bvec
{
public:
	std::complex<double> val;					//implement complex number handling 21-04-2019||2100
	unsigned int n1 = 0, n2 = 0;
};
bvec vect;



int main()
{
	pinit();
#pragma region CORE_VARIABLE_def
	//define the BAM here and pass it as reference to other functions
	handler_bam_size();
	//---------------------Generate Dynamic BAM Array------------------------------------------------
	std::vector<std::vector<std::complex<double>> > bam(size_b, std::vector<std::complex<double>>(size_b));
	//---------------------Generate BUS type Array---------------------------------------------------
	std::vector<tbus> t_bus(size_b) ;
	//---------------------Generate BUS PARAM Array--------------------------------------------------
	std::vector<std::vector<std::complex<double>>> pbus(size_b, std::vector<std::complex<double>>(N_PARAM));		//vector of 1x2 arrays | dimensions [size_b x 2]

#pragma endregion

	

	while (true)
	{
		/*
		PROGRAM LOOP

		Do not alter.
		*/

		
#pragma region Program_Handler



		std::cout << "\t\t\n\a MAIN - Enter command | bam | tbus | pbus | GSLF | exit | print | help\n";
		std::cin >> str;

		if (str == "bam")
			bam_gen(bam);					//Start the program for taking in bus matrix values directly												| IMPLEMENTED 100 % --
		if (str == "tbus")	
			bus_type(t_bus);				//Start the program for taking in bus classification defined in enum tbus.									| IMPLEMENTED 100 % --
		if (str == "pbus")
			bus_param(pbus);				//Start the program for taking in bus parameters, like voltage, P, Q, angle depending upon the system		| IMPLEMENTED 100 %	--
		if (str == "GSLF")
			bus_voltage(t_bus, pbus, bam);	//Start Computing the Load_Flow equations | using the user-fed data											| IMPLEMENTED 012 % ~
		if (str == "exit")
			exit(0);						//Terminate the program when exit is entered in the Main Command layer in the console						| IMPLEMENTED 100 % --
		if (str == "print")
			print_bm(bam);					//Print the Bus Admittance Matrix (BAM) on the screen														| IMPLEMENTED 100 % -- 
		if (str == "help")
			help();							//Print the help document on the screen																		| IMPLEMENTED 100 % TempComp
		index = true;						//Reset the index to reset the bus_gen control loop


#pragma endregion
	}


}


void handler_bam_size()
{
	//OBTAIN THE SIZE OF THE BUS MATRIX----------------------------------------------------------

	while (true) {
		std::cout << "enter the number of junctions' = ";
		std::cin >> size_b;

		//error handling
		if (size_b >= 2) {
			break;
		}
		std::cout << "\n\t/!\\ - invalid input\n";
		if (size_b < 2)
		{
			std::cout << "/x\\ terminating program - unhandled exception !\n \a\a ";
			exit(0);
		}
	}
	//std::cout << "\nprogram flow after matrix size,\n size = " << size_b;										//debug 1 
}

void bam_gen(std::vector<std::vector<std::complex<double>>>& bam)
{
	
	//initialise the BAM to zero 
	for (int i = 0; i < size_b; i++)																			// it works | still write a test for it
	{
		for (size_t j = 0; j < size_b; j++)
		{
			bam[i][j] = 0;
		}
	}		

#pragma region TEST1
	for (int i = 0; i < size_b; i++)
	{
		for (int j = 0; j < size_b; j++)
		{
			std::cout << bam[i][j] << "\t";
		}
		std::cout << std::endl;
	}
#pragma endregion


	//std::cout << "\nprogram flow after array-dyn creation \t\n"; print_bm();				//debug 2

	std::cout << "\nEnter admittance as per the mentioned format! \n";

	while (index)
	{

		std::cout << "enter the element value, node 1 and node 2\n";
		std::cin >> vect.val >> vect.n1 >> vect.n2;
		std::cin.clear();
		//std::cout << "val " << vect.val << "\nn1 " << vect.n1 << "\nn2 " << vect.n2;		//debug 4
		if ((vect.n1 > size_b) || (vect.n2 > size_b))
		{
			//std::cout << "\nprogram flow in <if> test";		//debug 3
			std::cout << "\nunhandled exception! - terminating program\n";
			exit(0);
		}
		else
		{
			std::cout << "------------------\n";
			//handle buses with a common reference | ie with either node = 0
			if (vect.n1 == 0)
			{
				bam[vect.n2 - 1][vect.n2 - 1] += vect.val;
			}
			else if (vect.n2 == 0)
			{
				bam[vect.n1 - 1][vect.n1 - 1] += vect.val;
			}
			else
			{
				bam[vect.n1 - 1][vect.n2 - 1] -= vect.val;
			}
			//handle case: for buses not connected to reference the values are added to both indices
			//also replicate the admittance as reverse
			if ((vect.n1 != 0 && vect.n2 != 0) && (vect.n1 != vect.n2))
			{
				bam[vect.n1 - 1][vect.n1 - 1] += vect.val;
				bam[vect.n2 - 1][vect.n2 - 1] += vect.val;
				bam[vect.n2 - 1][vect.n1 - 1] -= vect.val;						// implemented inherent negative non-diagonal elements

			}

		}


		//print the vector back
		std::cout << "\n\t vector[ " << vect.val << ", " << vect.n1 << ", " << vect.n2 << " ]\n\n";

		//check if the input is terminated by user
		std::cin >> str;
		if (str == "end")
		{
			index = false;
			break;
		}

	}
}

void bus_type(std::vector<tbus>& t_bus)
{
	//tbus t;	// declare t as type enum tbus
	/*
	This function takes input from user regarding the bus type as a type of enumerator tbus | TYPE  = SLACK, LOAD, GEN
	*/
	int indx;
	int typ;
	std::cout << "\n\nINPUT BUS CLASS - in the following format |"
				 "[bus index, TYPE]\n"
				 "SLACK = 1, LOAD = 2, GEN = 3"<< std::endl;
	while (true)
	{
		std::cout << "\nEnter Bus Index\n" << std::endl;
		std::cin >> indx;
		std::cout << "\nEnter Bus TYPE" << std::endl;
		std::cin >> typ;

			switch (typ)
			{
			case SLACK:
				t_bus[indx - 1] = SLACK;
				break;
			case LOAD:
				t_bus[indx - 1] = LOAD;
				break;
			case GEN:
				t_bus[indx - 1] = GEN;
				break;
			default:
				std::cout << "\n ! - Error invalid input" << std::endl;
				break;
			}

		std::cout << "press 'e' to end the bus classifier program\n"
					 "press 'p' to print the current bus status\n";
		std::cin >> str;
		if (str == "e")
			break;
		if (str == "p")
		{
			for (size_t i = 0; i < size_b; i++)
			{
				switch (t_bus[i])
				{
				case 1:
					std::cout << "SLACK" << "\t";
					break;
				case 2:
					std::cout << "LOAD" << "\t";
					break;
				case 3:
					std::cout << "GEN" << "\t";
					break;
				default:
					std::cout << "\n ! - OVERFLOW" << "\t";
					break;
				}
				
			}
		}
	}
	return;
}

void bus_param(std::vector<std::vector<std::complex<double>>>& pbus)
{
	using namespace std;
	/*
	This function takes in bus parameters.
	Enter the data in the given format [BUS index, P, Q, V, d]
	*/
	cout << "\nenter the data in the following format\n"
			"[BUS index, S, V]" << endl;

	complex<double> buff = (0.0,0.0);
	int ind = 0;
	while (true)
	{
		cout << "\nenter BUS INDEX = " << endl;
		cin >> ind;
		for (size_t i = 0; i < N_PARAM; i++)
		{	
			switch (i)
			{
			case 0:
				cout << "\tS [P, Q] = ";
				cin >> buff;
				break;
			case 1:
				cout << "\tV = ";
				cin >> buff;
				break;
			//case 2:
			//	cout << "\tV = ";
			//	cin >> buff;
			//case 3:
			//	cout << "\td = ";
			//	cin >> buff;
			default:
				break;
			}
			pbus[ind - 1][i] = buff;

		}
	
		cout << "\nenter 'p' to print parameters | 'e' to exit <pbus>" << endl;
		cin >> str;
		if (str == "p")
		{
			for (size_t i = 0; i < size_b; i++)
			{
				cout << "BUS " << i+1 << endl;

				for (size_t j = 0; j < N_PARAM; j++)
				{
					cout << pbus[i][j] << endl;
				}
				cout << endl;
			}
		}
		if (str == "e")
			break;
	}

	return;
}

void print_bm(std::vector<std::vector<std::complex<double>>>& bam)
{
	/*
	This function prints the BAM to the screen in the complex number format.
	*/
	for (int i = 0; i < size_b; i++)
	{
		for (int j = 0; j < size_b; j++)
		{
			std::cout << bam[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	return;
}

void bus_voltage(std::vector<tbus>& t_bus, std::vector<std::vector<std::complex<double>>>& pbus, std::vector<std::vector<std::complex<double>>>& bam)
{
	/*
	THIS IS THE CORE PROCESSING FUNCTION | IT TAKES AS ARGUMENTS
	
	! -	PROGRAM MEMBER VECTORS AND VARIABLES TO ITERATE A SOLUTION WITH
			PREDEFINED TOLERANCE LIMITS.

		t_bus	-	defines the type of bus 
						tbus[k]
		pbus	-	defines the parameters of bus
						pbus[k][S]	=	pbus[k][0]	! - S and V are defined in the enumerator <pbus_typ>	
						pbus[k][V]	=	pbus[k][1]	
		bam		-	defines the bus admittance matrix
						bam[i][j]	
							where, i and j are the respective bus indices between which the impedance is 
							observed


	/!\ -RULES 

	1.	Do Not Modify The Member Vectors | as they are referenced. Doing so
		will lead to maloperation of the program and can cause the program to
		crash, losing all the feed data in the process.
	2.	The member equations are inbuilt and shall not be altered
		these equations are generic for Load Flow Analysis.
		

	BUS VOLTAGE EQUATION								   i-1						   i-1
	V_i(k) =  ____1_____ |` (_(real(S_i), -imag(S_i))_)	-	E	bam[i][j] * V_j(k)	-	E	bam[i][j] * V_j(k-1)	`|
			   bam[i][i] |_       conj( V_i(k-1) )         j=1						   j=1							_|
	
	where;	k is the current iteration
			E  is the sigma summation

	Also for Q BUS REACTIVE POWER
													   i-1						    N
	Q_i(k)	=	-imag	|`	conj( V_i(k-1) )	|`		E	bam[i][j] * V_j(k)	+	E	bam[i][j] * V_j(k-1)	`|	`|
						|_						|_	   j=1						   j=1							_|	_|
	
	*/
	int iter = 0;
	int b = 1;
	//The V_vect 1D vector of size (size_b - 1) is used to hold voltage angle values of the buses
	//It however does not hold the values pertaining to the slack bus
	std::vector<std::complex<double>> V_vect(size_b);
	std::complex<double> in1 = (1,0);		 //Complex dummy integer replacement for '1' to avoid conflict of types
	std::complex<double> tmp_S = (0, 0);	 //holds the new value for the numerator term temporarily | See TMP_S
	std::complex<double> V_tmp = (1, 0);     //temporary value for voltage iteration
	std::complex<double> Vj_old = (0, 0);	 //temp var to hold value of voltage for previous iteration
	std::complex<double> T1 = (0, 0);		 //See below for explanation of  T1 and T2
	std::complex<double> T2 = (0, 0);		 //____________________________________________				
	std::complex<double> Q = (0, 0);		 //Reactive power temp var
	bool condition = false;
	while (condition == false)
	{
		/*
		This loop performs a tolerance check on the parameters to break the loop
		However, the loop will stop executing if the number of iterations goes beyond
		threshold value as defined in definition THRESHOLD
		*/
		
		for (size_t i = 1; i < size_b; i++)
		{
			// This loop will do a pass on all the buses of the system except the slack bus
			// These values will be then used in the following iterations at the instant and
			// or after the one whole iteration is performed
			//std::complex<int> j = bam[i][2] ;
			
			tmp_S = (real(pbus[i][S]), -imag(pbus[i][S]));			// TMP_S

			//HERE ! am
			// compute the two summation terms first		|			T1  "yij_vjk" |	T2	 "yij_vj_old"

			//T1		| See BUS VOLTAGE EQUATION
			for (size_t j = 0; j < i; j++)
			{
				T1 += bam[i][j] * V_vect[j];					//Here V_vect[j] is the new value for the iteration
			}

			//T2
			for (size_t j = i+1 ; j <= size_b; j++)
			{
				T2 += bam[i][j] * V_vect[i];	// --Vj_old is a temp var to hold previos value for this iter--
			}

			//Vj_old = V_vect[i];

			switch (b)
			{
			case SLACK:		// PARAM  of SLACK BUS STAY CONSTANT
				V_vect[i] = (1, 0);
				break;

			case LOAD:		//BUS VOLTAGE FOR LOAD BUSES
				// use the previously calculated parameters to obtain the value for bus voltage
				
				V_vect[i] = (in1 / bam[i][i]) * ((tmp_S / conj(V_vect[i])) - T1 - T2);
				break;
			
			case GEN:		//BUS VOLTAGE FOR GENERATOR BUSES
				// compute the reactive power first, then calculate voltage angle and alter the voltage accordingly
				//		by retaining the magnitude and affixing the angle to the value
				Q = -imag( conj(V_vect[i]) * (T1 + T2) );
				V_tmp = arg((in1 / bam[i][i]) * (((real(pbus[i][S]), -Q) / conj(V_vect[i])) - T1 - T2));
				//now obtain angle and after affixing it to magnitude add to matrix
				V_vect[i] = abs(pbus[i][V]) * (cos(V_tmp), sin(V_tmp));
				break;

			default:
				std::cout << "??\t\t/x\\\a - Unhandled exception error!\a\t\t Terminating Program" << std::endl;
				break;
			}

			
			

		}


		/*
		Check if tolerance is satisfies
		*/
		/*for (size_t i = 0; i < size_b; i++)
		{
			if ()
			{

			}
		}*/


		
		if (iter == THRESHOLD)
		{
			std::cout << "\n\n\t\t! - iteration THRESHOLD encountered. Stopping computation" << std::endl;
			break;
		}
		std::cout << "DEBUG - print Voltages ? y/n";
		std::cin >> str;
		if (str == "y")
		{
			std::cout << "printing voltages . . .\n";
			for (size_t i = 0; i < size_b; i++)
			{
				std::cout << "V1 = " << V_vect[i] << "\n";
			}
		}
	
	}
	return;
}

void help()
{
	pinit();
	return;
}


void pinit()
{
	using namespace std;
	cout << "\a\n\t\tGAUSS SEIDEL LOAD FLOW SOLVER\n" << endl;
	cout << "\t\t\tConsole Application | v2.3" << endl;
	cout << "COMMANDS \n"
			"\tbam\t-\tlets you enter the bus admittance matrix\n"
			"\ttbus\t-\tlets you enter the type of each bus \n\t\t! - dont leave any unclassified buses. This will cause an error\n"
			"\tpbus\t-\tlets you enter the net bus power and voltage phasor\n"
			"\tGSLF\t-\tThis command starts the Gauss-Seidel Solver\n" << endl;

	return;
}


