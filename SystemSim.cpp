#include <iostream>
#include <cmath>
#include <fstream>

const int PROCESSES_TO_RUN = 10000;
const float AVG_SERVICE_TIME = .04f;
const float MIN_AVG_ARRIVAL_RATE = 10;
const float MAX_AVG_ARRIVAL_RATE = 30;
const float SIM_NUMBER = 200;

enum EventType
{
	Arrival,
	Departure
};

struct Event
{
	double clockTime;
	double arrivalTime;
	double serviceTime;
	EventType eventType;
	Event* next;
};

struct Process
{
	double arrivalTime;
	double serviceTime;
	Process* next;
};

double GenerateRandTime(float avgRate)
{
	double rand = (double)std::rand() / (double)RAND_MAX;
	return (-1.0 / avgRate) * std::log(1 - ((rand == 1) ? 0 : rand));
}

float OverRideInput()
{
	float avgArrRate = 0;
	while (avgArrRate <= 0)
	{
		std::cout << std::endl << "Invalid input. Arrival Rate must be greater than zero";
		std::cout << std::endl << "Enter a vaild arrival rate: ";
		std::cin >> avgArrRate;
	}
	return avgArrRate;
}

float GetUserArrivalRate()
{
	float avgArrRate;
	std::cout << "Enter the average arrival rate: ";
	while (!(std::cin >> avgArrRate))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << std::endl << "Error reading input." << std::endl;
		std::cout << "Enter numerical average arrival rate: ";
	}
	while (avgArrRate < MIN_AVG_ARRIVAL_RATE || avgArrRate > MAX_AVG_ARRIVAL_RATE)
	{
		std::cout << std::endl << "Invalid input. Arrival Rate should be between 10 and 30.";
		std::cout << std::endl << "Enter a vaild arrival rate or enter -1 to override: ";
		while (!(std::cin >> avgArrRate))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << std::endl << "Error reading input." << std::endl;
			std::cout << "Enter numerical average arrival rate: ";
		}
		if (avgArrRate == -1)
			return OverRideInput();
	}
	return avgArrRate;
}

float GetUserServiceTime()
{
	float avgSerTime;
	std::cout << "Enter the average service time: ";
	while (!(std::cin >> avgSerTime))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << std::endl << "Error reading input." << std::endl;
		std::cout << "Enter numerical average service time: ";
	}
	while (avgSerTime <= 0)
	{
		std::cout << std::endl << "Invalid input. Service time must greater than zero.";
		std::cout << std::endl << "Enter the average service time: ";
		while (!(std::cin >> avgSerTime))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << std::endl << "Error reading input." << std::endl;
			std::cout << "Enter numerical average service time: ";
		}
	}
	return avgSerTime;
}

int GetUserNumCPUs()
{
	int numCPUs;
	std::cout << "Enter the number of CPUs: ";
	while (!(std::cin >> numCPUs))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << std::endl << "Error reading input." << std::endl;
		std::cout << "Enter integer value for CPU number: ";
	}
	while (numCPUs <= 0)
	{
		std::cout << std::endl << "Invalid input. Number of CPUs must greater than zero.";
		std::cout << std::endl << "Enter the number of CPUs: ";
		while (!(std::cin >> numCPUs))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << std::endl << "Error reading input." << std::endl;
			std::cout << "Enter integer value for CPU number: ";
		}
	}
	return numCPUs;
}

class EventQueue
{
private:
	Event* queue;

	void AddEventByTime(Event* e)
	{
		if (queue == nullptr || e->clockTime < queue->clockTime)
		{
			Event* temp = queue;
			queue = e;
			e->next = temp;
		}
		else
		{
			Event* cursor = queue;
			while (cursor->next != nullptr && e->clockTime >= cursor->next->clockTime)
			{
				cursor = cursor->next;
			}
			Event* temp = cursor->next;
			cursor->next = e;
			e->next = temp;
		}
	}

	Event* CreateEvent(EventType type, double arrivalTimeStamp, double serviceTime, double clockTime)
	{
		Event* newEvent = new Event;
		newEvent->arrivalTime = arrivalTimeStamp;
		newEvent->serviceTime = serviceTime;
		newEvent->clockTime = clockTime;
		newEvent->eventType = type;
		newEvent->next = nullptr;
		return newEvent;
	}

public:
	EventQueue()
	{
		queue = nullptr;
	}

	~EventQueue()
	{
		while (queue != nullptr)
		{
			Event* temp = queue->next;
			delete queue;
			queue = temp;
		}
	}

	Event* Pop()
	{
		Event* r = new Event;
		r->clockTime = queue->clockTime;
		r->arrivalTime = queue->arrivalTime;
		r->serviceTime = queue->serviceTime;
		r->eventType = queue->eventType;
		r->next = r->next;
		Event* p = queue;
		queue = queue->next;
		delete p;
		return r;
	}

