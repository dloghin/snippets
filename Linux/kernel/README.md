Jetson TK1 running the latest [L4T 21.5](https://developer.nvidia.com/linux-tegra-r215)

```
~$ sudo apt install docker.io
...
The following extra packages will be installed:
 apparmor aufs-tools cgroup-lite libapparmor-perl libapparmor1 libperl5.18
 perl perl-base perl-modules
Suggested packages:
 apparmor-profiles apparmor-profiles-extra apparmor-docs apparmor-utils
 debootstrap lxc rinse perl-doc libterm-readline-gnu-perl
 libterm-readline-perl-perl libb-lint-perl libcpanplus-dist-build-perl
 libcpanplus-perl libfile-checktree-perl liblog-message-perl
 libobject-accessor-perl
The following NEW packages will be installed:
 apparmor aufs-tools cgroup-lite docker.io libapparmor-perl
The following packages will be upgraded:
 libapparmor1 libperl5.18 perl perl-base perl-modules
```

Get kernel sources and flashing tools from [L4T 21.5](https://developer.nvidia.com/embedded/dlc/l4t-Jetson-TK1-Driver-Package-R21-5)

### On Jetson TK1

```
# 1. Decompress L4T 
~$ tar xf Tegra124_Linux_R21.5.0_armhf.tbz2
~$ cd Linux_for_Tegra
#
# 2. Get kernel sources (enter tegra-l4t-r21.5 when asked for the tag)
~$ ./source_sync.sh
...
Please enter a tag to sync /home/hadoop/hadoop/tmp/Linux_for_Tegra/sources/kernel_source source to
(enter nothing to skip):
# enter tegra-l4t-r21.5
...
Please enter a tag to sync /home/hadoop/hadoop/tmp/Linux_for_Tegra/sources/u-boot_source source to
(enter nothing to skip):
# enter tegra-l4t-r21.5
...
~$ cd sources/kernel_source
#
# 3. Copy current config and make sure it is ok
~$ zcat /proc/config.gz > .config
~$ make oldconfig
#
# 4. Apply patch
~$ git apply patch_jetsontk1_docker_l4t_r25-1.diff
#
# 5. Compile kernel and modules
~$ make -j 4 zImage
~$ make modules
~$ sudo make modules_install
```

### Prepare Jetson for flashing

1. Shut-down and power-off Jetson TK1
  
2. Use the USB cable and connect a x86/64 host to the Jetson TK1. 
  
3. On Jetson TK1, press the "Force Recovery" button, press the "Power" button, release the "Power" button and then release the "Force Recovery" button.

### On a x86/64 target

Using lsusb, you should see a Nvidia device. Then use *flash.sh* tool from L4T.

```
# 1. Prepare L4T by coping the resulted zImage into kernel folder
~$ tar xf Tegra124_Linux_R21.5.0_armhf.tbz2
~$ cd Linux_for_Tegra
~$ cp PATH_TO_IMAGE/zImage kernel/
~$ sudo ./apply_binaries.sh
#
# 2. Check USB connection
~$ lsusb
Bus 001 Device 004: ID 0955:7140 NVidia Corp.
#
# 3. Write the image on Jetson TK1
~$ sudo ./flash.sh -k 6 jetson-tk1 mmcblk0p1
```

### On Jetson TK1

Reboot the board into normal mode (you may remove the USB cable)
```
~$ uname -a
~$ sudo depmod -a
```

# Links

[Patch inspiration](http://www.jarzebski.pl/files/jetsontk1/patch/patch-19.3-giduid_fix.diff)

[How to build your own kernel for Jetson TK1](https://devtalk.nvidia.com/default/topic/762653/-howto-build-own-kernel-for-jetson-tk1)

[On the error running DeviceCreate at Docker installation](https://github.com/moby/moby/issues/6325)

[Script to check Jetson TK1 kernel configuration for running Docker](https://raw.githubusercontent.com/dotcloud/docker/master/contrib/check-config.sh)
