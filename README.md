This is a parrallel program to [Bus_Matrix_Generator](https://github.com/wayri/bus_matrix)

# Gauss_Siedel_Load_Flow_Analyser
This is a console program that uses complex vectors (in C++ lingo) to perform gauss seidel analysis on the data fed by the user.
The program is documented in the comments. 
## Flow
The program flow begins with taking in the required data as | Bus Admittance Matrix (see [bus matrix](https://github.com/wayri/bus_matrix)), Bus Parameters as Apparent Power 'S' and phasor Voltage 'V', Bus type among; SLACK, LOAD or GEN bus. The function call to bus_voltage(--) function should be performed only after the required data is entered as a whole and verified. The program does not implement in-built protection against such scenarios where any data would be incomplete.
