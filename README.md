# Mobile_Switch_Center_Sim
I created this program by myself from scratch for a school project in a Wireless/Mobile communications course.

This program simulates a small mobile switch center which reads in a file with a user ID, time a phone call is made, 
the cell in which the phone call is made, and the duration of the phone call. With all this information the program 
allocates a channel to give to the user in the most efficinet way possible to avoid signal interference, just like a 
real MSC would do in a real cellular system.

The program files can be found in the Wireless-Mobile Project 2 folder.

PROGRAM INFO-------------------------------------------------------------------------------------------------------------------
a)	My channel allocation strategy (hunting strategy)
For my channel allocation strategy I just did a simple forwards or backwards parse through the channels (2-dimensianal array). 
Clusters A and B parsed the channels forward meaning from index 0-14 while cluster C parsed in reverse from 14-0. So the first
channel looked at in clusters A and B was 0 while the first channel looked at in cluster C was channel 14. The reason I did it
this way was because it was easy to pull off and I did not want to complicate things with a complex multi-function hunting 
strategy.

b)	Grade of Service and average Signal Interference Ratio
Low-input: 66 calls accepted, 0 rejected GOS = 0%, Average SIR = 68dB
High-input: 61 calls accepted, 4 rejected GOS = 6.6%, Average SIR = 58dB

In general the results for each test were reasonable, but some of it was surprising especially with the high load simulation. 
I thought the high-input simulation would yield much more calls rejected than only 4, I thought the lack of a creative hunting
sequence would have a larger effect on the ability to find a good channel for every call. The low-input simulation on the 
other hand was much more predictable, it was really not that much stress on this system given the amount of channels available,
so it was no surprise when no calls were rejected even with the simple allocation strategy.

c)	Sysem behavior
For the most part the system did behave as intended, but it was a big program so there were a few bugs that I had to deal 
with at first. The priority queue was a huge help, I first tried to implement it a different way using a regular queue and
it was a nightmare trying to organize the data in the queue by time. I was receiving call request in incorrect order and it
was messing up my whole simulation. Thanks to the priority queue and its data organizing I was able to get the program
functioning correctly. Another thing that I had trouble with the first time was implementing the function to get the SIR of 
a channel. I was getting weird numbers at first but later figured out it was due to a Integer to Float conversion problem so
that was easily fixed. In its current state the system outputs every new call with its call number, cell number, time of call,
duration, number of interferers, SIR, if it was accepted or denied, and to which channel it was connected to. The system also 
outputs whenever a call is disconnected and specifies which channel is now available. All this output is as intended and 
everything seems to be working fine.

d)	How could this system be improved (so that it offered a lower GOS, or a higher average SIR, or both)?
If I was to improve this system, I would definitely invest time in coming up with a more elaborate channel allocation method.
My method was too simple and with a better hunting sequence I may have been able to get even more efficient results in both 
GOS and SIR, particularly with the high-input test. Also my data structure for the channel usage matrix was not so great.
I had trouble finding the perfect way to store that data and being able to use it across the whole system without it having 
to be so tedious to get that information. This improvement would be mainly to simplify and shorten the program and it would 
probably not have much of an effect on the GOS or SIR.
