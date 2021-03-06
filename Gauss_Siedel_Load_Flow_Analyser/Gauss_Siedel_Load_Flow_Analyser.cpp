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
		2.	There is a problem of rounding in the complex numbers as well. This can
				later cause problems with checking for tolerance
*/

#include "pch.h"
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
#define THRESHOLD 5
//Tolerance for GSLF variables | 
// there are 2 parameters whose tolerances are defined TOL_V and TOL_A, for voltage magnitude and angle respectively
#define TOL_V 0.001
#define	TOL_A 0.001

//ENUMERATIONS
// define bus types to alter the processing in accordance to the type
enum tbus
{
	SLACK = 0,
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
double q_lim_top = 1.654151;	//define reactive power limits	|	upper
double q_lim_low = 0.000001;	//define reactive power limits	|	lower
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

		//test------------------
		//set number of buses to 4 for this test, uncomment the line below | and execute the program
		//bus_voltage(t_bus, pbus, bam);
		//-----------------------

		std::cout << "\t\t\n\a MAIN - Enter command | bam | tbus | pbus | GSLF | exit | print | help\n";
		std::cin >> str;

		if (str == "bam")
			bam_gen(bam);					//Start the program for taking in bus matrix values directly												| IMPLEMENTED 100 % --
		if (str == "tbus")	
			bus_type(t_bus);				//Start the program for taking in bus classification defined in enum tbus.									| IMPLEMENTED 100 % --
		if (str == "pbus")
			bus_param(pbus);				//Start the program for taking in bus parameters, like voltage, P, Q, angle depending upon the system		| IMPLEMENTED 100 %	--
		if (str == "GSLF")
			bus_voltage(t_bus, pbus, bam);	//Start Computing the Load_Flow equations | using the user-fed data											| IMPLEMENTED 082 % ~
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

//#pragma region TEST1
//	for (int i = 0; i < size_b; i++)
//	{
//		for (int j = 0; j < size_b; j++)
//		{
//			std::cout << bam[i][j] << "\t";
//		}
//		std::cout << std::endl;
//	}
//#pragma endregion
//

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
		if (indx == 0 or indx > size_b)
		{
			std::cout << "\n\t! - Error: INVALID INPUT" <<std::endl;
		}
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
				case SLACK:
					std::cout << "SLACK" << "\t";
					break;
				case LOAD:
					std::cout << "LOAD" << "\t";
					break;
				case GEN:
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
	std::complex<double> o(1, 0);		//declare a complex type 'o' to be used for type casting

	//#pragma region TEST_STANDARD_INPUT
	//	/*
	//	This region includes test routines with a prefed data-set with known outcome
	//	The data replaces the function arguments to work.
	//	This region should be normally commented
	//	*/
	//	
	//	
	//	//BUS_ADMITTANCE_MATRIX
	//	bam[0][0] = decltype(o)(8.8985,-44.8359); bam[0][1] = decltype(o)(-3.8156,19.0781);	bam[0][2] = decltype(o)(-5.1695,25.8478);	bam[0][3] = decltype(o)(0,0);
	//	bam[1][0] = decltype(o)(-3.8156,19.0781); bam[1][1] = decltype(o)(8.8985,-44.8359); bam[1][2] = decltype(o)(0,0); bam[1][3] = decltype(o)(-5.1695,25.8478);
	//	bam[2][0] = decltype(o)(-5.1695,25.8478);	bam[2][1] = decltype(o)(0,0);	bam[2][2] = decltype(o)(8.1932,-40.8638);	bam[2][3] = decltype(o)(-3.0237,15.1185);
	//	bam[3][0] = decltype(o)(0,0);	bam[3][1] = decltype(o)(-5.1695,25.8478);	bam[3][2] = decltype(o)(-3.0237,15.1185); bam[3][3] = decltype(o)(8.1932,-40.8638);
	//
	//	//Type_of_BUS		t_bus
	//	t_bus[0] = SLACK;
	//	t_bus[1] = LOAD;
	//	t_bus[2] = LOAD;
	//	t_bus[3] = GEN;
	//
	//	//BUS Parameters	pbus
	//	pbus[0][S] = decltype(o)(-0.5,-0.3099);	pbus[0][V] = decltype(o)(1,0);
	//	pbus[1][S] = decltype(o)(-1.7,-1.0535);	pbus[1][V] = decltype(o)(1,0);
	//	pbus[2][S] = decltype(o)(-2.0,-1.2394);	pbus[2][V] = decltype(o)(1,0);
	//	pbus[3][S] = decltype(o)(2.38,-0.4958);	pbus[3][V] = decltype(o)(1.02,0);
	//
	//#pragma region SUB_TEST_VAL_DEF_PRINT
	//	//BAM
	//	std::cout << "\n\nBAM\n";
	//	for (size_t p = 0; p < size_b; p++)
	//	{
	//		for (size_t k = 0; k < size_b; k++)
	//		{
	//			std::cout << bam[p][k] << "\t";
	//		}
	//		std::cout << std::endl;
	//	}
	//	std::cout << "\n\nTBUS\n";
	//	//T_BUS
	//	for (size_t i = 0; i < size_b; i++)
	//	{
	//		std::cout << t_bus[i] << std::endl;
	//	}
	//	//PBUS
	//	std::cout << "\n\nPBUS\n";
	//	for (size_t i = 0; i <size_b; i++)
	//	{
	//		for (size_t j = 0; j < 2; j++)
	//		{
	//			std::cout << pbus[i][j];
	//				std::cout << std::endl;
	//		}
	//		std::cout << "\n" <<std::endl;
	//	}
	//#pragma endregion
	//
	//
	//#pragma endregion	  //uncomment this block for test



