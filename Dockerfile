FROM ubuntu:19.04

RUN apt-get update -y && \
    apt-get install -y \
    g++ \
    cmake \
    libssh-dev \
    libboost-all-dev \
    git \
    make \
    net-tools \
    iputils-ping \
    openssh-server \
    vim \
    sudo

# Add ssh service
RUN mkdir /var/run/sshd && \
    echo 'root:mapreduce' | chpasswd && \
    sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile
EXPOSE 22

# For reduce node
EXPOSE 8001
# For master node
EXPOSE 8002

RUN echo "export PATH=$PATH:/home/mapreduce/distributed_map_reduce/bin" >> /etc/environment
RUN echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/mapreduce/distributed_map_reduce/lib" >> /etc/environment

RUN useradd -m mapreduce --create-home && \
    echo 'mapreduce:mapreduce' | chpasswd && \
    adduser mapreduce sudo && \
    chown -R mapreduce /home/mapreduce

USER mapreduce
WORKDIR /home/mapreduce
#RUN git clone https://github.com/RomanMilishchuk/distributed_map_reduce && \
#    cd distributed_map_reduce && \
#    mkdir build && \
#    cd build && \
#    cmake .. -DCMAKE_BUILD_TYPE=Release && \
#    make && \
#    make install


ENTRYPOINT echo "mapreduce" | sudo -S service ssh restart && \
           export PATH=$PATH:/home/mapreduce/distributed_map_reduce/build/ && \
           export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/mapreduce/distributed_map_reduce/lib  && \
           bash
