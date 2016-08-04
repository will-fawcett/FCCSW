################################################################################
# Package: PileupStudy
################################################################################
gaudi_subdir(PileupStudy v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Detector/DetInterface)

find_package(HepMC)
find_package(Pythia8 COMPONENTS pythia8 pythia8tohepmc)
find_package(FCCEDM)
find_package(DD4hep COMPONENTS DDG4 DDSegmentation REQUIRED)
find_package(PODIO)
find_package(ROOT COMPONENTS Geom REQUIRED)

gaudi_install_headers(PileupStudy)
gaudi_install_python_modules()

gaudi_add_module(PileupStudy
                 components/*.cpp
                 INCLUDE_DIRS PileupStudy HepMC Pythia8 FWCore FCCEDM PODIO DD4hep ROOT
                 LINK_LIBRARIES HepMC GaudiAlgLib Pythia8 FCCEDM PODIO DD4hep ${DD4hep_COMPONENT_LIBRARIES} ROOT)

include(CTest)

#gaudi_add_test(particleGun
#               FRAMEWORK options/particleGun.py
#               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

