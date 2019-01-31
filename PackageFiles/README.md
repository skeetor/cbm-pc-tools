# Installation instructions

### gtest

To create a new package for gest follow these steps:


* Create the following directory structure
    * googletest
        * github
        * build
            * Win32
            * Win64
* Clone the google test from https://github.com/google/googletest into the github directory
* Configure the project so that the 'build' directory is in the parent of your sources. For 32 bit it needs to be configured to 'Win32' for 64 bit it has to be 'Win64'. You have to do this seperately for both. Select the googletest directory as your source folder. 
    * cd to build\Win32 and run
        * cmake -G "Visual Studio 15 2017" ..\..\github\googletest
        * Run Visual Studio with the generated solution file and build both Debug and Release
    * cd to build\Win64 and run
        * cmake -G "Visual Studio 15 2017 Win64" ..\..\github\googletest
        * Run Visual Studio with the generated solution file and build both Debug and Release
* Edit the gtest.nuspec file from the PackageFiles directory and update the version (if appropriate)
* Copy the gtest.target and gtest.nuspec file to the github directory
* Go the github directory and run 'nuget pack gtest.nuspec'

That's it. You should now have a 'gtest.<version>.nupkg' in your github directory. Copy this to your package directory and configure Visual Studio to use that package source.
