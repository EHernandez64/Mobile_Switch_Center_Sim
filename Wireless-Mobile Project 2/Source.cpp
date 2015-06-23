//Enrique Hernandez
//CS 4596
//Project #2 
//Mobile Switching Center Simulation
//Due: 11/13/2014

#include <iostream>;
#include <fstream>;
#include <string>;
#include <iomanip>;
#include <math.h>;
#include <queue>;

using namespace std;

//Distance Table as a two-dimensinal array as a Global
int Dtable[9][9] = 
{  
	{0,2000,2000,4000,6000,4000,4000,6000,6000}, //1
	{2000,0,2000,2000,4000,2000,4000,4000,6000}, //2
	{2000,2000,0,4000,4000,2000,2000,4000,4000}, //3
	{4000,2000,4000,0,2000,2000,4000,4000,6000}, //4
	{6000,4000,4000,2000,0,2000,4000,2000,4000}, //5
	{4000,2000,2000,2000,2000,0,2000,2000,4000}, //6
	{4000,4000,2000,4000,4000,2000,0,2000,2000}, //7
	{6000,4000,4000,4000,2000,2000,2000,0,2000}, //8
	{6000,6000,4000,6000,4000,4000,2000,2000,0}  //9
};

//This struct represents an Event in the Queue
struct event{
	char type;     //call or disconnect
	int num;	   //call id
	int time;      //at what time is the call or disconnect made
	int cell;      //in what cell is the call or disconnect made
	int duration;  //Duration of the call (0 for a disconnect)
	int ch;		   //Channel to disconnect or connect
};
//This class is used to organize the Event Q by time
class CompTime {
public:
    bool operator()(event& e1, event& e2)
    {  //If there is a tie the lower call number goes first
	   if((e1.time == e2.time) && (e1.num > e2.num)) return true;
	   //Otherwise the lowest time is always at the top
       if (e1.time > e2.time) return true;
       return false;
    }
};

//Priority queue used to store the events
	priority_queue<event, vector<event>, CompTime> eventQ;
//Channel Avail Tables-> each cluster has their own channels (15 total)
	int aTable [5][3];
	int bTable [5][3];
	int cTable [5][3];

//Function prototypes
bool procCall ( event &e);  //Process call
bool huntSeq (event &e);	//Hunting sequences
bool chAvail (int (&table)[5][3],int seqNo, int startCh, event &e); //Check if Channel available
float chSIR(int (&table)[5][3], int ch, event &e);			//Calculate the Signal Inteference

float AveSIR = 0.0;  // Global Var Used to get the average SIR

