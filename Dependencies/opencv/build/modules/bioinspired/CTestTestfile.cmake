# CMake generated Testfile for 
# Source directory: C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired
# Build directory: D:/dev/plise_work_dammit/Dependencies/opencv/build/modules/bioinspired
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(opencv_test_bioinspired "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Debug/opencv_test_bioinspiredd.exe" "--gtest_output=xml:opencv_test_bioinspired.xml")
  set_tests_properties(opencv_test_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Accuracy" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1282;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;3;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(opencv_test_bioinspired "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Release/opencv_test_bioinspired.exe" "--gtest_output=xml:opencv_test_bioinspired.xml")
  set_tests_properties(opencv_test_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Accuracy" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1282;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;3;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;0;")
else()
  add_test(opencv_test_bioinspired NOT_AVAILABLE)
endif()
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(opencv_perf_bioinspired "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Debug/opencv_perf_bioinspiredd.exe" "--gtest_output=xml:opencv_perf_bioinspired.xml")
  set_tests_properties(opencv_perf_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Performance" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/performance" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1189;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1070;ocv_add_perf_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;3;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(opencv_perf_bioinspired "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Release/opencv_perf_bioinspired.exe" "--gtest_output=xml:opencv_perf_bioinspired.xml")
  set_tests_properties(opencv_perf_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Performance" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/performance" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1189;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1070;ocv_add_perf_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;3;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;0;")
else()
  add_test(opencv_perf_bioinspired NOT_AVAILABLE)
endif()
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(opencv_sanity_bioinspired "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Debug/opencv_perf_bioinspiredd.exe" "--gtest_output=xml:opencv_perf_bioinspired.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
  set_tests_properties(opencv_sanity_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Sanity" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/sanity" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1190;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1070;ocv_add_perf_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;3;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(opencv_sanity_bioinspired "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Release/opencv_perf_bioinspired.exe" "--gtest_output=xml:opencv_perf_bioinspired.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
  set_tests_properties(opencv_sanity_bioinspired PROPERTIES  LABELS "Extra;opencv_bioinspired;Sanity" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/sanity" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1190;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1070;ocv_add_perf_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;3;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/bioinspired/CMakeLists.txt;0;")
else()
  add_test(opencv_sanity_bioinspired NOT_AVAILABLE)
endif()
