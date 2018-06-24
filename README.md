# TOUGH2Plot
A Windows Graphical User Interface to visualize the output of a TOUGH2 multiphase simulations. 
For efficiency reason, TOUGH2 needs to be modified to produce binary output files instead of regular text output. A customized TOUGH2 generates the binary outputs (PTSAT.TPL and FLUX.TPL) and the grid geometry (HGRID.TPG). PTSAT.TPL has the pressure, temperature and saturation data over the simulation time and FLUX.TPL has the fluxes between cells. These files can be found under the Example directory. 

The saturation of oil at 1.5x10^7 seconds is shown below.

![Saturation_of_Oil](images/saturation_of_oil.png)



The permeability of the formation is shown below.

![Permeability](images/permeability.png)