	void ScheduleEvent(EventType type, double arrivalTime, double serviceTime, double clockTime)
	{
		AddEventByTime(CreateEvent(type, arrivalTime, serviceTime, clockTime));
	}
};

class FCFSReadyQueue
{
private:
	Process* queue;
	Process* queueEnd;
	int count;
	double changeTimeStamp = 0;
	double timeWeightedSum = 0;

	void UpdateTimeWeightedSum(double clock)
	{
		timeWeightedSum += (clock - changeTimeStamp) * count;
		changeTimeStamp = clock;
	}

public:
	FCFSReadyQueue()
	{
		queue = nullptr;
		queueEnd = nullptr;
		count = 0;
	}

	~FCFSReadyQueue()
	{
		while (queue != nullptr)
		{
			Process* temp = queue->next;
			delete queue;
			queue = temp;
		}
	}

	Process* Pop(double clock)
	{
		UpdateTimeWeightedSum(clock);
		count--;
		Process* r = new Process;
		r->arrivalTime = queue->arrivalTime;
		r->serviceTime = queue->serviceTime;
		Process* p = queue;
		queue = queue->next;
		delete p;
		return r;
	}

	void Push(double clock, double serviceTime, double arrTime)
	{
		UpdateTimeWeightedSum(clock);
		count++;
		Process* newProcess = new Process;
		newProcess->arrivalTime = arrTime;
		newProcess->serviceTime = serviceTime;
		newProcess->next = nullptr;

		if (queue == nullptr)
		{
			Process* temp = queue;
			queue = newProcess;
			newProcess->next = temp;
			queueEnd = newProcess;
		}
		else
		{
			queueEnd->next = newProcess;
			queueEnd = newProcess;
		}
	}

	bool IsEmpty()
	{
		return (queue == nullptr);
	}

	double GetTimeWeightedSum()
	{
		return timeWeightedSum;
	}
};

void RunSim()
{
	float avgArrivalRate;
	float avgServiceTime;
	int numCPUs;
	double expectedUtilization;
	double expectedQ;
	double nextArrivalTime;

	avgArrivalRate = GetUserArrivalRate();
	avgServiceTime = GetUserServiceTime();
	numCPUs = GetUserNumCPUs();

	FCFSReadyQueue readyQueue;
	EventQueue eventQueue;
	double clock = 0;
	double totalServiceTime = 0;
	double totalTimeInSystem = 0;
	int completedProcesses = 0;
	bool cpuIdle = true;

	nextArrivalTime = GenerateRandTime(avgArrivalRate) + clock;
	eventQueue.ScheduleEvent(Arrival, nextArrivalTime, GenerateRandTime(1 / avgServiceTime), nextArrivalTime);

	while (completedProcesses < PROCESSES_TO_RUN)
	{
		Event* currentEvent = eventQueue.Pop();
		clock = currentEvent->clockTime;

		switch (currentEvent->eventType)
		{
		case Arrival:
			nextArrivalTime = GenerateRandTime(avgArrivalRate) + clock;
			eventQueue.ScheduleEvent(Arrival, nextArrivalTime, GenerateRandTime(1 / avgServiceTime), nextArrivalTime);
			if (cpuIdle)
			{
				eventQueue.ScheduleEvent(Departure, clock, currentEvent->serviceTime, clock + currentEvent->serviceTime);
				cpuIdle = false;
			}
			else
				readyQueue.Push(clock, GenerateRandTime(1 / avgServiceTime), clock);
			break;
		case Departure:
			completedProcesses++;
			totalServiceTime += currentEvent->serviceTime;
			totalTimeInSystem += (clock - currentEvent->arrivalTime);
			if (readyQueue.IsEmpty())
				cpuIdle = true;
			else
			{
				Process* nextProcess = readyQueue.Pop(clock);
				eventQueue.ScheduleEvent(Departure, nextProcess->arrivalTime, nextProcess->serviceTime, clock + nextProcess->serviceTime);
				delete nextProcess;
			}
			break;
		default:
			std::cout << "Error processing event" << std::endl;
			break;
		}
		delete currentEvent;
	}

	expectedUtilization = avgArrivalRate * avgServiceTime;
	expectedQ = expectedUtilization / (1 - expectedUtilization);


	std::cout << "--------------------------------------" << std::endl;
	std::cout << "Experimental Values:" << std::endl;
	std::cout << "  Average turnaround time: " << totalTimeInSystem / PROCESSES_TO_RUN << std::endl;
	std::cout << "  Total throughput: " << PROCESSES_TO_RUN / clock << std::endl;
	std::cout << "  Utilization: " << totalServiceTime / clock << std::endl;
	std::cout << "  Average number of processes in Ready Queue: " << readyQueue.GetTimeWeightedSum() / clock << std::endl;
	std::cout << "Expected Values:" << std::endl;
	std::cout << "  Average turnaround time: " << expectedQ / avgArrivalRate << std::endl;
	std::cout << "  Total throughput: " << avgArrivalRate << std::endl;
	std::cout << "  Utilization: " << expectedUtilization << std::endl;
	std::cout << "  Average number of processes in Ready Queue: " << expectedQ - expectedUtilization << std::endl;
	std::cout << "--------------------------------------" << std::endl;
}

