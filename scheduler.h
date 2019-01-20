#include <vector>

using namespace std;

/*
	A cell in a Gantt chart that holds data.
	
	pid	- process id
	vt	- visiting time
	ct	- completion time
*/
struct GanttData
{
	GanttData(int p = 0, int v = 0, int c = 0)
	{
		pid = p;
		vt = v;
		ct = c;
	}
	
	int pid;
	int vt;
	int ct;
};

/*
	Generates a FCFS Gantt chart.
	
	gantt	- buffer for the Gantt chart
	at		- vector of arrival time
	bt		- vector of burst time
	nProc	- number of processes
*/
void fcfsGantt(vector<GanttData> & gantt, vector<int> at, vector<int> bt, int nProc)
{
	for (int p = 0; p < nProc; p++)
		gantt.push_back(GanttData(p));
	
	int temp;
	
	// Sort processes based on arrival time
	for (int i = 0; i < nProc; i++)
	{
		for (int j = i+1; j < nProc; j++)
		{
			if (at[i] > at[j])
			{
				temp = at[i];
				at[i] = at[j];
				at[j] = temp;
				temp = bt[i];
				bt[i] = bt[j];
				bt[j] = temp;
				temp = gantt[i].pid;
				gantt[i].pid = gantt[j].pid;
				gantt[j].pid = temp;
			}
		}
	}
	
	int curVt = at[0];
	for (int i = 0; i < nProc; i++)
	{
		gantt[i].vt = curVt;
		curVt += bt[i];
		gantt[i].ct = curVt;
	}
}

/*
	Recursively generates a SJF Gantt chart.
	
	gantt	- buffer for the Gantt chart
	at		- vector of arrival time
	bt		- vector of burst time
	nProc	- number of processes
	curVt	- current visiting time
	pid		- vector of process ids for keeping track of process id of at and bt
*/
void sjfGantt(vector<GanttData> & gantt, vector<int> at, vector<int> bt, int nProc, int curVt = 0, vector<int> pid = vector<int>())
{
	if (nProc == 0) return;
	
	int sjf = 0;
	int minAt = 0;
	vector<int> visited;
	
	if (pid.empty())
	{
		for (int i = 0; i < nProc; i++)
			pid.push_back(i);
	}
	
	// Find earliest arrived process
	if (curVt == 0)
	{
		minAt = at[0];
		for (int i = 0; i < nProc; i++)
		{
			if (at[i] < minAt)
				minAt = at[i];
		}
		curVt = minAt;
	}
	
	// List all processes which have arrived within current visiting time
	for (int i = 0; i < nProc; i++)
	{
		if (curVt > 0)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		else if (at[i] == minAt)
			visited.push_back(i);
	}
	
	// Find shortest job
	sjf = visited[0];
	for (int i = 0; i < visited.size(); i++)
	{
		int v = visited[i];
		if (bt[v] < bt[sjf])
			sjf = v;
		else if (bt[v] == bt[sjf] && at[v] < at[sjf])
			sjf = v;
	}
	
	GanttData g(pid[sjf], curVt);
	
	// Update current visiting time
	curVt += bt[sjf];
	g.ct = curVt;
	
	// Add found shortest job process into Gantt chart
	gantt.push_back(g);
	
	at.erase(at.begin() + sjf);
	bt.erase(bt.begin() + sjf);
	pid.erase(pid.begin() + sjf);
	
	sjfGantt(gantt, at, bt, --nProc, curVt, pid);
}

