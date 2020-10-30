#include <iostream>
#include "scheduler.h"

int main()
{
	while (true)
	{
		int nProc = 0;
		int choice = 0;
		int quantum = 0;
		bool bAll = false;
		float totalBt = 0;
		float totalWt = 0;
		float totalTat = 0;
		float avgWt = 0;
		float avgTat = 0;
		float cpuUtil = 0;
		vector<int> at, bt, pr, ct, wt, tat;
		vector<GanttData> gantt;
		
		while (true)
		{
			system("cls");
			cout << "Please select a CPU scheduling algorithm\n\n"
				 << "1. All\n"
				 << "2. FCFS\n"
				 << "3. SJF\n"
				 << "4. SRTF\n"
				 << "5. Priority Scheduling (non-preemptive)\n"
				 << "6. Priority Scheduling (preemptive)\n"
				 << "7. RR\n"
				 << "0. Quit\n\n"
				 << "Enter selection [0-7]: ";
			cin >> choice;
			system("cls");
			if (choice == 0)
				return 0;
			else if (choice >= 1 && choice <= 7)
			{
				bAll = choice == 1;
				break;
			}
			else
			{
				cout << "Error: Invalid choice.";
				cin.get(); cin.get();
			}
		}
		
		cout << "Number of processes: ";
		cin >> nProc;
		
		// Allocate memory
		at = vector<int>(nProc, 0);
		bt = vector<int>(nProc, 0);
		pr = vector<int>(nProc, 0);
		ct = vector<int>(nProc, 0);
		wt = vector<int>(nProc, 0);
		tat = vector<int>(nProc, 0);
		
		cout << "\nArrival Time\n";
		for (int i = 0; i < nProc; i++)
		{
			cout << (char)('A'+i) << ": ";
			cin >> at[i];
		}
		
		cout << "\nBurst Time\n";
		for (int i = 0; i < nProc; i++)
		{
			cout << (char)('A'+i) << ": ";
			cin >> bt[i];
		}
		
		if (choice == 5 || choice == 6 || bAll)
		{
			cout << "\nPriority\n";
			for (int i = 0; i < nProc; i++)
			{
				cout << (char)('A'+i) << ": ";
				cin >> pr[i];
			}
		}
		
		if (choice == 7 || bAll)
		{
			cout << "\nTime Slice: ";
			cin >> quantum;
		}
		
		system("cls");
		
		for (choice = bAll ? 2 : choice; choice <= 7; choice++)
		{
			switch(choice)
			{
			case 2:
				cout << "[FCFS]\n\n";
				fcfsGantt(gantt, at, bt, nProc);
				break;
			case 3:
				cout << "[SJF]\n\n";
				sjfGantt(gantt, at, bt, nProc);
				break;
			case 4:
				cout << "[SRTF]\n\n";
				srtfGantt(gantt, at, bt, nProc);
				break;
			case 5:
				cout << "[Priority Scheduling (non-preemptive)]\n\n";
				prGantt(gantt, at, bt, pr, nProc);
				break;
			case 6:
				cout << "[Priority Scheduling (preemptive)]\n\n";
				prPreempGantt(gantt, at, bt, pr, nProc);
				break;
			case 7:
				cout << "[RR]\n\n";
				rrGantt(gantt, at, bt, quantum, nProc);
				break;
			}
			
			for(int i = 0; i < gantt.size(); i++)
			{
				int p = gantt[i].pid;
				int c = gantt[i].ct;
				// Find last occurence to get the actual completion time
				for(int j = gantt.size()-1; j > i; j--)
				{
					if (p == gantt[j].pid)
						c = gantt[j].ct;
				}
				ct[p] = c;
			}
			
			for (int i = 0; i < nProc; i++)
			{
				tat[i] = ct[i] - at[i];
				wt[i] = tat[i] - bt[i];
				totalBt += bt[i];
				totalWt += wt[i];
				totalTat += tat[i];
			}
			
			avgWt = totalWt / nProc;
			avgTat = totalTat / nProc;
			cpuUtil = totalBt / gantt.back().ct * 100.0f;
			
			// Output CPU schedule
			if (choice == 5 || choice == 6)
				cout << "P\tAT\tBT\tPR\tCT\tWT\tTAT\n";
			else
				cout << "P\tAT\tBT\tCT\tWT\tTAT\n";
				
			for (int i = 0; i < nProc; i++)
			{
				cout << (char)('A'+i) << "\t"
					 << at[i] << "\t"
					 << bt[i] << "\t";
				if (choice == 5 || choice == 6)
					cout << pr[i] << "\t";
				cout << ct[i] << "\t"
					 << wt[i] << "\t"
					 << tat[i] << endl;
			}
			
			// Output Gantt chart
			if (gantt.size() > 0)
			{
				cout << endl;
				int l = gantt.size();
				if (gantt[0].vt != 0)
					l += 1;
				for (int i = 0; i < l; i++)
					cout << "+---";
				cout << "+\n";
				if (gantt[0].vt != 0)
					cout << "|///";
				for (int i = 0; i < gantt.size(); i++)
					cout << "| " << (char)('A'+gantt[i].pid) << " ";
				cout << "|\n";
				for (int i = 0; i < l; i++)
					cout << "+---";
				cout << "+\n";
				if (gantt[0].vt != 0)
					cout << "0   ";
				cout << gantt[0].vt;
				for (int i = 0; i < gantt.size(); i++)
				{
					int t = gantt[i].ct;
					cout << (t > 100 ? " " : t > 10 ? "  " : "   ");
					cout << t;
				}
				cout << endl;
			}
			
			cout << "\nTotal WT: " << totalWt << " ms";
			cout << "\nTotal TAT: " << totalTat << " ms\n";
			cout << "\nAverage WT: " << avgWt << " ms";
			cout << "\nAverage TAT: " << avgTat << " ms\n";
			cout << "\nCPU Util: " << cpuUtil << "%\n\n";
			
			gantt.clear();
			totalWt = 0;
			totalTat = 0;
			
			if (!bAll)
				break;
		}
		
		cin.get(); cin.get();
	}
}
