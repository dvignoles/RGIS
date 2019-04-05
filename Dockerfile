FROM    ubuntu:18.10
EXPOSE 22
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y apt-utils git vim cmake python python-numpy \
                       openssh-server libnetcdf-dev netcdf-bin nco cdo \
                       libudunits2-0 libudunits2-data libudunits2-dev \
                       libexpat1 libexpat1-dev libxext-dev \
                       libmotif-common libmotif-dev gdal-bin gdal-data libgdal-dev \
                       postgis postgis-doc postgresql-client && rm -rf /var/lib/apt/lists/*

# Always use UTC on a server
RUN ln -snf /usr/share/zoneinfo/UTC /etc/localtime && echo UTC > /etc/timezone

# Install GHAAS
RUN git clone https://github.com/bmfekete/RGIS /tmp/RGIS && /tmp/RGIS/install.sh /usr/local/share && rm -rf /tmp/RGIS
ENV PATH="${PATH}:/usr/local/share/ghaas/bin:/usr/local/share/ghaas/f"


ENV  USER="compass"
ENV GROUP="compass"
ENV   UID="1000"
ENV   GID="1000"

COPY Container/startup.sh /opt/startup.sh
VOLUME /data/RGISarchive2
VOLUME /home/${USER}

RUN addgroup --gid ${GID} ${GROUP} && adduser --uid ${UID} --gid ${GID} --home /home/${USER} ${USER}

USER ${USER}:${GROUP}
WORKDIR /home/${USER}

ENTRYPOINT ["/bin/bash"]
