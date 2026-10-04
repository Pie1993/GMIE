#RUN for build make file
 cmake .. 

#RUN for build nano_particles executable file
 make

#RUN for start simulation with GUI
 ./nano_particles

# The idea is to chose a pattern for the simulation exploiting this GUI module 
# taken note of the seed used in order to be able to use it directly in the CORE_2_0 module
 ./nano_particles 1791110016 