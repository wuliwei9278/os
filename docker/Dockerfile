FROM dockcross/base:latest
MAINTAINER Declan Zhang <deqzhang@ucdavis.edu>

#Download and install cross-compiler binaries
#set -o xtrace
#WORKDIR /work
RUN apt-get update && apt-get install -y zsh
RUN wget https://releases.linaro.org/components/toolchain/binaries/latest/arm-eabi/gcc-linaro-6.3.1-2017.02-x86_64_arm-eabi.tar.xz \
&&  wget https://releases.linaro.org/components/toolchain/binaries/latest/aarch64-elf/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-elf.tar.xz\
&& mkdir -p /opt/linaro/ \
&& tar xf gcc-linaro-6.3.1-2017.02-x86_64_arm-eabi.tar.xz -C /opt/linaro/ \
&& tar xf gcc-linaro-6.3.1-2017.02-x86_64_aarch64-elf.tar.xz -C /opt/linaro/ \
&& rm -f gcc-linaro-6.3.1-2017.02-x86_64_arm-eabi.tar.xz gcc-linaro-6.3.1-2017.02-x86_64_aarch64-elf.tar.xz

RUN set -x && apt-get install -y libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev && git clone https://github.com/flypie/flypie-pi-qemu qemu && cd qemu && \
	git submodule update --init dtc && ./configure --target-list=arm-softmmu && make -j$(nproc) && make install && rm -fr qemu

ENV CROSS_PATH_armv7=/opt/linaro/gcc-linaro-6.3.1-2017.02-x86_64_arm-eabi \
		CROSS_PATH_armv8=/opt/linaro/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-elf
ENV	PATH=$PATH:${CROSS_PATH_armv7}/bin:${CROSS_PATH_armv8}/bin

COPY zshrc.sh /root/.zshrc