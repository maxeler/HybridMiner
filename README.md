=============================
Hybrid-Coinminer
=============================

A coin miner application that can mine SHA-256 based coin and Scrypt based coin simulataneously.


How to run:
================
    ./HybridcoinMiner-DFE -t
        Run miner in test mode.
    ./HybridcoinMiner-DFE -i <SHA256 pool address> -u <username> -p <password> -l <scrypt pool address> -a <username> -k <password>
        Run miner in live mode.

If the maxfile does not contain a scrypt miner, scrypt related arguments are optional:
    ./HybridcoinMiner-DFE -i <SHA256 pool address> -u <username> -p <password>
        Run bitcoin only miner in live mode.

If the maxfile does not contain a bitcoin miner, bitcoin related arguments are optional:
    ./HybridcoinMiner-DFE -l <scrypt pool address> -a <username> -k <password>
        Run scrypt only miner in live mode.

Information to compile
======================
Ensure the environment variables below are correctly set:
  * MAXELEROSDIR
  * MAXCOMPILERDIR

This release has been built with MaxCompiler 2013.3

If you are using a MaxCompiler version greater than 2013.3, you may need to
remove the distributed maxfiles before recompiling the application. In that
case, the following command before compilation:
	make distclean

Makefile targets
================
  install
	Compiles and installs all binaries (to bin/)
  clean
	Removes results of compilation from build directories
  distclean
    Removes all results of compilation from build directories, including
    all maxfiles


Also note that different project run rules can be activated through the
RUNRULE environment variable.  The default is to build for Vectis DFEs,
which can be explicitly targeted:
	make RUNRULE=DFE install
	make RUNRULE=DFE clean
	make RUNRULE=Simulation  install
	make RUNRULE=Simulation  clean


External Libraries Required
============================
Please ensure cURL and JSON-C are installed in the machine.
To install cURL, run (with root access):
	yum install curl 
	yum install curl-devel

To install JSON-C, run (with root access):
	yum install json-c 
	yum install json-c-devel 