	//REFACTOR this section
	/*
	This function performs the gauss seidel load flow analysis
	*/

	/*
	Variable Set for this function
	*/
	
	int iter = 0;	//iteration counter
	std::vector<std::complex<double>> V_vect(size_b);	//output and buffer
	std::complex<double> T1(0, 0);	//term 1 for voltage
	std::complex<double> T2(0, 0);	//term 2 for voltage
	std::complex<double> T1_q(0, 0);//term 1 for reactive power calculation
	std::complex<double> T2_q(0, 0);//term 2 for reactive power calculation
	double Q = 0.0;	//temporary variable for GEN_Q
	std::complex<double> slack_power(0, 0);
	std::complex<double> tmp_s(1, 0);
	double V_arg = 0;
	

	//initialize V_vect
	for (size_t i = 0; i < size_b; i++)
	{
		if (t_bus[i] == GEN)
		{
			V_vect[i] = pbus[i][V];		//retain the constant value from the Param_BUS matrix <pbus>
		}
		else
		{
			V_vect[i] = decltype(o)(1, 0);
		}
	}
	std::cout << std::endl;
	for (size_t i = 0; i < size_b; i++)		//debug 5
	{
		std::cout << V_vect[i] << std::endl;
	}
	std::cout << std::endl;
	
	while (true)
	{
		//iteration loop

		for (size_t bind = 0; bind < size_b; bind++)
		{
			//scan through all buses for each iteration
			switch (t_bus[bind])
			{
			case SLACK:
				//if bus is slack bus
				V_vect[bind] = pbus[bind][V];		//decltype(o)(1,0);
				//	std::cout << "\n at slack, iter = " << iter << ", V_vect[" << bind << "] = " << V_vect[bind] << std::endl;			//debug
				break;

			case LOAD:
				//if bus is load bus
				// compute the two summation terms first		|			T1  "yij_vjk" |	T2	 "yij_vj_old"
				
				//T1		| See BUS VOLTAGE EQUATION
				for (size_t j = 0; j < bind; j++)
				{
				//	std::cout << "\n at load T1, bam[" << bind << "][" << j << "] = " << bam[bind][j] << ", V_vect[" << j << "]" << V_vect[j] << std::endl;		//debug
					T1 += V_vect[j] * bam[bind][j];					//Here V_vect[j] is the new value for the iteration
				//	std::cout << "T1 = " << T1 << std::endl;																			//debug
				}
				//T2
				for (size_t j = bind + 1; j < size_b; j++)
				{
				//	std::cout << "\n at load T2, bam[" << bind << "][" << j << "] = " << bam[bind][j] << ", V_vect[" << j << "]" << V_vect[j] << std::endl;		//debug
					T2 += V_vect[j] * bam[bind][j];
				//	std::cout << "T2 = " << T2 << std::endl;																									//debug
					//T2 = 0;
				}
				//	std::cout << "\n at load, T1 = " << T1 << ", T2 = " << T2 << ", Conj(V_vect[bind]) = " << conj(V_vect[bind]);								//debug
				//	V_vect[i] = (in1 / bam[i][i]) * ((tmp_S / conj(V_vect[i])) - T1 - T2);
				
				tmp_s = conj(decltype(o)(pbus[bind][S]));
				V_vect[bind] = (o/bam[bind][bind]) * ((tmp_s/conj(V_vect[bind])) - (T1 + T2));
	
				//	std::cout << "\n at load, iter = " << iter << ", V_vect[" << bind << "] = " << V_vect[bind] << ", tmp_s = " << tmp_s  << std::endl;			//debug
					T1 = decltype(o)(0, 0);			//clear the term value holder to avoid erroneous result
					T2 = decltype(o)(0, 0);
				break;

			case GEN:


				//		for VOltage regarding Q the T1 and T2 calculations are different thus change them, see stevenson page 342


				//T1		| recompute for Q
				for (size_t j = 0; j < bind; j++)
				{
				//	std::cout << "\n\n\t\t at gen T1, bam[" << bind << "][" << j << "] = " << bam[bind][j] << ", V_vect[" << j << "]" << V_vect[j] <<std::endl;		//debug
					T1_q += bam[bind][j] * V_vect[j];					//Here V_vect[j] is the new value for the iteration
				//	std::cout << "T1 = " << T1 << std::endl;																										//debug
				}
				//T2		| recompute for Q
				for (size_t j = bind; j < size_b; j++)
				{
				//	std::cout << "\n\n\t\t at gen T2, bam[" << bind << "][" << j << "] = " << bam[bind][j] << ", V_vect[" << j << "]" <<  V_vect[j] << std::endl;	//debug
					T2_q += bam[bind][j] * V_vect[j];
				//	std::cout << "T2 = " << T2 << std::endl;																										//debug
					//T2 = 0;
				}
				Q = 0;
				Q = -imag(conj(V_vect[bind]) * (T1_q + T2_q));
				//	std::cout << "\n at gen, Q = " << Q << ", T1_q = " << T1_q << ", T2_q = " << T2_q << ", Conj(V_vect[bind]) = " << conj(V_vect[bind]);			//debug

					//recalculate T1 and T2 for voltage
					//T1		| See BUS VOLTAGE EQUATION
					for (size_t j = 0; j < bind; j++)
					{
				//		std::cout << "\n at gen T1, bam[" << bind << "][" << j << "] = " << bam[bind][j] << ", V_vect[" << j << "]" << V_vect[j] << std::endl;		//debug
						T1 += V_vect[j] * bam[bind][j];					//Here V_vect[j] is the new value for the iteration
				//		std::cout << "T1 = " << T1 << std::endl;																									//debug
					}
					//T2
					for (size_t j = bind + 1; j < size_b; j++)
					{
					//	std::cout << "\n at gen T2, bam[" << bind << "][" << j << "] = " << bam[bind][j] << ", V_vect[" << j << "]" << V_vect[j] << std::endl;		//debug
						T2 += V_vect[j] * bam[bind][j];
					//	std::cout << "T2 = " << T2 << std::endl;																									//debug
						//T2 = 0;
					}


				tmp_s = conj(decltype(o)(real(pbus[bind][S]), Q));
				V_arg = arg((o / bam[bind][bind]) * ((tmp_s / conj(V_vect[bind])) - T1 - T2));
				V_vect[bind] = norm(V_vect[bind]) * decltype(o)(cos(V_arg), sin(V_arg));
				//	std::cout << "\n at gen, iter = " << iter << ", V_vect[" << bind << "] = " << V_vect[bind] << ", V_arg = " << V_arg  << std::endl;				//debug
					T1 = decltype(o)(0, 0);
					T2 = decltype(o)(0, 0);
					T1_q = decltype(o)(0, 0);
					T2_q = decltype(o)(0, 0);
					
				break;
				
			default:
				std::cout << "\a\n\n\t\t/x\\ - Error! UNHANDLED EXCEPTION";
				break;
			}
		
			/*
		---------------------------------------------------------------------------------
			COMPUTE POWER FOR SLACK BUS																					//EXTEND TO ALL BUS POWERS
										N				
				P_i - jQ_i = V_i*	x	E bam[i][n] V[n]
									   n=1
			*/
				for (size_t ix = 0; ix < size_b; ix++)
				{
					slack_power += bam[0][ix] * V_vect[ix];
				}
				slack_power = conj(V_vect[0]) * slack_power;
				//assign to the pbus[0][S]
				pbus[0][S] = slack_power;
			//----------------------------------------------------------------------------------

		}

		//replace by turning PV bus into PQ bus in case of Q - overflow
		if (Q > q_lim_top || Q < q_lim_low)
		{
			std::wcout << "\n\n\t\t/!\\ - unhandled exception - Q values has oveflowed, Q = " << Q << std::endl;
			//	std::wcout << "\n\n\t\t/!\\ - Q values has oveflowed, Q = " << Q << "changing bus to type_ t_bus[LOAD]" << std::endl;
			//	t_bus[bind] = LOAD;
			break;
		}


		iter++;
		if (iter == THRESHOLD)
		{
			std::cout << "\n\n\t\t! - OVERFLOW\n\t\t terminating execution";
			break;
		}
	}



	//! - compute slack bus power from bus voltages | copy VS code


	//Print the output voltage vector
	std::cout << std::endl;
	for (size_t i = 0; i < size_b; i++)
	{
		std::cout << " V_" << i + 1 << " = " << V_vect[i];
		std::cout << std::endl;
	
	}

	for (size_t i = 0; i < size_b; i++)
	{
		std::cout << "S_" << i + 1 << " = " << pbus[i][S];
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