```bash
#SphinxBase install
sudo apt-get install bison swig
cd ~
mkdir sphinx-source
cd sphinx-source/
git clone https://github.com/cmusphinx/sphinxbase.git
cd sphinxbase/
./autogen.sh
make
sudo make install
echo '/usr/local/lib' | sudo tee --append /etc/ld.so.conf.d/spinxbase.conf
sudo ldconfig
echo 'PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig' | sudo tee --append ~/.bashrc
echo 'export PKG_CONFIG_PATH' | sudo tee --append ~/.bashrc
source ~/.bashrc

#PocketSphinx install
cd ~/sphinx-source/
git clone https://github.com/cmusphinx/pocketsphinx.git
cd pocketsphinx
./autogen.sh 
make
sudo make install
echo '/usr/local/lib' | sudo tee --append /etc/ld.so.conf.d/pocketsphinx.conf
sudo ldconfig
echo 'PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig' | sudo tee --append ~/.bashrc
echo 'export PKG_CONFIG_PATH' | sudo tee --append ~/.bashrc
source ~/.bashrc

#ALSA install
sudo apt-get install libasound2-dev alsa-base

#Festival install
sudo apt-get install festival festival-dev

sudo apt-get install libestools-dev libestools2.4 speech-tools
```

Libraries have been installed in:
   /usr/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the `-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the `LD_RUN_PATH' environment variable
     during linking
   - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to `/etc/ld.so.conf'
