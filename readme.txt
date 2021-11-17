This app uses Python 3 scripts to run.
1) Install python 3 for windows.
2) Add python 3 exe forlder to windows eviromental variables PATH if needed.
3) Install "pyserial" module using "pip"
	3a) Run a cmd prompt and navigate to <python instalation forder>\Scripts and execute "pip.exe install pyserial" 
	3b) OR "python -m pip install pyserial".
4) Run RB_MagicLights app.
5) Insert the Magic Lights given IP address after configuring them to access your wifi.
6) Create a LEDs effects file using LED Matrix Studio V0.8.13 (Other versions may need a modified python script to absorb the generated files.)
This must have LEDs  effect file must be created with the exact length of leds used in the Magic Lights string, supporting a maximum of 300 LEDs.
7) Chose the LedMatrix leds effect file
8) Press send to load it to the Magic Lights.