/*
	Recursively generates a SRTF Gantt chart.
	
	gantt	- buffer for the Gantt chart
	at		- vector of arrival time
	bt		- vector of burst time
	nProc	- number of processes
	curVt	- current visiting time
	pid		- vector of process ids for keeping track of process id of at and bt
*/
void srtfGantt(vector<GanttData> & gantt, vector<int> at, vector<int> bt, int nProc, int curVt = 0, vector<int> pid = vector<int>())
{
	if (nProc == 0) return;
	
	int sjf = 0;
	int nextSjf = 0;
	int minAt = 0;
	vector<int> visited;
	
	if (pid.empty())
	{
		for (int i = 0; i < nProc; i++)
			pid.push_back(i);
	}
	
	// Find earliest arrived process
	if (curVt == 0)
	{
		minAt = at[0];
		for (int i = 0; i < nProc; i++)
		{
			if (at[i] < minAt)
				minAt = at[i];
		}
		curVt = minAt;
	}
	
	// List all processes which have arrived within current visiting time
	for (int i = 0; i < nProc; i++)
	{
		if (curVt > 0)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		else if (at[i] == minAt)
			visited.push_back(i);
	}
	
	// Find shortest job
	sjf = visited[0];
	for (int i = 0; i < visited.size(); i++)
	{
		int v = visited[i];
		if (bt[v] < bt[sjf])
			sjf = v;
		else if (bt[v] == bt[sjf] && at[v] < at[sjf])
			sjf = v;
	}
	
	GanttData g(pid[sjf], curVt);
	
	// Iteratively execute shortest job process by 1ms and check for other processes that has a lower burst time
	nextSjf = sjf;
	while (sjf == nextSjf && bt[sjf] > 0)
	{
		g.ct = ++curVt;
		bt[sjf]--;
		visited.clear();
		for (int i = 0; i < nProc; i++)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		nextSjf = visited[0];
		for (int i = 0; i < visited.size(); i++)
		{
			int v = visited[i];
			if (bt[v] < bt[nextSjf])
				nextSjf = v;
			else if (bt[v] == bt[nextSjf] && at[v] < at[sjf])
				nextSjf = v;
		}
	}
	
	// Add found shortest job process into Gantt chart
	gantt.push_back(g);
	
	if (bt[sjf] == 0)
	{
		at.erase(at.begin() + sjf);
		bt.erase(bt.begin() + sjf);
		pid.erase(pid.begin() + sjf);
		nProc--;
	}
	
	srtfGantt(gantt, at, bt, nProc, curVt, pid);
}

/*
	Recursively generates a priority based Gantt chart.
	
	gantt	- buffer for the Gantt chart
	at		- vector of arrival time
	bt		- vector of burst time
	pr		- vector or priority
	nProc	- number of processes
	curVt	- current visiting time
	pid		- vector of process ids for keeping track of process id of at and bt
*/
void prGantt(vector<GanttData> & gantt, vector<int> at, vector<int> bt, vector<int> pr, int nProc, int curVt = 0, vector<int> pid = vector<int>())
{
	if (nProc == 0) return;
	
	int prio = 0;
	int minAt = 0;
	vector<int> visited;
	
	if (pid.empty())
	{
		for (int i = 0; i < nProc; i++)
			pid.push_back(i);
	}
	
	// Find earliest arrived process
	if (curVt == 0)
	{
		minAt = at[0];
		for (int i = 0; i < nProc; i++)
		{
			if (at[i] < minAt)
				minAt = at[i];
		}
		curVt = minAt;
	}
	
	// List all processes which have arrived within current visiting time
	for (int i = 0; i < nProc; i++)
	{
		if (curVt > 0)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		else if (at[i] == minAt)
			visited.push_back(i);
	}
	
	// Find highest priority
	prio = visited[0];
	for (int i = 0; i < visited.size(); i++)
	{
		int v = visited[i];
		if (pr[v] < pr[prio])
			prio = v;
		else if (pr[v] == pr[prio] && at[v] < at[prio])
			prio = v;
	}
	
	GanttData g(pid[prio], curVt);
	
	// Update current visiting time
	curVt += bt[prio];
	g.ct = curVt;
	
	// Add found process with highest priority into Gantt chart
	gantt.push_back(g);
	
	at.erase(at.begin() + prio);
	bt.erase(bt.begin() + prio);
	pr.erase(pr.begin() + prio);
	pid.erase(pid.begin() + prio);
	
	prGantt(gantt, at, bt, pr, --nProc, curVt, pid);
}

