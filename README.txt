This project reads monophonic tracks from bass and drums and outputs a graphical analysis
that can be used to answer questions about rhythm consistency through a song.

Right now it runs on the desktop, but future work will include moving it to a microcontroller
platform and using a LED display for real-time feedback.

Packages needed to build:
* aubio 
* libpng
* png++


TODO:
* add quality figures to note onsets by taking the output of the comb filter update.
* add quality figures to CSV output upon onset mismatches
* report onsets with quality figures below a threshold
* command line argument to set the quality threshold
* command line argument to change the PNG output filename 
* command line argument to set the PNG output scaling
