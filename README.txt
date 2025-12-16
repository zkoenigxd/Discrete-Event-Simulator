***** Compiling the program: *****

In the linux command line, enter “g++ SystemSim.cpp” while in the directory containing the file

***** Running the program: *****

In the linux command line, enter “./a.out” in the directory containing the compiled executable

The program will start with the following menu:

0 : Run a simulation with user input.
1 : Run a series of simulations and print the data to file.
2 : Quit.
Enter an option: 

Enter an integer option.

0 : Run a simulation with user input.
	Prompts the user for an average service time.
		Will not accept negative numbers
		Numbers greater than 10 or larger than 30 will not be accepted without user override
		User can override this by entering “-1”, then entering the desired non-negative value
	Prompts the user for an average arrival rate.
		Will not accept negative numbers.
		Expects a value of “.04”, but will run other values
	Runs a single simulation and outputs the experimental data values from the simulation, followed by the expected values
		Expected values are obtained by the standard equations assuming a M/M/1 system at a utilization of less than one and an infinite queue size.
		Expected values produce invalid outputs at CPU utilizations greater than 100% (expected result, formulas are invalid in this condition)

1 : Run a series of simulations and print the data to file.
	Immediately runs a series of simulations and outputs the data to a .csv file in the same directory as the executable.
	Hard-coded to run approximately 2000 sims (lambda step-change of .01)
		To change sim number, edit the global constant “SIM_NUMBER”
2 : Quit.
	Exits program

Random seed will not reset unless the program is exited and run again.
