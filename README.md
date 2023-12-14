[![Mentioned in Awesome LIDAR](https://awesome.re/mentioned-badge.svg)](https://github.com/szenergy/awesome-lidar#basic-matching-algorithms) Awesome LIDAR; [![Mentioned in Awesome Robotics Libraries](https://awesome.re/mentioned-badge.svg)](http://jslee02.github.io/awesome-robotics-libraries/#3d-mapping) Awesome Robotics Libraries; [![Mentioned in Awesome Robotics](https://awesome.re/mentioned-badge.svg)](https://github.com/ahundt/awesome-robotics#point-clouds) Awesome Robotics
![banner](doc/images/banner_light.jpeg)


---
`libpointmatcher` is currently tested on our build system with the following architecture and OS
- x86 and arm64/v8
- Ubuntu bionic (18.04) and focal (20.04) 

# Documentation and Tutorials

libpointmatcher is a modular library implementing the Iterative Closest Point (ICP) algorithm for aligning point clouds. It has applications in robotics and computer vision.
The library is written in C++ for effeciency with [bindings in Python](https://github.com/norlab-ulaval/libpointmatcher/blob/master/doc/index.md#python-).
 
**Quick link for the tutorial pages: [Tutorials](doc/index.md)** (also available on [readthedocs.org](http://libpointmatcher.readthedocs.org/) but might not be up-to-date).

Those tutorials are written using Markdown syntax and stored in the project's `/doc` folder.  Their scope ranges from introductory material on performing point cloud registration to instructions for the more experienced developer on how to extend the library's codebase. 

Libpointmatcher's source code is fully documented based on doxygen to provide an easy API to developers. An example of this API can be found [here](https://norlab.ulaval.ca/libpointmatcher-doc/), but it is suggested to use the one build for your version in `doc/html`. 

libpointmatcher was orginaly developed by [François Pomerleau](mailto:f.pomerleau@gmail.com) and [Stéphane Magnenat](http://stephane.magnenat.net) as part of our work at [ASL-ETH](http://www.asl.ethz.ch).
It is now maintained by the Northern Robotics Laboratory ([Norlab](https://norlab.ulaval.ca/)), led by François Pomerleau.

You can read the latest changes in the [release notes](doc/ReleaseNotes.md).

# Quick Start

Clone the repository with its submodule
```shell
git clone --recurse-submodules https://github.com/norlab-ulaval/libpointmatcher.git

# If libpointmatcher is already cloned, fetch its new submodule 
git submodule update --remote --recursive --init
```


Although we suggest to use the [tutorials](doc/index.md), here is a quick version of it:

The library has a light dependency list:

 * [Eigen] version 3, a modern C++ matrix and linear-algebra library,
 * [boost] version 1.48 and up, portable C++ source libraries,
 * [libnabo] version 1.0.7, a fast K Nearest Neighbour library for low-dimensional spaces,
 
and was compiled on:
  * Ubuntu ([see how](/doc/CompilationUbuntu.md))
  * Mac OS X ([see how](/doc/CompilationMac.md))
  * Windows ([see how](/doc/CompilationWindows.md) - partially supported)

### Compilation & Installation 

For beginner users unfamiliar with compiling and installing a library in Linux, go [here](doc/CompilationUbuntu.md) for detailed instructions on compiling libpointmatcher from the source code.  
If you are comfortable with Linux and CMake and have already installed the prerequisites above, the following commands should install libpointmatcher on your system.

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

### Testing

Libpointmatcher ships with a version of the Google testing framework [GTest](https://github.com/google/googletest).  Unit tests are located in the `utest/` directory and are compiled with libpointmatcher (CMake variable `BUILD_TESTS` must be set to `TRUE` before compiling).  To run the tests and make sure that your compiled version is working correctly, run the test executable in your build directory:

```bash
cd build
utest/utest --path ../examples/data/
```

### Linking to external projects.

We mainly develop for __cmake projects__ and we provide example files under [`examples/demo_cmake/`](https://github.com/norlab-ulaval/libpointmatcher/tree/master/examples/demo_cmake) to help you in your own project. We also provide a __QT Creator__ example in [`examples/demo_QT/`](https://github.com/norlab-ulaval/libpointmatcher/tree/master/examples/demo_Qt), which manually lists all the dependencies in the file [`demo.pro`](https://github.com/norlab-ulaval/libpointmatcher/blob/master/examples/demo_Qt/demo.pro). You would need to ajust those paths to point at the appropriate locations on your system. For a more detailed procedure, check the [Linking Projects to libpointmatcher](doc/LinkingProjects.md) section.

## File formats

The library support different file formats for importing or exporting data:

* csv (Comma Separated Values)
* vtk (Visualization Toolkit Files)
* ply (Polygon File Format)
* pcd (Point Cloud Library Format)

Those functionnalities are available without increasing the list of dependencies at the expense of
limited functionality support. For more details, see the
tutorial [Importing and Exporting Point Clouds](doc/ImportExport.md). Example executables using
those file formats from the command line can be found in the `/examples` directory and are
described [here](doc/ICPIntro.md) in more detail.

---

# Contributing

## Bug reporting

Please use our [github's issue tracker](http://github.com/ethz-asl/libpointmatcher/issues) to report bugs. If you are running the library on Ubuntu, copy-paste the output of the script [listVersionsUbuntu.sh](https://github.com/norlab-ulaval/libpointmatcher/blob/master/utest/listVersionsUbuntu.sh) to simplify the search of an answer.

## Codebase

Libpointmatcher codebase now integrate [norlab-build-system (NBS)](https://github.com/norlab-ulaval/norlab-build-system) and [norlab-shell-script-tools (N2ST)](https://github.com/norlab-ulaval/norlab-shell-script-tools). `NBS` is a build-infrastructure-agnostic build system custom-made for our need at NorLab and `N2ST` is library of shell script, function for shell script development and a shell testing tools leveraging `bats-core`.

NBS is deployed on our [TeamCity](https://www.jetbrains.com/teamcity/) continuous integration/deployment server and oversees both protected branch of the [libpointmatcher](https://github.com/norlab-ulaval/libpointmatcher) GitHub repository: the `master` branch and the `develop` branch.

- The `develop` branch can only be merged through a pull-request from any `<feature>` branchs. Any contributor can submit a pull request to the `develop` branch;
- The `master` branch can only be merged from the `release` branch through a pull-request by a repository admin.
In both cases submiting a pull request will trigger a build configuration on our build system and the pull request will be granted if the build/test run succeede.

**Current build matrix:**
`[latest] x [x86, arm64] x [ubuntu] x [bionic, focal, jammy] x [Release, RelWithDebInfo, MinSizeRel]`

### Development workflow

To speed up the development process, you can run the build system localy on your workstation and have access to stacktrace and build log. 
It support multi-OS and multi-architecture through docker container.

#### Install _libpointmatcher-build-system_ dependencies
```shell
cd <path/to/libpointmatcher>

# If libpointmatcher is already cloned, fetch the NBS and N2ST submodule 
git submodule update --remote --recursive --init

cd ./build_system/lpm_utility_script

# Execute docker tools install script i.e. docker daemon, docker compose, docker buildx 
bash lpm_install_docker_tools.bash

# Configure a multi-architecture docker builder
bash lpm_create_multiarch_docker_builder.bash
```

#### libpointmatcher development › to execute build/test step locally
```shell
cd <path/to/libpointmatcher>/build_system

# Run the build matrix as specified in ".env.build_matrix.libpointmatcher" on native architecture using "ci_PR" service 
bash lpm_crawl_libpointmatcher_build_matrix.bash --fail-fast -- build ci_PR

# Run a specific case using build flags with multi-architecture virtualization using "ci_PR_amd64" and "ci_PR_arm64v8" services 
bash lpm_crawl_libpointmatcher_build_matrix.bash --repository-version-build-matrix-override latest \
                                                 --os-name-build-matrix-override ubuntu \
                                                 --cmake-build-type-build-matrix-override RelWithDebInfo \
                                                 --ubuntu-version-build-matrix-override focal \
                                                 --fail-fast \
                                                 -- build ci_PR_amd64 ci_PR_arm64v8

# Read the help for details
bash lpm_crawl_libpointmatcher_build_matrix.bash --help
```
Note: To assess the state of the codebase, even for cases that are known the break the build, 
execute `lpm_crawl_libpointmatcher_build_matrix.bleeding.bash` with build matrix `.env.build_matrix.libpointmatcher.bleeding`.
The stable build matrix used for release is `.env.build_matrix.libpointmatcher`.


#### Build system development 
```shell
cd <path/to/libpointmatcher>/build_system/tests/
 
# To execute docker dryrun and configuration tests
bash run_all_docker_dryrun_and_config_tests.bash

# To execute shell script tests
bash run_bats_core_test_in_n2st.bash

# To spin a container in interactive mode with the codebase cloned but not compiled  
cd ./tests_docker_interactive/
bash build_and_run_IamBuildSystemTester.bash bash
```

#### Build system notes: 
- `lpm_crawl_dependencies_build_matrix.bash` execute the build matrix for the libpointmatcher dependencies. 
   It's not required to build them locally as they are pre-build by our TeamCity server periodically push to dockerhub.
   When executing `lpm_crawl_libpointmatcher_build_matrix.bash`, the `libpointmatcher-dependencies` docker images are pull and used as base image for the `libpointmatcher-[ci_PR_test|release]` images.
- About `libpointmatcher/.github/workflow/` vs `libpointmatcher/build_system/` logic: Those are separate build logic.
  `.github/workflow/` was community contributed and as the responsibilities of building python-binding and pushing packages. 
  For this reason, it run a one-dimension build matrix: multiple python version, single OS version, single arch (x86) and
  single compile flag which GitHub action computing resources can handle just fine.

---

# Citing

If you use libpointmatcher in an academic context, please cite the following publication:

	@article{Pomerleau12comp,
		author = {Pomerleau, Fran{\c c}ois and Colas, Francis and Siegwart, Roland and Magnenat, St{\'e}phane},
		title = {{Comparing ICP Variants on Real-World Data Sets}},
		journal = {Autonomous Robots},
		year = {2013},
		volume = {34},
		number = {3},
		pages = {133--148},
		month = feb
	}

and/or

	@INPROCEEDINGS{pomerleau11tracking,
		author = {Fran{\c c}ois Pomerleau and St{\'e}phane Magnenat and Francis Colas and Ming Liu and Roland Siegwart},
		title = {Tracking a Depth Camera: Parameter Exploration for Fast ICP},
		booktitle = {Proc. of the IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS)},
		publisher = {IEEE Press},
		pages = {3824--3829},
		year = {2011}
	}

# Extra Reading

If you are interested in learning more about different registration algorithms, we recently put together a literature review surveying multiple solutions. The review is organized in the same way as the library and many examples are provided based on real deployments.

F. Pomerleau, F. Colas and R. Siegwart (2015), "_A Review of Point Cloud Registration Algorithms for Mobile Robotics_", __Foundations and Trends® in Robotics__: Vol. 4: No. 1, pp 1-104.  https://doi.org/10.1561/2300000035 

If you don't have access to the journal, you can download it from [here](https://www.researchgate.net/publication/277558596_A_Review_of_Point_Cloud_Registration_Algorithms_for_Mobile_Robotics).

# More Point Clouds

We also produced those freely available data sets to test different registration solutions:

[_Challenging data sets for point cloud registration algorithms_](http://projects.asl.ethz.ch/datasets/doku.php?id=laserregistration:laserregistration)

![alt tag](http://projects.asl.ethz.ch/datasets/lib/exe/fetch.php?cache=&media=laserregistration:asldataset_weblarge.jpg)

You can download the files in CSV or VTK formats, which are directly supported by the library I/O module. 


# Projects and Partners

If you are using libpointmatcher in your project and you would like to have it listed here, please contact [François Pomerleau](mailto:f.pomerleau@gmail.com).

 * European Project [NIFTi](http://www.nifti.eu/) (FP7 ICT-247870): Search and rescue project in dynamic environments. Results: [video of multi-floor reconstruction](http://www.youtube.com/watch?v=lP5Mj-TGaiw) and [video of railyard reconstruction](http://www.youtube.com/watch?v=ygIvzWVfPYk). All results with real-time computation.
 * NASA Ames [Stereo Pipeline](https://ti.arc.nasa.gov/tech/asr/groups/intelligent-robotics/ngt/stereo/): Planetary reconstruction from satellite observations. Results: used for Mars, Moon, and Earth point clouds.
 * Armasuisse S+T UGV research program [ARTOR](http://www.artor.ethz.ch/): Development of techniques for reliable autonomous navigation of a wheeled robot in rough, outdoor terrain. Results: [video of urban and dynamic 3D reconstruction](http://www.youtube.com/watch?v=UCCAUf64tD0) and [video of open space 3D reconstruction](http://www.youtube.com/watch?v=M5Y99o7um88) with real-time computation.
 * Swiss National Science Foundation - [Limnobotics](http://www.limnobotics.ch/): Robotic solution for toxic algae monitoring in lacs. Result: [video of 3D shore reconstruction](http://www.youtube.com/watch?v=g8l-Xq4qYeE) with real-time computation.
 * [CGAL](https://www.cgal.org/) includes our library for their registration pipeline.
 * [Norlab](https://norlab.ulaval.ca/) is maintaining and using the library for its research on autonomous navigation in harsh environments.
 * [ANYbotics AG](https://www.anybotics.com) is investigating autonomous navigation algorithms using this library.
 * [Point Laz Mining LiDAR Expert](https://www.pointlaz.com/) is scanning mine shafts to ensure infrastructure safety.

For a larger list of work realized with libpointmatcher, please see the page [Applications And Publications](/doc/ApplicationsAndPub.md).


# License

libpointmatcher is released under a permissive BSD license. Enjoy!

[Ubuntu]: http://www.ubuntu.com
[CMake]: http://www.cmake.org
[CMake documentation]: https://cmake.org/cmake/help/v3.10/
[git]: http://git-scm.com
[Eigen]: http://eigen.tuxfamily.org
[libnabo]: http://github.com/ethz-asl/libnabo
[ROS]: http://www.ros.org/
[Paraview]: http://www.paraview.org/
[yaml-cpp]: https://github.com/jbeder/yaml-cpp
[Doxygen]: https://www.doxygen.nl/index.html
[boost]: http://www.boost.org/


---


![alt tag](doc/images/banner_dark.jpeg)
