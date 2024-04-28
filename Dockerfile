FROM ubuntu:23.10

SHELL ["/bin/bash", "-c"]

RUN apt-get update

# common tools install
RUN apt-get install -y \
    zsh build-essential \ 
    wget curl unzip git sudo \
    gcc g++ gdb make cmake 

# embedded compiler
RUN apt-get install -y \
    gcc-arm-none-eabi \
    dotnet-runtime-6.0

# Install develop tools (ssh/clangd/zsh/etc...)
RUN apt-get -y install \
    lsb-release gnupg software-properties-common 

RUN echo -e "\n" | bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && \
    ln -s /usr/bin/clangd-* /usr/bin/clangd

# Add user
RUN useradd -m creeper --shell /bin/zsh && echo "creeper:creeper" | chpasswd && adduser creeper sudo && \
    echo "creeper ALL=(ALL:ALL) NOPASSWD:ALL" >> /etc/sudoers && \
    gpasswd --add creeper dialout

USER creeper
WORKDIR /home/creeper

# install oh my zsh & change theme to af-magic
RUN wget https://gitee.com/mirrors/oh-my-zsh/raw/master/tools/install.sh -O zsh-install.sh && \
    sudo chmod +x ./zsh-install.sh && sh ./zsh-install.sh && \
    sed -i 's/ZSH_THEME=\"[a-z0-9\-]*\"/ZSH_THEME="af-magic"/g' ~/.zshrc

RUN sudo chsh -s /bin/zsh creeper