#Visualize Arduino

This piece of code analyzes and projects the readings and calculations of the Arduino(101)-Quadcopter on the screen using OpenGL on Windows.
It uses OpenGL 2.0.

<h4>SerialClass</h4>
This class reads the COM-port the Arduino is on, and is only used to read a line every once in a while.

<h4>Lines.cpp</h4>
The magic happens in this file. When this file is compiled together with OpenGL and SerialClass, it will ask for a COM-port and a BAUD-rate, and will try to start drawing lines.