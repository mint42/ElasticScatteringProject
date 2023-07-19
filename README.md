# Elastic Scattering Project

This repository serves as a log for my undergraduate thesis project which is working to verify the elastic cross-section for a high energy scattering experiment. This project started as my 2022 summer internship and is continuing into the summer of 2023. The data used comes from [Jefferson Lab](https://jlab.org)'s Hall C and this work was done under the advisment of Dr. Julie Roche. Initial results were found using [ROOT](https://root.cern)/C++, MATLAB, and they continue to be corrected for using the [SIMC](https://github.com/simc_gfortran) monte carlo simulation.

## Background on the Experiment

This project exists as a small part of the PION LT experiment that's ongoing at JLab. High energy electrons are shot on liquid hydrogen target in order to study the structure of the proton.

## Analysis Process Overview

Cuts were placed on the experimental data to control for good momentum elastic reactions, and to account for dead time and bream trips. This was compared against the Mott cross-section with a consideration for proton form factor (elastic and magnetic).

SIMC is currently being used to find the solid angle.

## Future Corrections

- Correcting events using dummy runs.
- Radiating corrective 
- Tracking efficiency 
- Dead time ~20%
- Vertex tracking

## AUTHOR

[Riley Reedy](https://github.com/mint42)
