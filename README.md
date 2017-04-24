# Raspberry Pi Bare Metal Programming

[TOC]

### Code Envrionment Usage:

- ```bash
  git clone https://github.com/Piping/rpvm && cd rpvm
  #rpvm is your_work_directory now
  #docker image for first time 
  docker build -t arm docker
  make code # this will lead you to the coding environment
  make # this will look at Makefil @ your_work_directory and build
  make run # this will run qemu emulator to load and run your program
  make runimg # similar to make run but load img format(as plain binary)
  make install # unimplemented--> install files to real RP2 
  ```

- [raspberry pi docuemnts-bare metal](https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=72260)

- after `make code`, you will be in the docker container environment, `/work` is mapped to `your_work_directory` in host machine, **anything change done to other directory will not be saved**

- inside docker Ctrl-p +Ctrl-q to detach from the shell, `docker attach arm` to get back code envrionment, `Ctrl+D`or type exit to leave and destory the container 

### Git & Continuous Intergration

- The way Travis works with pull requests is that it fetches the master repository, then merges the PR on top of it locally. That means that from the working copy, just looking which files changed since the last commit actually looks at the files changed during the merge.

- [trigger test on last commit](https://github.com/DefinitelyTyped/DefinitelyTyped/issues/1314)

- ####  Git Flow vs GitHub Flow vs Gitlab Flow

  - practice **continuous delivery** means that your default branch can be deployed

  - Git Flow:

    - master / development branch seperation
    - ![Git Flow](https://about.gitlab.com/images/git_flow/gitdashflow.png)
    - difficulity arise in hotfix and patch that have to be applied to all branches

  - Github Flow:

    - master / feature branch seperation
    - ![Github Flow](https://about.gitlab.com/images/git_flow/github_flow.png)
    - Still leaves a lot of questions unanswered regarding deployments, environments, releases and integrations with issues.
    - GitHub flow does assume you are able to deploy to production every time you merge a feature branch. This is possible for web developemnt. (SaaS)

  - Gitlab Flow

    -  Use Master / Production(Release) Branch 

    - ![Simplest Form](https://about.gitlab.com/images/git_flow/production_branch.png)

    - If you need a more exact time you can have your deployment script create a tag on each deployment.

    - More Variant of Gitlab Flow

      - https://about.gitlab.com/2014/09/29/gitlab-flow/
      - ![Release Branch](https://about.gitlab.com/images/git_flow/release_branches.png)

    - Git Command:

      - ```bash
        #create new branch, git branch mycode && git checkout mycode
        git checkout -b mycode
        #now at mycode branch, edit and add files and commit, after those 
        git checkout master
        git merge mycode
        git push [origin...]
        #wait for pull requets to be reviewed, tested, approved
        ```

###  ARM CPU Family Clarify

- The **Cortex M** series all fit into the **microcontroller** family, that is they are specifically optimised for smaller, specific, time-sensitive applications. Because they have a more specific job, they typically don't have as much memory.

  The **Cortex A** series are more of a **general-purpose microprocessor** embedded type core, that you would find in a smartphone or a tablet or other [SOC's](https://en.wikipedia.org/wiki/System_on_a_chip). They typically favor higher performance and functions that are more suitable for running an OS such as Android.

- | ARM-Family       | ARM Architecture | ARM-Core   | GUN-CFLAGS |
  | ---------------- | ---------------- | ---------- | ---------- |
  | Cortex-A(32-bit) | ARMv7-A          | Cortex A7  |            |
  | Cortex-A(64-bit) | ARMv8-A          | Cortex-A53 |            |

- [Wiki-Table-Reference for Exact Relations](https://en.wikipedia.org/wiki/List_of_ARM_microarchitectures), Arm-Naming scheme is very **confusinng**, but top 2 is only processor we cares about.(Pi2 and Pi3 respectively) **Becareful** when you research for help, since there are many names like Arm7 that maybe refers for arm-family? or arm-architecture? or arm-core(the product line name)??.

- [CLAGS Manual for Cortex A7](https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html)

- [CLAGS Manual for Cortex A53](https://gcc.gnu.org/onlinedocs/gcc/AArch64-Options.htmll)

- Above flags are **NOT the SAME**

- `arm-eabi-gcc -print-multi-lib` to verify lib information

- Becareful when you try to compile the compiler from source, above difference is significant `[root:/work] 1h29m55s #` my build time.

### Machine Code Related

- **ABI**: Application binary Interface

  - Each architecture or architecture/os couple has an ABI. The ABI (Application binary Interface) describes how functions should be called, syscalls numbers, arguments passed, which registers can be used

  - an abi describes how the compiler should generate the assembler.

  - for example

    - **arm-elf** toolchain generates obj code for some OS which support executing elf format (example linux ABI). OS will control executing of your program.

      **arm-none-eabi** toolchain generates obj code for micro-controllers or microprocessors (**for bare metal**, this will be **EABI - embedded ABI**). This code are downloaded to clean flash of MC and core of MC start executing it after power-on. No OS, extended command set, no possibility for linking with shared modules.

  - Tool chains have a loose name convention like arch [-vendor][-os] - abi

  - ```
    arch -refers to target architecture (which in our case is ARM)
    vendor -refers to toolchain supplier
    os - refers to the target operating system
    gnueabi - run on Linux with default configuration (abi) 
    ```

  - [More Explaination](https://community.nxp.com/thread/313490)

- ARM Assembly [Guide](http://www.peter-cockerell.net/aalp/html/frames.html)

  - ![Computer System](http://www.peter-cockerell.net/aalp/html/images/fig1-1-hi.jpg)
  - ​
  - ​

### Emulation

- armb == arm with big endian

- gdb simulation

  - arm-eabi-gdb kernel.elf
    - `target sim`
    - `load`
    - `break <tab> _start`
    - `run`
    - `set $pc=0` to emulate software reset
    - `stepi`
    - Ctrl-D to exit
#### qemu

-  https://qemu.weilnetz.de/doc/qemu-doc.html#Display-options

  - **dtb*** stands for (device tree blob)

  - To get and build the QEMU source code for RP2

    ```bash
    git clone https://github.com/rsta2/qemu.git qemu
    cd qemu
    git submodule update --init dtc pixman

    mkdir build
    cd build
    ../configure --target-list=arm-softmmu
    make
    ```

  - These options enable specific features of QEMU

    | Option                                   | Meaning                                  |
    | ---------------------------------------- | ---------------------------------------- |
    | -global bcm2835-fb.xres=1024 -global bcm2835-fb.yres=768 | (set screen resolution)                  |
    | -serial stdio                            | (write serial output to stdout)          |
    | -d guest_errors,unimp                    | (write unimplemented feature calls to stdout) |
    | -sd filename                             | (enable SD card access to the disk image filename) |
    | -usbdevice keyboard                      | (enable USB keyboard support)            |
    | -usbdevice mouse                         | (enable USB mouse support, but mouse cursor not working) |
    | -usbdevice disk:filename                 | (enable USB mass storage device access to disk image) |

  - To run image 

    - `qemu-system-arm -M raspi2 -bios /path/to/kernel7.img`

  - To run elf format

    - `qemu-system-arm -M raspi2 -m 128M -serial stdio -kernel kernel7.elf`


### BOOT Process

- GPU(Videocore) share the peripheral bus and some interrupts, means that some interrupts are not avaiable to ARM core
- GPU starts running at reset or power on and includes code to read the first FAT partition of
  the SD Card on the MMC bus and loads a file called **bootcode.bin** into memory and starts execution of that code
- code in bootcode.bin searches for a file  called **start.elf** and a **config.txt** file to set various kernel settings before 
- After that, a **kernel.img** file which it then loads into memory at a specific address (0x8000) and starts the ARM processor executing at that memory location.
- At this point, GPU is running and ARM will execute the code in kernel.img
- The **start.elf** file contains the code that runs on the GPU to provide most of the requirements of OpenGL,etc
- For our purpose, we write our own **kernel.img** only.
- GPU has initiate the processor's clock and PLL? setting
- PI 1:
  - VC CPU Address 0x7E000000 is mapped to 0x20000000, so IO base set to 0x20000000 for the original Raspberry Pi.
- Pi 2:
  - ARM IO base set to 0x3F000000 

### C-Land

#### Freestanding Headers

- these headers belongs to compiler


- <stddef.h> to get size_t & NULL 
- <stdint.h> to get the intx_t and uintx_t datatypes to make sure exact size 
- <float.h>, <iso646.h>, <limits.h>, and <stdarg.h> headers.
- GCC actually ships a few more headers, but these are special purpose.


### Acknowledgement

- I used resources below, they are hepful
  - https://github.com/rsta2/circle
  - http://wiki.osdev.org/Raspberry_Pi_Bare_Bones
  - http://www.linaro.org/downloads/

