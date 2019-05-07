This branch is a forked version that aims to streamline and improve the existing BerconMaps project. 
Intended platforms are 3DS Max 2018+.
Testing platform is an AMD CPU running Windows 10 (with latest feature updates).

--  Remove redundant custom control curve (it's currently crashing anything after Max 2017)

--  Optimize code and implement basic AVX support

--  Fix Resource ID issues

--  Fix unintended behavior when used in the Slate Material editor

Fork date: March 26, 2018
Update: May 2019 // Resuming work on this project

BerconMaps
==========

BerconMaps is a 3rd party texture plugin for Autodesk 3ds Max. It adds five new prodecural texture maps: Noise, Wood, Tile, Distortion and Gradient.

For more information, pictures and binaries go to http://www.ylilammi.com/2013/09/berconmaps/

License
-------------------------

Apache License, Version 2.0

Read LICENSE file

Contributing
-------------------------

- Fork
- Improve
- Submit a pull request

Credits
-------------------------

- Jerry Ylilammi (author)
- Vladimir Koylazov (bug fixes and support for 3ds Max 2015)
- Ken Perlin (Perlin/Simplex noise)
- Stefan Gustavson (C++ implementation of Perlin and Simplex noises)
- Steven Worley (Worley noise and C implementation of it)
- Blender team (Fractals)
- John Burnett (Distortions max shadecontext implementation)
- David Baker (Compiled Max 8 version of the plugin and edited source codes for Max 8)
- Vladislav Gavrilov (Helped with integrating my custom gradient control into Max)
