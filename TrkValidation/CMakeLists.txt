################################################################################
# Package: TrkValidation
################################################################################
gaudi_subdir(TrkValidation v1r0)


# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Sim/SimG4Common  Detector/DetInterface Sim/SimG4Interface)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(DD4hep COMPONENTS DDG4)
find_package(ROOT COMPONENTS MathCore GenVector Geom REQUIRED)




find_package(HepMC)
find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
find_package(FCCEDM)
find_package(DD4hep COMPONENTS DDG4 DDSegmentation DDRec REQUIRED)
find_package(PODIO)
find_package(ROOT COMPONENTS Geom MathCore Physics REQUIRED)

#gaudi_install_headers(TrkValidation)
gaudi_install_python_modules()

gaudi_add_module(TrkValidation
                 components/*.cpp
                 INCLUDE_DIRS TrkValidation HepMC Pythia8 FWCore FCCEDM PODIO DD4hep ROOT DetInterface SimG4Common SimG4Interface
                 LINK_LIBRARIES HepMC GaudiAlgLib Pythia8 FCCEDM PODIO DD4hep ${DD4hep_COMPONENT_LIBRARIES} ROOT Geant4)

include(CTest)

#gaudi_add_test(particleGun
#               FRAMEWORK options/particleGun.py
#               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
