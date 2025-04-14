# eddie_pmu_control

This is a script to control the power supply via EtherCAT connection to a PMU (Power Management Unit).

## Build

```bash
git clone https://github.com/secorolab/eddie_pmu_control.git

cd eddie_pmu_control

mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..

sudo make install
```
* This will install the `eddie_pmu_control` binary to `/usr/local/bin/eddie_pmu_control`.
* The script requires root privileges to run, so you may need to use `sudo` when executing it.

* The script uses the `SOEM` library for EtherCAT communication. Make sure you have the library installed and properly configured on your system.
* The [CMakeLists.txt](CMakeLists.txt) file assumes you have `SOEM` installed in the same path as `CMAKE_INSTALL_PREFIX`. If you have it installed in a different location, you may need to modify the `CMakeLists.txt` file accordingly.

## Usage

```bash
sudo ./eddie_pmu_control <interface> <command>
```

*   `<interface>`: The network interface to use for EtherCAT communication (e.g., `enp2s0`).
*   `<command>`: An integer representing the desired command to send to the PMU.

### Commands

*   `0`: Base and torso off
*   `1`: Base on, torso off
*   `2`: Base on (default), torso on
*   `3`: Base off, torso on
*   `4`: Base on, torso on
*   `5`: Do not shutdown PMU
*   `6`: Shutdown PMU with delay

### Example

```bash
sudo ./eddie_pmu_control enp2s0 2
```
When the base is turned on, this will turn on the power supply for the torso. 