void RunDataGenSim()
{
	float avgArrivalRate;
	float avgServiceTime;
	double nextArrivalTime;
	std::ofstream myfile("ServerReport.csv");
	myfile << "Arrival Rate" << ", "
		<< "Turnaround Time" << ", "
		<< "Total Throughput" << ", "
		<< "CPU Utilization" << ", "
		<< "Avg Processes Waiting" << std::endl;


	avgServiceTime = AVG_SERVICE_TIME;
	avgArrivalRate = MIN_AVG_ARRIVAL_RATE;

	float progress = 0.0;
	int progressBarWidth = 70;

	while (avgArrivalRate < MAX_AVG_ARRIVAL_RATE)
	{
		FCFSReadyQueue readyQueue;
		EventQueue eventQueue;
		double clock = 0;
		double totalServiceTime = 0;
		double totalTimeInSystem = 0;
		int completedProcesses = 0;
		bool cpuIdle = true;

		nextArrivalTime = GenerateRandTime(avgArrivalRate) + clock;
		eventQueue.ScheduleEvent(Arrival, nextArrivalTime, GenerateRandTime(1 / avgServiceTime), nextArrivalTime);

		while (completedProcesses < PROCESSES_TO_RUN)
		{
			Event* currentEvent = eventQueue.Pop();
			clock = currentEvent->clockTime;

			switch (currentEvent->eventType)
			{
			case Arrival:
				nextArrivalTime = GenerateRandTime(avgArrivalRate) + clock;
				eventQueue.ScheduleEvent(Arrival, nextArrivalTime, GenerateRandTime(1 / avgServiceTime), nextArrivalTime);
				if (cpuIdle)
				{
					eventQueue.ScheduleEvent(Departure, clock, currentEvent->serviceTime, clock + currentEvent->serviceTime);
					cpuIdle = false;
				}
				else
					readyQueue.Push(clock, GenerateRandTime(1 / avgServiceTime), clock);
				break;
			case Departure:
				completedProcesses++;
				totalServiceTime += currentEvent->serviceTime;
				totalTimeInSystem += (clock - currentEvent->arrivalTime);
				if (readyQueue.IsEmpty())
					cpuIdle = true;
				else
				{
					Process* nextProcess = readyQueue.Pop(clock);
					eventQueue.ScheduleEvent(Departure, nextProcess->arrivalTime, nextProcess->serviceTime, clock + nextProcess->serviceTime);
					delete nextProcess;
				}
				break;
			default:
				std::cout << "Error processing event" << std::endl;
				break;
			}
			delete currentEvent;
		}

		myfile << avgArrivalRate << ", "
			<< totalTimeInSystem / PROCESSES_TO_RUN << ", "
			<< PROCESSES_TO_RUN / clock << ", "
			<< totalServiceTime / clock << ", "
			<< readyQueue.GetTimeWeightedSum() / clock << std::endl;

		avgArrivalRate += (MAX_AVG_ARRIVAL_RATE - MIN_AVG_ARRIVAL_RATE) / (float)SIM_NUMBER;
		progress += 1.0 / (float)SIM_NUMBER;

		if ((int)(progress * 1000) % 10 == 0)
		{
			std::cout << "[";
			int pos = progressBarWidth * progress;
			for (int i = 0; i < progressBarWidth; ++i) {
				if (i <= pos) std::cout << "=";
				else std::cout << " ";
			}
			std::cout << "] " << int(progress * 100.0) << " %\r";
			std::cout.flush();
		}

	}
	std::cout << "[";
	int pos = progressBarWidth;
	for (int i = 0; i < progressBarWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << 100 << " %\r" << std::endl;
	myfile.close();
	std::cout << "File 'ServerReport.csv' Generated" << std::endl;
}

int main()
{
	srand((unsigned int)time(0));
	int option = 0;
	while (option != -1)
	{
		std::cout <<
			"0 : Run a simulation with user input." << std::endl <<
			"1 : Run a series of simulations and print the data to file." << std::endl <<
			"2 : Quit." << std::endl <<
			"Enter an option: ";
		if (!(std::cin >> option))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			option = 666; // Option bad, send to default case
		}
		switch (option)
		{
		case 0:
			std::cout << std::endl;
			RunSim();
			std::cout << std::endl;
			break;
		case 1:
			std::cout << std::endl;
			RunDataGenSim();
			std::cout << std::endl;
			break;
		case 2:
			option = -1;
			break;
		default:
			option = 0;
			std::cout << "Error reading input." << std::endl << std::endl;
			break;
		}
	}
	std::cout << std::endl << "Exiting program." << std::endl;

	return 0;
}
