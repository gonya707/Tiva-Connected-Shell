Tiva-Connected-Shell
====================

Shell prototype for Tiva connected TM4C1294NCPDT and compatible launchpads

This project provides a good skeleton for developing shell consoles on Tiva connected launchpads and compatible devices. 

It comes with 3 simple commands as an example.

Screenshot: http://i.imgur.com/bmShM1D.png

HOW-TO
===========================================

Just download and import it via File > Import.

You will also need to modify the properties of the project to suit your working directories. Right button the project on CCS and select properties. Then on: 

		build > arm compiler > include options 		add dir to #include search path: "C:\<TivaWare path>"

and on:

		build > arm linker > file search path		Include librart file or command file as input: 		"C:\<TivaWare path>\driverlib\ccs\Debug\driverlib.lib"

Finally right button again the proyect and under "Add files" select:

		C:\<TivaWare path>\driverlib\ccs\Debug\driverlib.lib