/*
	Recursively generates a priority (preemptive) based Gantt chart.
	
	gantt	- buffer for the Gantt chart
	at		- vector of arrival time
	bt		- vector of burst time
	pr		- vector or priority
	nProc	- number of processes
	curVt	- current visiting time
	pid		- vector of process ids for keeping track of process id of at and bt
*/
void prPreempGantt(vector<GanttData> & gantt, vector<int> at, vector<int> bt, vector<int> pr, int nProc, int curVt = 0, vector<int> pid = vector<int>())
{
	if (nProc == 0) return;
	
	int prio = 0;
	int nextPrio = 0;
	int minAt = 0;
	vector<int> visited;
	
	if (pid.empty())
	{
		for (int i = 0; i < nProc; i++)
			pid.push_back(i);
	}
	
	// Find earliest arrived process
	if (curVt == 0)
	{
		minAt = at[0];
		for (int i = 0; i < nProc; i++)
		{
			if (at[i] < minAt)
				minAt = at[i];
		}
		curVt = minAt;
	}
	
	// List all processes which have arrived within current visiting time
	for (int i = 0; i < nProc; i++)
	{
		if (curVt > 0)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		else if (at[i] == minAt)
			visited.push_back(i);
	}
	
	// Find highest priority
	prio = visited[0];
	for (int i = 0; i < visited.size(); i++)
	{
		int v = visited[i];
		if (pr[v] < pr[prio])
			prio = v;
		else if (pr[v] == pr[prio] && at[v] < at[prio])
			prio = v;
	}
	
	GanttData g(pid[prio], curVt);
	
	// Iteratively execute process with highest priority by 1ms and check for other processes that has a higher priority
	nextPrio = prio;
	while (prio == nextPrio && bt[prio] > 0)
	{
		g.ct = ++curVt;
		bt[prio]--;
		visited.clear();
		for (int i = 0; i < nProc; i++)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		nextPrio = visited[0];
		for (int i = 0; i < visited.size(); i++)
		{
			int v = visited[i];
			if (pr[v] < pr[nextPrio])
				nextPrio = v;
			else if (pr[v] == pr[nextPrio] && at[v] < at[prio])
				nextPrio = v;
		}
	}
	
	// Add found shortest job process into Gantt chart
	gantt.push_back(g);
	
	if (bt[prio] == 0)
	{
		at.erase(at.begin() + prio);
		bt.erase(bt.begin() + prio);
		pr.erase(pr.begin() + prio);
		pid.erase(pid.begin() + prio);
		nProc--;
	}
	
	prPreempGantt(gantt, at, bt, pr, nProc, curVt, pid);
}

/*
	Recursively generates a Round Robin Gantt chart.
	
	gantt	- buffer for the Gantt chart
	at		- vector of arrival time
	bt		- vector of burst time
	quantum	- time slice
	nProc	- number of processes
	curVt	- current visiting time
	pid		- vector of process ids for keeping track of process id of at and bt
	state	- vector of execution state of processes
*/
void rrGantt(vector<GanttData> & gantt, vector<int> at, vector<int> bt, int quantum, int nProc, int curVt = 0, vector<int> pid = vector<int>(), vector<bool> state = vector<bool>())
{
	if (nProc == 0) return;
	
	int minAt = 0;
	int oldBt = 0;
	vector<int> visited;
	
	if (pid.empty())
	{
		for (int i = 0; i < nProc; i++)
			pid.push_back(i);
	}
	
	if (state.empty())
	{
		for (int i = 0; i < nProc; i++)
			state.push_back(false);
	}
	
	// Find earliest arrived process
	for (int i = 0; i < nProc; i++)
	{
		if (at[i] < at[minAt])
			minAt = i;
	}
	
	if (curVt == 0)
		curVt = at[minAt];
	
	// List all processes which have arrived within current visiting time
	for (int i = 0; i < nProc; i++)
	{
		if (curVt > 0)
		{
			if (at[i] <= curVt)
				visited.push_back(i);
		}
		else if (at[i] == at[minAt])
			visited.push_back(i);
	}
	
	// Prioritize unexecuted process
	for (int i = 0; i < visited.size(); i++)
	{
		int v = visited[i];
		if (at[v] == at[minAt])
		{
			if (!state[v] && state[minAt])
			{
				minAt = v;
				break;
			}
		}
	}
	
	GanttData g(pid[minAt], curVt);
	
	oldBt = bt[minAt];
	bt[minAt] -= quantum;
	
	if (bt[minAt] < 0)
		bt[minAt] = 0;
	
	state[minAt] = true;
	
	curVt += oldBt - bt[minAt];
	g.ct = curVt;
	at[minAt] = curVt;
	
	gantt.push_back(g);
	
	if (bt[minAt] == 0)
	{
		at.erase(at.begin() + minAt);
		bt.erase(bt.begin() + minAt);
		pid.erase(pid.begin() + minAt);
		state.erase(state.begin() + minAt);
		nProc--;
	}
	
	rrGantt(gantt, at, bt, quantum, nProc, curVt, pid, state);
}