int main(){
	//Initiallize all channels to Available (-1)
	for(int i = 0; i<5; i++)
		for(int j = 0; j<3; j++){
			aTable[i][j] = -1;
			bTable[i][j] = -1;
			cTable[i][j] = -1;
		}
	//Counters to Collect Stats
	int callsD = 0,callsA = 0;
	string ignoredL;
	int skipN;
	//Temporary variables used to read in file
	char typ;
	int num,ti,cell, dur;

	ifstream inFile;
	inFile.open("input-high.txt");

	getline(inFile,ignoredL);

	//Store all call events in The Queue
	while(! inFile.eof()){

		inFile>>num;
		inFile>>ti;
		inFile>>cell;
		inFile>>dur;

		event temp = {'c',num,ti,(cell-1),dur,-1};
		eventQ.push(temp);
	}
	inFile.close();

/*//Used To view the content in the queue
	while( !eventQ.empty()){

		event e1 = eventQ.top();
		cout << setw(3) << e1.num << " " << setw(3) << e1.time << " " <<
			setw(3) << e1.cell << " "<< setw(3)<<e1.duration<< endl;
		eventQ.pop();
	}
*/
	//Loop to process everything inside of the Queue
	while(!eventQ.empty()){
		event e = eventQ.top();
		if(procCall(e)){
			if(e.type == 'd') //process a disconnect
				eventQ.pop();
			else{
			callsA++;
	//Add a new event to the Queue if a call is successfully made
			event temp2 = {'d',e.num,(e.time+e.duration),e.cell,0,e.ch};
			eventQ.pop();
			eventQ.push(temp2);
			}
		}
		else{
			callsD++;
			eventQ.pop();
		}
	}
	//Output All the Stats Stored during this simulation
	float GOS = 0.0;
	GOS = (float)callsD/(float)callsA;
	AveSIR = AveSIR/(callsA+callsD);
	printf("\nTotals: %d calls accepted, %d calls rejected\n",callsA,callsD);
	cout<<setprecision(2)<<"GOS = "<<(GOS*100)<<"%  Average SIR : "<<floor(AveSIR)<<"dB"<<endl;

	//Used to view console output
	system("Pause");
	return 0;
}
//Process Events
bool procCall ( event &e){
	//If you have a call event output the information of the call and send it to huntSeq
	if(e.type == 'c'){
		cout<<"*********************************************************"<<endl;
		printf("New Call: Number= %d, Cell = %d, Time = %d Duration = %d",e.num,e.cell,e.time,e.duration);
		if(huntSeq(e) == true){
			cout<<", Accepted"<<endl;
			return true;
		}
		else{
			cout<<", Denied"<<endl;
			return false;
		}
	}
	//If you have a disconnect event Change the values in the usage table back to available
	if(e.type == 'd'){
		int cellCh = 0;
		if(e.cell == 0 || e.cell == 5 || e.cell == 8){
			for(int i = 0; i < 3; i++)
				if(aTable[e.ch][i] == e.cell)
					aTable[e.ch][i] = -1;
		}
		else if(e.cell == 1 || e.cell == 4 || e.cell == 6){
			cellCh = 5;
			for(int i = 0; i < 3; i++)
				if(bTable[e.ch][i] == e.cell)
					bTable[e.ch][i] = -1;
		}

		else{
			cellCh = 10;
			for(int i = 0; i < 3; i++)
				if(cTable[e.ch][i] == e.cell)
					cTable[e.ch][i] = -1;
		}
		//Output to show that a call has been disconnected, and channel is now available
		cout<<"========================================================================="<<endl;
		printf("Call: %d in Cell: %d disconnected at Time: %d. Channel %d now available\n",e.num, e.cell,e.time, e.ch + cellCh);
		return true;
	}
	cout<<"Error: call type not a call or disconnect"<<endl;
	return false;
}
//Differnet Hunting sequences for each cluster
bool huntSeq (event &e){
	bool found = false;
	//The following call shows what I am sending to the next function:
	//chAvail(channel table, sequence number, channel to start at, event)
	//Cluster A 
	if(e.cell == 0)
		found = chAvail(aTable,1,0,e);
	if(e.cell == 1)
		found = chAvail(bTable,1,5,e);
	if(e.cell == 2)
		found = chAvail(cTable,1,10,e);
	//Cluster B 
	if(e.cell == 3)
		found = chAvail(cTable,0,10,e);
	if(e.cell == 4)
		found = chAvail(bTable,0,5,e);
	if(e.cell == 5)
		found = chAvail(aTable,0,0,e);
	//Cluster C 
	if(e.cell == 6)
		found = chAvail(bTable,0,5,e);
	if(e.cell == 7)
		found = chAvail(cTable,0,10,e);
	if(e.cell == 8)
		found = chAvail(aTable,0,0,e);
	return found;
}
//Check if the channel is available
bool chAvail (int (&table)[5][3],int seqNo,int startCh, event &e){
	float SIR = 0.0;
	int inCh = 0;
	//Each seqNo identifies a certain hunting sequence
	if(seqNo == 0){
		//Looking for calls with the same channel in the same cell
		for(int i =0; i< 5; i++){
			inCh = 0;
			for(int j=0; j<3;j++){
				if(table[i][j] == e.cell)
					inCh = 1;
			}
		//If this channel does not contain a call in the same cell
		if(inCh != 1){              
			SIR = chSIR(table,i,e); //check the SIR of that channel
			if(SIR > 22){           //If the SIR is good enough, assign the channel
				printf("Connected to Channel: %d\nSIR = %f\t",i+startCh,SIR);
				AveSIR= AveSIR + SIR; //Collecting stats
				for(int j = 0; j <3; j++) //assign the call to that channel in the table
					if(table[i][j] == -1){
						table[i][j] = e.cell;
						e.ch = i;
						return true; //Return true to indicate sucessfull channel allocation
					}
				}
			else
				printf("SIR too low on Channel %d, SIR = %f\n",i+startCh,SIR);
			}
		}
	}
	//Another Hunting sequence: Same thing happend as above but parsing backwards
	if(seqNo == 1){
		
		for(int i = 4; i >= 0; i--){
			inCh = 0;
			for(int j=2; j >= 0;j--){
				if(table[i][j] == e.cell)
					inCh = 1;
			}
		if(inCh != 1){
			SIR = chSIR(table,i,e);
			if(SIR > 22){
				printf("\tConnected to Channel: %d\nSIR = %f\t",i+startCh,SIR);
				AveSIR= AveSIR + SIR;
				for(int j = 0; j <3; j++)
					if(table[i][j] == -1){
						table[i][j] = e.cell;
						e.ch = i;
						return true;
					}		
				}
			else
				printf("SIR too low on Channel %d, SIR = %f\n",i+startCh,SIR);
			}
		}
	}
	//If no channel found for the call, Reject this call and return false
	printf("\nNo Channel was found for Call: %d in Cell: %d ", e.num,e.cell);
	return false;
}
//Calculate How much interference is in a Channel
float chSIR(int (&table)[5][3], int ch, event &e){
	int chUsage[3] = {-1,-1,-1}; //holds info of all interfering channels
	int interCount = 0; //holds the number of interferers
	float SI = 0.0,len1 = 0.0, len2 = 0.0; //Temps to store data
	//get the values of the cells using this channel
	int j = 0;
	for(int i = 0; i < 3; i++){
		if(table[ch][i] != -1){
			chUsage[j] = table[ch][i];
			j++;
		}
	}
	//Count the number of interferers
	for(int i = 0; i <3; i++)
		if(chUsage[i] != -1)
			interCount = interCount +1;
	//If there are no interferers Return SIR of 35
	if(interCount == 0){
		cout<<"\nInterferers = None ->";
		AveSIR += 35.0;
		return 35.0;
	}
	//If there is a same channel same cell problem return 0
	for(int i = 0; i < 3; i++)
		if(e.cell == chUsage[i]){
			cout<<"\nInterference from the same channel and cell, check another Channel...";
			return 0.0;
		}
	//If there is one interferer use the SIR formula
	if(interCount == 1){
		printf("\nInterferers = 1 in Cell: %d ",chUsage[0]);
		len1 = Dtable[e.cell][chUsage[0]];
		cout<<", Len1: "<<len1<<endl;
		SI = pow(1000.0,-4.0)/pow(len1,-4.0);
		SI = 10 * log(SI);
		return SI;
	}
	//If there are two interferers use the SIR formula
	else if (interCount == 2){
		printf("\nInterferers = 2 in Cells: %d and %d ", chUsage[0], chUsage[1]);
		len1 = Dtable[e.cell][chUsage[0]];
		len2 = Dtable[e.cell][chUsage[1]];
		cout<<", len1: "<<len1<<", len2: "<<len2<<endl;
		SI = pow(1000.0, -4.0) / (pow(len1, -4.0) + pow(len2, -4.0));
		SI = 10 * log(SI);
		return SI;
	}
	//if more than 3 interferers there is an error
	else
		cout << "Too many interference channels counted" << endl;
	
	return 0;
}
